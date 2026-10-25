#pragma once
#include <JuceHeader.h>

class AuraVoice final : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    void setSample(const juce::AudioBuffer<float>* source, double sourceRate);
    void setPlanetTone(float brightness, float body, float space);
private:
    const juce::AudioBuffer<float>* sampleBuffer = nullptr;
    double sampleRate = 44100.0;
    double position = 0.0;
    double increment = 1.0;
    float level = 0.0f, velocityGain = 0.0f;
    float brightness = 0.65f, body = 0.5f, space = 0.25f;
    float lastL = 0.0f, lastR = 0.0f;
    bool playing = false;
};

class AuraSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class AURAAudioProcessor final : public juce::AudioProcessor
{
public:
    AURAAudioProcessor();
    ~AURAAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "AURA"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    bool loadSample(const juce::File& file);
    bool hasSample() const { return sample.getNumSamples() > 0; }
    juce::String getSampleName() const { return sampleName; }
    juce::String getSamplePath() const { return samplePath; }
    const juce::AudioBuffer<float>& getSampleBuffer() const { return sample; }
    double getSampleRateStored() const { return sampleRateStored; }
    juce::AudioProcessorValueTreeState parameters;

private:
    juce::AudioFormatManager formats;
    juce::AudioBuffer<float> sample;
    juce::String sampleName = "DROP A ONE-SHOT";
    juce::String samplePath;
    double sampleRateStored = 44100.0;
    juce::Synthesiser synth;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void refreshVoices();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AURAAudioProcessor)
};
