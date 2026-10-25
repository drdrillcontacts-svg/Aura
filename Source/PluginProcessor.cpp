#include "PluginProcessor.h"

//==============================================================================
// AuraVoice
//==============================================================================
void AuraVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    AURAAudioProcessor* p = processor;
    if (p == nullptr)
    {
        state = State::done;
        return;
    }

    const auto* buf = p->activeSample.load(std::memory_order_acquire);
    if (buf == nullptr || buf->getNumSamples() < 4)
    {
        state = State::done;
        return;
    }

    sampleBuf = buf;
    storedRate = p->sampleRateStored > 0.0 ? p->sampleRateStored : 44100.0;

    auto raw = [p](const char* id) { return p->parameters.getRawParameterValue(id)->load(); };
    const int rootNote = static_cast<int>(raw("rootNote"));
    const double semitones = static_cast<double>(midiNoteNumber - rootNote);

    double inc = (getSampleRate() / storedRate) * std::pow(2.0, semitones / 12.0);
    inc = juce::jlimit(0.25, 8.0, inc); // aliasing guard, documented in README
    increment = inc;

    const float velDepth = static_cast<float>(raw("velDepth"));
    const float exponent = 1.0f + (1.0f - velDepth) * 1.5f;
    velocityGain = std::pow(juce::jlimit(0.0f, 1.0f, velocity), exponent);

    const double sr = getSampleRate();
    const double aSec = juce::jmax(0.001, static_cast<double>(raw("attack")));
    const double dSec = juce::jmax(0.005, static_cast<double>(raw("decay")));
    const double rSec = juce::jmax(0.005, static_cast<double>(raw("release")));
    sustainLevel = static_cast<float>(raw("sustain"));
    attackCoef  = static_cast<float>(1.0 - std::exp(-3.0 / (aSec * sr)));
    decayCoef   = static_cast<float>(1.0 - std::exp(-4.0 / (dSec * sr)));
    releaseCoef = static_cast<float>(1.0 - std::exp(-4.0 / (rSec * sr)));

    position = 0.0;
    env = 0.0f;
    state = State::attack;
    currentMidiNote = midiNoteNumber;
}

void AuraVoice::stopNote(float, bool)
{
    if (state == State::attack || state == State::decay || state == State::sustain)
        state = State::release;
}

namespace
{
    // Catmull-Rom cubic interpolation on the integer lattice around i.
    inline float cubicSample(const float* d, int i, int length)
    {
        const int im1 = juce::jmax(0, i - 1);
        const int ip1 = juce::jmin(length - 1, i + 1);
        const int ip2 = juce::jmin(length - 1, i + 2);
        const float p0 = d[im1];
        const float p1 = d[i];
        const float p2 = d[ip1];
        const float p3 = d[ip2];
        return p1 + 0.5f * (p2 - p0 + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3));
    }
}

void AuraVoice::renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples)
{
    if (state == State::done || sampleBuf == nullptr)
        return;

    const int srcChannels = sampleBuf->getNumChannels();
    const int srcLength = sampleBuf->getNumSamples();
    const float* dataL = sampleBuf->getReadPointer(0);
    const float* dataR = srcChannels > 1 ? sampleBuf->getReadPointer(1) : dataL;
    const int outChannels = output.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        // envelope
        switch (state)
        {
            case State::attack:
                env += (1.0f - env) * attackCoef;
                if (env >= 1.0f - attackCoef * 1.5f) state = State::decay;
                break;
            case State::decay:
                env += (sustainLevel - env) * decayCoef;
                if (sustainLevel >= 0.0f && env <= sustainLevel + decayCoef * 0.5f)
                    state = State::sustain;
                break;
            case State::sustain:
                env += (sustainLevel - env) * decayCoef;
                break;
            case State::release:
                env += (0.0f - env) * releaseCoef;
                if (env < 1e-4f)
                {
                    env = 0.0f;
                    state = State::done;
                    break;
                }
                break;
            case State::done:
                break;
        }
        if (state == State::done)
        {
            clearCurrentNote();
            return;
        }

        const int idx = static_cast<int>(position);

        // Sample material exhausted: force a release tail, then stop when quiet.
        if (idx >= srcLength - 1)
        {
            if (state != State::release)
                state = State::release;
            if (env < 0.01f)
            {
                env = 0.0f;
                state = State::done;
                clearCurrentNote();
                return;
            }
            position += increment;
            continue; // render silence during the (silent) release tail
        }

        const int iClamped = juce::jmin(idx, srcLength - 2);
        const float frac = static_cast<float>(position - iClamped);
        const float l0 = cubicSample(dataL, iClamped, srcLength);
        const float l1 = cubicSample(dataL, iClamped + 1, srcLength);
        const float r0 = cubicSample(dataR, iClamped, srcLength);
        const float r1 = cubicSample(dataR, iClamped + 1, srcLength);
        const float l = l0 + (l1 - l0) * frac;
        const float r = r0 + (r1 - r0) * frac;

        // fade the last 48 source samples to avoid a termination click
        const float tail = position < srcLength - 1.0
            ? 1.0f
            : juce::jmax(0.0f, 1.0f - static_cast<float>((position - (srcLength - 1)) / (48.0 * increment + 1e-6)));

        const float gain = velocityGain * env * tail;
        if (outChannels > 0)
            output.addSample(0, startSample + i, l * gain);
        if (outChannels > 1)
            output.addSample(1, startSample + i, r * gain);

        position += increment;
    }
}

