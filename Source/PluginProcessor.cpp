#include "PluginProcessor.h"
#include "PluginEditor.h"

bool AuraVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<AuraSound*>(sound) != nullptr;
}

void AuraVoice::setSample(const juce::AudioBuffer<float>* source, double sourceRate)
{
    sampleBuffer = source;
    sampleRate = sourceRate > 0.0 ? sourceRate : 44100.0;
}

void AuraVoice::setPlanetTone(float b, float bo, float s)
{
    brightness = juce::jlimit(0.0f, 1.0f, b);
    body = juce::jlimit(0.0f, 1.0f, bo);
    space = juce::jlimit(0.0f, 1.0f, s);
}

void AuraVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    if (sampleBuffer == nullptr || sampleBuffer->getNumSamples() < 2)
    {
        playing = false;
        return;
    }

    position = 0.0;
    const double semitones = static_cast<double>(midiNoteNumber - 60);
    increment = (sampleRate / getSampleRate()) * std::pow(2.0, semitones / 12.0);
    velocityGain = velocity;
    level = 1.0f;
    lastL = lastR = 0.0f;
    playing = true;
}

void AuraVoice::stopNote(float, bool)
{
    // One-shot mode: MIDI note-off does not cut the sample.
}

void AuraVoice::renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples)
{
    if (!playing || sampleBuffer == nullptr)
        return;

    const int sourceChannels = sampleBuffer->getNumChannels();
    const int sourceLength = sampleBuffer->getNumSamples();
    const int outChannels = output.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        const int idx = static_cast<int>(position);
        if (idx >= sourceLength - 1)
        {
            playing = false;
            clearCurrentNote();
            break;
        }

        const float frac = static_cast<float>(position - idx);
        auto interp = [&](int ch)
        {
            const int c = juce::jmin(ch, sourceChannels - 1);
            const float a = sampleBuffer->getSample(c, idx);
            const float b = sampleBuffer->getSample(c, idx + 1);
            return a + (b - a) * frac;
        };

        float l = interp(0);
        float r = sourceChannels > 1 ? interp(1) : l;

        // Planet tone: body adds gentle low-mid weight; brightness blends a subtle edge.
        const float bodyGain = 0.82f + body * 0.42f;
        l *= bodyGain; r *= bodyGain;
        l = lastL + (l - lastL) * (0.15f + brightness * 0.82f);
        r = lastR + (r - lastR) * (0.15f + brightness * 0.82f);
        lastL = l; lastR = r;

        const float fade = juce::jmin(1.0f, static_cast<float>(sourceLength - idx) / 64.0f);
        const float gain = velocityGain * level * fade;
        if (outChannels > 0) output.addSample(0, startSample + i, l * gain);
        if (outChannels > 1) output.addSample(1, startSample + i, r * gain);

        position += increment;
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout AURAAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("brightness", "Brightness", 0.0f, 1.0f, 0.65f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("body", "Body", 0.0f, 1.0f, 0.50f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("space", "Orbital Space", 0.0f, 1.0f, 0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("gravity", "Gravity", 0.0f, 1.0f, 0.68f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("atmosphere", "Atmosphere", 0.0f, 1.0f, 0.72f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("temperature", "Temperature", 0.0f, 1.0f, 0.45f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.70f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverb", "Reverb", 0.0f, 1.0f, 0.35f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.1f, 8.0f, 1.8f));
    return layout;
}

AURAAudioProcessor::AURAAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "AURA_STATE", createParameterLayout())
{
    formats.registerBasicFormats();
    for (int i = 0; i < 16; ++i) synth.addVoice(new AuraVoice());
    synth.addSound(new AuraSound());
}

void AURAAudioProcessor::prepareToPlay(double sr, int)
{
    synth.setCurrentPlaybackSampleRate(sr);
    refreshVoices();
}

bool AURAAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AURAAudioProcessor::refreshVoices()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<AuraVoice*>(synth.getVoice(i)))
        {
            v->setSample(&sample, sampleRateStored);
            v->setPlanetTone(parameters.getRawParameterValue("brightness")->load(),
                             parameters.getRawParameterValue("body")->load(),
                             parameters.getRawParameterValue("space")->load());
        }
}

void AURAAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    refreshVoices();
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());
}

bool AURAAudioProcessor::loadSample(const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
    if (reader == nullptr || reader->lengthInSamples <= 0)
        return false;

    const auto length = static_cast<int>(juce::jmin<int64>(reader->lengthInSamples, 60LL * 60LL * 192000LL));
    juce::AudioBuffer<float> loaded(static_cast<int>(reader->numChannels), length);
    reader->read(&loaded, 0, length, 0, true, true);

    sample = std::move(loaded);
    sampleRateStored = reader->sampleRate;
    sampleName = file.getFileName();
    samplePath = file.getFullPathName();
    refreshVoices();
    return true;
}

void AURAAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    state.setProperty("samplePath", samplePath, nullptr);
    if (samplePath.isNotEmpty())
        state.setProperty("sampleName", sampleName, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AURAAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        const auto path = state.getProperty("samplePath").toString();
        parameters.replaceState(state);
        if (path.isNotEmpty())
            loadSample(juce::File(path));
    }
}

juce::AudioProcessorEditor* AURAAudioProcessor::createEditor()
{
    return new AURAAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AURAAudioProcessor();
}
