#pragma once
#include <JuceHeader.h>
#include "PlanetDSP.h"

#include <array>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>

//==============================================================================
class AuraSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

//==============================================================================
// One voice of the AURA one-shot engine: resampling playback of the loaded
// sample with ADSR envelope, velocity response and root-note mapping.
class AURAAudioProcessor;

class AuraVoice final : public juce::SynthesiserVoice
{
public:
    AuraVoice() = default;

    AURAAudioProcessor* processor = nullptr; // set by the processor at voice creation

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<AuraSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>&, int startSample, int numSamples) override;

private:
    enum class State { attack, decay, sustain, release, done };

    const juce::AudioBuffer<float>* sampleBuf = nullptr;
    double storedRate = 44100.0;
    double position = 0.0;
    double increment = 1.0;

    State state = State::done;
    int currentMidiNote = -1;
    float env = 0.0f;
    float velocityGain = 0.0f;
    float sustainLevel = 0.0f;
    float attackCoef = 1.0f, decayCoef = 1.0f, releaseCoef = 1.0f;
};

//==============================================================================
// Background thread that decodes audio files off the message + audio threads.
class SampleLoader final : public juce::Thread
{
public:
    explicit SampleLoader(AURAAudioProcessor& p) : juce::Thread("AURA loader"), proc(p) {}

    ~SampleLoader() override
    {
        // Wake the worker from its std::condition_variable wait (JUCE's own
        // exit signal doesn't reach it) before asking the thread to stop.
        {
            std::lock_guard<std::mutex> lock (mutex);
            shuttingDown = true;
        }
        cv.notify_all();
        stopThread(2000);
    }

    void enqueue(const juce::File& file);

public:
    struct Job
    {
        juce::File file;
        int sequence = 0;
    };

private:

    void run() override;
    AURAAudioProcessor& proc;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> shuttingDown = false;
    std::optional<Job> job;
    int sequenceCounter = 0;

    // Result of the most recent decode, guarded by `mutex`, so the audio
    // buffer never has to be captured into a (copyable) std::function.
    std::unique_ptr<juce::AudioBuffer<float>> pendingBuffer;
    double pendingRate = 44100.0;
    juce::String pendingName, pendingPath, pendingError;
    int pendingSeq = 0;
    JUCE_DECLARE_NON_COPYABLE(SampleLoader)
};

//==============================================================================
class AURAAudioProcessor final : public juce::AudioProcessor
{
public:
    AURAAudioProcessor();
    ~AURAAudioProcessor() override;

    // AudioProcessor ---------------------------------------------------------
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
    double getTailLengthSeconds() const override { return 8.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    // Sample management -------------------------------------------------------
    // Synchronous load (used by state restoration and the test harness).
    bool loadSampleSync(const juce::File& file, juce::String& error);
    // Asynchronous load; completes on the message thread via onSampleChanged.
    void loadSampleAsync(const juce::File& file);

    void commitSample(std::unique_ptr<juce::AudioBuffer<float>> buffer,
                      const juce::String& name, const juce::String& path, double sampleRate);
    void clearSample();

    bool hasSample() const { return sample != nullptr && sample->getNumSamples() > 0; }
    juce::String getSampleName() const { return sampleName; }
    juce::String getSamplePath() const { return samplePath; }
    double getSampleDurationSeconds() const
    {
        if (sample == nullptr || sampleRateStored <= 0.0) return 0.0;
        return sample->getNumSamples() / sampleRateStored;
    }
    const std::array<float, 128>& getWaveformPeaks() const { return waveformPeaks; }
    const juce::AudioBuffer<float>& getSampleBuffer() const { return *sample; }

    void setSampleChangedCallback(std::function<void()> cb) { onSampleChanged = std::move(cb); }

    juce::AudioProcessorValueTreeState parameters;
    aura::PlanetChain chain;
    juce::Synthesiser synth;
    std::atomic<float> inputActivity { 0.0f }; // last MIDI note-on velocity
    // Active-voice note mask (0-63 / 64-127), published for the keyboard UI.
    std::array<std::atomic<uint64_t>, 2> activeNotes {};

    // Test/introspection helpers
    float velocityGainFor(float velocity) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AURAAudioProcessor)

private:
    friend class SampleLoader;
    friend class AuraVoice;
    void finishDecode(const SampleLoader::Job& job, std::unique_ptr<juce::AudioBuffer<float>> buffer,
                      double rate, const juce::String& error, const juce::String& name);

    juce::AudioFormatManager formats;
    std::unique_ptr<juce::AudioBuffer<float>> sample; // ownership (message thread)
    std::array<std::unique_ptr<juce::AudioBuffer<float>>, 4> retiredBuffers;
    int retireSlot = 0;
    std::atomic<const juce::AudioBuffer<float>*> activeSample { nullptr };

    juce::String sampleName = "NO SAMPLE";
    juce::String samplePath;
    double sampleRateStored = 44100.0;
    std::array<float, 128> waveformPeaks { 0.0f };
    std::function<void()> onSampleChanged;
    std::unique_ptr<SampleLoader> loader;
    // Liveness guard: decode callbacks posted after destruction must no-op.
    std::shared_ptr<std::atomic<bool>> loaderAlive;
    static constexpr int maxVoices = 16;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