//==============================================================================
// Parameter layout
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AURAAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterChoice>("planet", "Planet",
        juce::StringArray { "Mercury", "Venus", "Earth", "Mars", "Jupiter",
                            "Saturn", "Uranus", "Neptune", "Pluto" }, 2));
    layout.add(std::make_unique<juce::AudioParameterChoice>("mode", "Mode",
        juce::StringArray { "Realistic", "Cinematic", "Alien" }, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>("gravity", "Gravity", 0.0f, 1.0f, 0.68f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("atmosphere", "Atmosphere", 0.0f, 1.0f, 0.50f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("temperature", "Temperature", 0.0f, 1.0f, 0.45f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("density", "Density", 0.0f, 1.0f, 0.60f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("brightness", "Brightness", 0.0f, 1.0f, 0.62f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("harmonics", "Harmonics", 0.0f, 1.0f, 0.48f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverb", "Reverb", 0.0f, 1.0f, 0.35f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.70f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("orbSize", "Orbital Size", 0.0f, 1.0f, 0.68f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("orbDecay", "Orbital Decay",
        juce::NormalisableRange<float>(0.2f, 8.0f), 1.8f));
    layout.add(std::make_unique<juce::AudioParameterInt>("rootNote", "Root Note", 21, 108, 60));
    layout.add(std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 1.5f), 0.002f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("decay", "Decay",
        juce::NormalisableRange<float>(0.01f, 4.0f, 0.01f, 1.5f), 0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("release", "Release",
        juce::NormalisableRange<float>(0.01f, 8.0f, 0.01f, 1.5f), 1.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("velDepth", "Velocity", 0.0f, 1.0f, 1.0f));
    return layout;
}

//==============================================================================
AURAAudioProcessor::AURAAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "AURA_STATE", createParameterLayout())
{
    formats.registerBasicFormats(); // WAV, AIFF, FLAC, OGG (built-in readers)
    for (int i = 0; i < maxVoices; ++i)
    {
        auto* v = new AuraVoice();
        v->processor = this;
        synth.addVoice(v);
    }
    synth.addSound(new AuraSound());
    loaderAlive = std::make_shared<std::atomic<bool>>(true);
    loader = std::make_unique<SampleLoader>(*this);
    loader->startThread();
}

AURAAudioProcessor::~AURAAudioProcessor()
{
    loaderAlive->store(false);
    loader.reset();
    activeSample.store(nullptr);
}

void AURAAudioProcessor::prepareToPlay(double sampleRate, int)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    chain.prepare(sampleRate);
    auto raw = [this](const char* id) { return parameters.getRawParameterValue(id)->load(); };
    chain.setTargets(static_cast<float>(raw("gravity")), static_cast<float>(raw("atmosphere")),
                     static_cast<float>(raw("temperature")), static_cast<float>(raw("density")),
                     static_cast<float>(raw("brightness")), static_cast<float>(raw("harmonics")),
                     static_cast<float>(raw("reverb")), static_cast<float>(raw("width")),
                     static_cast<float>(raw("orbSize")), static_cast<float>(raw("orbDecay")),
                     static_cast<float>(raw("velDepth")),
                     static_cast<int>(raw("planet")), static_cast<int>(raw("mode")));
    chain.settle();
    chain.reset();
}

bool AURAAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AURAAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    auto raw = [this](const char* id) { return parameters.getRawParameterValue(id)->load(); };
    chain.setTargets(static_cast<float>(raw("gravity")), static_cast<float>(raw("atmosphere")),
                     static_cast<float>(raw("temperature")), static_cast<float>(raw("density")),
                     static_cast<float>(raw("brightness")), static_cast<float>(raw("harmonics")),
                     static_cast<float>(raw("reverb")), static_cast<float>(raw("width")),
                     static_cast<float>(raw("orbSize")), static_cast<float>(raw("orbDecay")),
                     static_cast<float>(raw("velDepth")),
                     static_cast<int>(raw("planet")), static_cast<int>(raw("mode")));
    chain.designBlock();

    // track input activity (for the IN indicator)
    for (const auto& message : midi)
    {
        const uint8* d = message.getMessage().getRawData();
        const auto status = d[0] & 0xf0;
        if (status == 0x90 && d[2] > 0)
        {
            const float v = static_cast<float>(d[2]) / 127.0f;
            if (v > inputActivity.load(std::memory_order_relaxed))
                inputActivity.store(v, std::memory_order_relaxed);
        }
    }

    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    // publish the set of sounding notes for the keyboard highlight
    uint64_t maskLo = 0, maskHi = 0;
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        auto* v = synth.getVoice(i);
        if (v != nullptr && v->isVoiceActive())
        {
            const int n = v->getCurrentlyPlayingNote();
            if (n >= 0 && n < 128)
            {
                if (n < 64) maskLo |= 1ULL << n;
                else        maskHi |= 1ULL << (n - 64);
            }
        }
    }
    activeNotes[0].store(maskLo, std::memory_order_relaxed);
    activeNotes[1].store(maskHi, std::memory_order_relaxed);

    // The chain is a single stereo unit: each sample pair must be processed
    // exactly once. (Processing per output channel would feed the already
    // processed left channel back through every stage a second time.)
    const int numSamples = buffer.getNumSamples();
    float* dataL = buffer.getWritePointer(0);
    float* dataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
    for (int i = 0; i < numSamples; ++i)
    {
        float l = dataL[i];
        float r = (dataR != nullptr) ? dataR[i] : l;
        chain.processSample(l, r);
        dataL[i] = l;
        if (dataR != nullptr)
            dataR[i] = r;
    }
    chain.endBlock();
}

//==============================================================================
// Sample loading
//==============================================================================
namespace
{
    constexpr long long maxSamplesToLoad = 12000000; // ~9 min @ 44.1k stereo

    struct Decoded
    {
        std::unique_ptr<juce::AudioBuffer<float>> buffer;
        double sampleRate = 44100.0;
    };

    Decoded decodeFile(juce::AudioFormatManager& formats, const juce::File& file, juce::String& error)
    {
        Decoded out;
        std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
        if (reader == nullptr)
        {
            error = "Unsupported or unreadable audio format.";
            return out;
        }
        if (reader->lengthInSamples <= 0)
        {
            error = "File contains no audio data.";
            return out;
        }
        out.sampleRate = reader->sampleRate > 0.0 ? reader->sampleRate : 44100.0;

        const long long capped = reader->lengthInSamples < maxSamplesToLoad ? reader->lengthInSamples : maxSamplesToLoad;
        const int length = static_cast<int>(capped);
        const int channels = static_cast<int>(reader->numChannels < 2 ? reader->numChannels : 2);
        juce::AudioBuffer<float> loaded(channels, length);
        if (!reader->read(&loaded, 0, length, 0, true, true))
        {
            error = "Could not decode the file (it may be corrupted).";
            return out;
        }

        if (loaded.getNumChannels() == 1)
        {
            juce::AudioBuffer<float> stereo(2, loaded.getNumSamples());
            stereo.copyFrom(0, 0, loaded, 0, 0, loaded.getNumSamples());
            stereo.copyFrom(1, 0, loaded, 0, 0, loaded.getNumSamples());
            out.buffer = std::make_unique<juce::AudioBuffer<float>>(std::move(stereo));
        }
        else
        {
            out.buffer = std::make_unique<juce::AudioBuffer<float>>(std::move(loaded));
        }
        return out;
    }
}

bool AURAAudioProcessor::loadSampleSync(const juce::File& file, juce::String& error)
{
    if (!file.existsAsFile())
    {
        error = "File not found.";
        return false;
    }

    auto decoded = decodeFile(formats, file, error);
    if (decoded.buffer == nullptr)
        return false;

    commitSample(std::move(decoded.buffer), file.getFileName(), file.getFullPathName(), decoded.sampleRate);
    return true;
}

void AURAAudioProcessor::loadSampleAsync(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        sampleName = file.getFileName() + "  (not found)";
        if (onSampleChanged) onSampleChanged();
        return;
    }
    loader->enqueue(file);
}

void AURAAudioProcessor::commitSample(std::unique_ptr<juce::AudioBuffer<float>> buffer,
                                      const juce::String& name, const juce::String& path, double sampleRate)
{
    if (buffer == nullptr)
        return;

    // retire the old buffer (a small ring keeps it alive for any in-flight voice)
    if (sample != nullptr)
    {
        retiredBuffers[static_cast<size_t>(retireSlot)] = std::move(sample);
        retireSlot = (retireSlot + 1) & 3;
    }

    sample = std::move(buffer);
    sampleRateStored = sampleRate;
    sampleName = name;
    samplePath = path;

    // waveform peaks for the UI
    waveformPeaks.fill(0.0f);
    const int n = sample->getNumSamples();
    const int chs = sample->getNumChannels();
    for (int b = 0; b < 128; ++b)
    {
        const int from = n * b / 128;
        const int to = juce::jmin(n, n * (b + 1) / 128 + 1);
        float peak = 0.0f;
        for (int c = 0; c < chs; ++c)
        {
            const float* data = sample->getReadPointer(c);
            for (int i = from; i < to; ++i)
                peak = juce::jmax(peak, std::abs(data[i]));
        }
        waveformPeaks[static_cast<size_t>(b)] = peak;
    }

    activeSample.store(sample.get(), std::memory_order_release);
    if (onSampleChanged)
        onSampleChanged();
}

void AURAAudioProcessor::clearSample()
{
    for (int n = 0; n < 128; ++n)
        synth.noteOff(1, n, 0.0f, true);

    if (sample != nullptr)
    {
        retiredBuffers[static_cast<size_t>(retireSlot)] = std::move(sample);
        retireSlot = (retireSlot + 1) & 3;
    }
    sampleName = "NO SAMPLE";
    samplePath.clear();
    sampleRateStored = 44100.0;
    waveformPeaks.fill(0.0f);
    activeSample.store(nullptr);
    if (onSampleChanged)
        onSampleChanged();
}

float AURAAudioProcessor::velocityGainFor(float velocity) const
{
    const float velDepth = static_cast<float>(parameters.getRawParameterValue("velDepth")->load());
    const float exponent = 1.0f + (1.0f - velDepth) * 1.5f;
    return std::pow(juce::jlimit(0.0f, 1.0f, velocity), exponent);
}

//==============================================================================
// SampleLoader thread
//==============================================================================
void SampleLoader::enqueue(const juce::File& file)
{
    std::lock_guard<std::mutex> lock(mutex);
    job = Job { file, ++sequenceCounter };
    cv.notify_all();
}

void SampleLoader::run()
{
    for (;;)
    {
        Job j;
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return job.has_value() || shuttingDown || threadShouldExit(); });
            if (shuttingDown || threadShouldExit())
                return;
            j = *job;
            job.reset();
        }

        juce::String error;
        auto decoded = decodeFile(proc.formats, j.file, error);

        // Stage the decode result (mutex-guarded) and hop to the message
        // thread to commit it: the buffer swap must not race with the audio
        // thread reading the sample pointer, and the async callback has to
        // be copyable, so it cannot capture the unique_ptr itself.
        const int seq = j.sequence;
        {
            std::lock_guard<std::mutex> lock(mutex);
            pendingBuffer = std::move(decoded.buffer);
            pendingRate = decoded.sampleRate;
            pendingName = j.file.getFileName();
            pendingPath = j.file.getFullPathName();
            pendingError = error;
            pendingSeq = seq;
        }
        const std::weak_ptr<std::atomic<bool>> alive = proc.loaderAlive;
        juce::MessageManager::callAsync(
            [this, seq, alive]
            {
                if (alive.lock() == nullptr)
                    return; // processor destroyed while the decode was running
                if (seq != sequenceCounter)
                    return; // superseded by a newer load request

                std::unique_ptr<juce::AudioBuffer<float>> buffer;
                double rate = 44100.0;
                juce::String name, path, error;
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (pendingSeq != seq)
                        return;
                    buffer = std::move(pendingBuffer);
                    rate = pendingRate;
                    name = pendingName;
                    path = pendingPath;
                    error = pendingError;
                    pendingBuffer = nullptr;
                }

                if (buffer != nullptr)
                    proc.commitSample(std::move(buffer), name, path, rate);
                else
                {
                    // keep the previous sample; surface the error via the name
                    proc.sampleName = name + "  (" + error + ")";
                    if (proc.onSampleChanged)
                        proc.onSampleChanged();
                }
            });
    }
}

//==============================================================================
// State
//==============================================================================
void AURAAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    if (!samplePath.isEmpty())
    {
        state.setProperty("samplePath", samplePath, nullptr);
        state.setProperty("sampleName", sampleName, nullptr);
        state.setProperty("sampleRate", sampleRateStored, nullptr);
    }
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AURAAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr || !xml->hasTagName(parameters.state.getType()))
        return;

    auto state = juce::ValueTree::fromXml(*xml);
    const auto path = state.getProperty("samplePath").toString();
    const auto name = state.getProperty("sampleName").toString();
    parameters.replaceState(state);

    if (path.isNotEmpty())
    {
        juce::File file(path);
        if (file.existsAsFile())
        {
            juce::String error;
            if (loadSampleSync(file, error))
                return;
        }
        // Missing file: keep the settings, report a soft error via the name
        sampleName = name + "  (missing)";
        samplePath = path;
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AURAAudioProcessor();
}
