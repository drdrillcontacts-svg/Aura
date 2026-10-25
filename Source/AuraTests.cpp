// AURA headless self-test: exercises the audio engine without a DAW.
//
//   cmake --build build --target AURA_SelfTest
//   ./build/AURA_SelfTest_artefacts/Release/AURA_SelfTest
//
// Exit code 0 = all checks passed.
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static int g_checks = 0;
static int g_failures = 0;

#define CHECK(cond, name)                                                       \
    do {                                                                        \
        ++g_checks;                                                             \
        if (cond) { std::cout << "PASS  " << name << "\n"; }                    \
        else { ++g_failures; std::cout << "FAIL  " << name << "\n"; }           \
    } while (0)

static void crashHandler(int)
{
    void* bt[64];
    const int n = backtrace(bt, 64);
    const char msg[48] = "\n=== CRASH backtrace ===\n";
    (void)!write(2, msg, sizeof(msg) - 1);
    backtrace_symbols_fd(bt, n, 2);
    _exit(42);
}

// The GUI editor (EditorFactory.cpp) is not part of this console target, so
// provide a stub for the vtable entry.
juce::AudioProcessorEditor* AURAAudioProcessor::createEditor() { return nullptr; }

//==============================================================================
static bool writeTestWav(const juce::File& file, double sr, double seconds, bool stereo)
{
    const int n = (int)(sr * seconds);
    juce::AudioBuffer<float> buf(stereo ? 2 : 1, n);
    const int d = (int)(0.005 * sr);
    for (int i = 0; i < n; ++i)
    {
        const double t = i / sr;
        const double env = std::exp(-t * 2.2);
        const float l = (float)((0.5 * std::sin(2.0 * M_PI * 440.0 * t)
                              + 0.3 * std::sin(2.0 * M_PI * 880.0 * t)
                              + 0.2 * std::sin(2.0 * M_PI * 1320.0 * t)) * env * 0.8);
        buf.setSample(0, i, l);
        if (stereo)
        {
            float r = 0.0f;
            if (i >= d)
            {
                const double t2 = (i - d) / sr;
                r = (float)((0.4 * std::sin(2.0 * M_PI * 440.0 * t2)
                           + 0.35 * std::sin(2.0 * M_PI * 1320.0 * t2)) * env * 0.8);
            }
            buf.setSample(1, i, r);
        }
    }
    juce::WavAudioFormat wav;
    file.deleteFile();
    // The writer takes ownership of the stream, so it must be heap-allocated.
    juce::FileOutputStream* stream = new juce::FileOutputStream(file);
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wav.createWriterFor(stream, sr, stereo ? 2 : 1, 16, {}, 0));
    if (writer == nullptr)
    {
        delete stream;
        return false;
    }
    return writer->writeFromAudioSampleBuffer(buf, 0, n);
}

//==============================================================================
// Render helper: `events` = (blockIndex, midiNote, velocityOr0ForNoteOff).
// Each block gets a fresh, cleared MidiBuffer, exactly like a host would.
static juce::AudioBuffer<float> renderEvents(AURAAudioProcessor& p,
                                             const std::vector<std::tuple<int, int, int>>& events,
                                             int totalBlocks, int blockSize = 64,
                                             double sampleRate = 44100.0)
{
    p.prepareToPlay(sampleRate, blockSize);
    juce::AudioBuffer<float> out(2, totalBlocks * blockSize);
    out.clear();
    juce::MidiBuffer midi;
    int ev = 0;
    for (int b = 0; b < totalBlocks; ++b)
    {
        midi.clear(); // the host clears the buffer each block; Synthesiser does not
        while (ev < (int)events.size() && std::get<0>(events[static_cast<size_t>(ev)]) == b)
        {
            const int note = std::get<1>(events[static_cast<size_t>(ev)]);
            const int vel = std::get<2>(events[static_cast<size_t>(ev)]);
            uint8 data[3] = { (uint8)(0x90 | 1), (uint8)note, (uint8)vel };
            midi.addEvent(juce::MidiMessage(data, 3), 0); // time relative to this block
            ++ev;
        }
        juce::AudioBuffer<float> block(2, blockSize);
        p.processBlock(block, midi);
        for (int ch = 0; ch < out.getNumChannels(); ++ch)
            out.addFrom(ch, b * blockSize, block, ch, 0, blockSize);
    }
    return out;
}

static float bufferEnergy(const juce::AudioBuffer<float>& b)
{
    float e = 0.0f;
    for (int ch = 0; ch < b.getNumChannels(); ++ch)
        for (int i = 0; i < b.getNumSamples(); ++i)
            e += std::abs(b.getSample(ch, i));
    return e;
}

static float rangeEnergy(const juce::AudioBuffer<float>& b, int ch, int start, int n)
{
    float e = 0.0f;
    const int end = juce::jmin(start + n, b.getNumSamples());
    for (int i = start; i < end; ++i)
        e += std::abs(b.getSample(ch, i));
    return e;
}

static float bufferL1Diff(const juce::AudioBuffer<float>& a, const juce::AudioBuffer<float>& b)
{
    const int n = juce::jmin(a.getNumSamples(), b.getNumSamples());
    float d = 0.0f, m = 0.0f;
    for (int ch = 0; ch < juce::jmin(a.getNumChannels(), b.getNumChannels()); ++ch)
        for (int i = 0; i < n; ++i)
        {
            d += std::abs(a.getSample(ch, i) - b.getSample(ch, i));
            m += std::abs(a.getSample(ch, i));
        }
    return m > 1e-9f ? d / m : d;
}

// Dominant frequency via FFT (first 4096 samples of the channel mix).
static double dominantFrequency(const juce::AudioBuffer<float>& b, double sampleRate)
{
    const int N = 4096;
    juce::AudioBuffer<float> mono(1, N);
    mono.clear();
    const int n = juce::jmin(N, b.getNumSamples());
    for (int i = 0; i < n; ++i)
    {
        const float mix = 0.5f * (b.getSample(0, i)
                          + (b.getNumChannels() > 1 ? b.getSample(1, i) : b.getSample(0, i)));
        mono.setSample(0, i, mix * (0.5f * (1.0f - std::cos(2.0f * M_PI * i / N))));
    }
    juce::dsp::FFT fft(12); // 2^12 = 4096 points
    // The transform writes a complex result, so the array must be 2 * N floats.
    std::vector<float> data(2 * N, 0.0f);
    for (int i = 0; i < N; ++i)
        data[i] = mono.getSample(0, i);
    fft.performRealOnlyForwardTransform(data.data());

    int best = 0;
    float bestMag = 0.0f;
    for (int i = 3; i < N / 2; ++i)
    {
        const float f = i * sampleRate / N;
        if (f < 50.0 || f > 9000.0) continue;
        const float re = data[(size_t)2 * i], im = data[(size_t)2 * i + 1];
        const float m = std::sqrt(re * re + im * im);
        if (m > bestMag) { bestMag = m; best = i; }
    }
    return best * sampleRate / N;
}

// Set a parameter from its RAW value (works for float, int and choice params;
// the host-facing float API is normalized 0..1, so convert via the range).
static void setParam(AURAAudioProcessor& p, const char* id, float rawValue)
{
    auto* par = p.parameters.getParameter(id);
    const auto norm = par->getNormalisableRange().convertTo0to1(rawValue);
    par->beginChangeGesture();
    par->setValueNotifyingHost(norm);
    par->endChangeGesture();
}

static float getParamRaw(AURAAudioProcessor& p, const char* id)
{
    return p.parameters.getRawParameterValue(id)->load();
}

// A fresh processor with the test sample loaded (each test gets its own, so
// voices from earlier tests can't contaminate later measurements).
static std::unique_ptr<AURAAudioProcessor> makeLoaded(const juce::File& wavFile, juce::String& err)
{
    auto p = std::make_unique<AURAAudioProcessor>();
    if (!p->loadSampleSync(wavFile, err))
        return nullptr;
    return p;
}

//==============================================================================
int main()
{
    signal(SIGSEGV, crashHandler);
    signal(SIGABRT, crashHandler);
    signal(SIGFPE, crashHandler);
    signal(SIGBUS, crashHandler);
    std::cout << std::unitbuf;
    std::cout << "AURA self-test\n================\n";

    const juce::File tempDir(juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getChildFile("aura_selftest_" + juce::String(juce::Random().nextInt(100000))));
    tempDir.createDirectory();
    const juce::File wavFile(tempDir.getChildFile("test_oneshot.wav"));
    const juce::File badFile(tempDir.getChildFile("corrupt.wav"));
    {
        juce::FileOutputStream stream(badFile);
        stream.write("this is not audio data", 22);
    }

    // ------------------------------------------------------------------ load
    AURAAudioProcessor proc;
    juce::String err;
    CHECK(writeTestWav(wavFile, 44100.0, 2.0, true), "test sample generated");
    CHECK(proc.loadSampleSync(wavFile, err), "sync load of WAV succeeds");
    CHECK(proc.hasSample(), "hasSample() true after load");
    CHECK(std::abs(proc.getSampleDurationSeconds() - 2.0) < 0.05, "sample duration ~2s");
    bool anyPeak = false;
    for (float v : proc.getWaveformPeaks()) anyPeak = anyPeak || v > 0.01f;
    CHECK(anyPeak, "waveform peaks computed");
    CHECK(!proc.loadSampleSync(badFile, err) && err.isNotEmpty(), "corrupt/unsupported file rejected with error");
    CHECK(!proc.loadSampleSync(juce::File("/nonexistent/path/sample.wav"), err), "missing file rejected");

    // ------------------------------------------------------------- playback
    {
        auto p = makeLoaded(wavFile, err);
        auto r = renderEvents(*p, { { 2, 60, 100 } }, 300);
        CHECK(bufferEnergy(r) > 0.01f, "MIDI note-on produces audio");
    }
    {
        auto p = makeLoaded(wavFile, err);
        auto r = renderEvents(*p, { { 2, 60, 100 }, { 20, 60, 0 } }, 300);
        // Equal-length windows: release tail right after note-off vs much later.
        const float e1 = rangeEnergy(r, 0, 20 * 64, 40 * 64);
        const float e2 = rangeEnergy(r, 0, 260 * 64, 40 * 64);
        CHECK(e1 > e2 * 1.5f && e1 > 0.001f, "note-off triggers release decay");
    }

    // chromatic pitch: C4 vs C5
    {
        auto p = makeLoaded(wavFile, err);
        auto a = renderEvents(*p, { { 4, 60, 100 } }, 160);
        auto b = renderEvents(*p, { { 4, 72, 100 } }, 160);
        const double fa = dominantFrequency(a, 44100.0);
        const double fb = dominantFrequency(b, 44100.0);
        std::cout << "       freq note60=" << fa << "Hz  note72=" << fb << "Hz\n";
        CHECK(fa > 380.0 && fa < 500.0, "note 60 ~ 440Hz (root)");
        CHECK(std::abs(fb / fa - 2.0) < 0.06, "note 72 ~ 2x pitch (chromatic mapping)");
    }


    // root note mapping
    {
        auto p = makeLoaded(wavFile, err);
        setParam(*p, "rootNote", 48.0f);
        auto a = renderEvents(*p, { { 4, 48, 100 } }, 160);
        const double f = dominantFrequency(a, 44100.0);
        std::cout << "       root=48: note48 dominant=" << f << "Hz\n";
        CHECK(f > 380.0 && f < 500.0, "root note changes pitch mapping");
    }

    // velocity
    {
        auto p = makeLoaded(wavFile, err);
        auto lo = renderEvents(*p, { { 4, 60, 20 } }, 200);
        auto hi = renderEvents(*p, { { 4, 60, 110 } }, 200);
        CHECK(bufferEnergy(hi) > bufferEnergy(lo) * 2.5f, "velocity affects output level");
    }

    // polyphony + voice stealing
    {
        auto p = makeLoaded(wavFile, err);
        std::vector<std::tuple<int, int, int>> ev;
        const int notes[8] = { 48, 55, 60, 62, 64, 67, 71, 76 };
        for (int n : notes) ev.push_back({ 4, n, 90 });
        auto poly = renderEvents(*p, ev, 200);
        CHECK(bufferEnergy(poly) > 0.05f, "polyphonic playback (8 notes)");

        std::vector<std::tuple<int, int, int>> overflow;
        for (int n = 36; n < 36 + 20; ++n) overflow.push_back({ 4, n, 90 });
        auto over = renderEvents(*p, overflow, 200); // 20 > 16 voices
        CHECK(bufferEnergy(over) > 0.05f, "voice stealing beyond voice limit (no crash)");
    }

    // --------------------------------------------------- planet profiles
    {
        auto pMercury = makeLoaded(wavFile, err);
        auto pJupiter = makeLoaded(wavFile, err);
        setParam(*pMercury, "planet", 0.0f);  // Mercury (choice index 0)
        setParam(*pJupiter, "planet", 4.0f);  // Jupiter (choice index 4)
        CHECK(std::abs(getParamRaw(*pMercury, "planet") - 0.0f) < 0.01f
          && std::abs(getParamRaw(*pJupiter, "planet") - 4.0f) < 0.01f, "planet choice params set");
        const auto ev = std::vector<std::tuple<int, int, int>> { { 4, 60, 100 } };
        const auto a = renderEvents(*pMercury, ev, 400);
        const auto b = renderEvents(*pJupiter, ev, 400);
        const float diff = bufferL1Diff(a, b);
        std::cout << "       mercury vs jupiter rel-diff = " << diff << "\n";
        CHECK(diff > 0.005f, "planet selection changes the processed sound");
    }

    // ------------------------------------------------- control sweep
    {
        const auto ev = std::vector<std::tuple<int, int, int>> { { 4, 60, 100 }, { 60, 60, 0 } };

        struct P { const char* id; float lo; float hi; const char* label; };
        const P params[] = {
            { "gravity", 0.0f, 1.0f, "Gravity" },
            { "atmosphere", 0.0f, 1.0f, "Atmosphere" },
            { "temperature", 0.0f, 1.0f, "Temperature" },
            { "density", 0.0f, 1.0f, "Density" },
            { "brightness", 0.0f, 1.0f, "Brightness" },
            { "harmonics", 0.0f, 1.0f, "Harmonics" },
            { "reverb", 0.0f, 1.0f, "Reverb" },
            { "width", 0.0f, 1.0f, "Width" },
            { "orbSize", 0.0f, 1.0f, "Orbital Size" },
            { "orbDecay", 0.2f, 6.0f, "Orbital Decay" },
        };
        for (const auto& prm : params)
        {
            auto p = makeLoaded(wavFile, err);
            CHECK(p != nullptr, (juce::String(prm.label) + " sample load").toRawUTF8());
            if (p == nullptr) continue;
            auto def = renderEvents(*p, ev, 400);
            setParam(*p, prm.id, prm.hi);
            auto hi = renderEvents(*p, ev, 400);
            setParam(*p, prm.id, prm.lo);
            auto lo = renderEvents(*p, ev, 400);
            const float dHi = bufferL1Diff(def, hi);
            const float dLo = bufferL1Diff(def, lo);
            const float d = juce::jmax(dHi, dLo);
            std::cout << "       " << prm.label << " rel-diff = " << d << "\n";
            CHECK(d > 1e-4f, (juce::String("control affects output: ") + prm.label).toRawUTF8());
        }

        // mode selector
        {
            auto p = makeLoaded(wavFile, err);
            auto def = renderEvents(*p, ev, 400);
            setParam(*p, "mode", 2.0f); // Alien (choice index 2)
            auto alien = renderEvents(*p, ev, 400);
            const float d = bufferL1Diff(def, alien);
            std::cout << "       mode (Alien) rel-diff = " << d << "\n";
            CHECK(d > 1e-4f, "mode selector affects output");
        }
    }

    // ------------------------------------------------------- ADSR
    {
        auto p = makeLoaded(wavFile, err);
        setParam(*p, "attack", 0.5f);
        auto slow = renderEvents(*p, { { 2, 60, 100 } }, 120);
        setParam(*p, "attack", 0.002f);
        auto fast = renderEvents(*p, { { 2, 60, 100 } }, 120);
        const float slowE = rangeEnergy(slow, 0, 2 * 64, 10 * 64);
        const float fastE = rangeEnergy(fast, 0, 2 * 64, 10 * 64);
        CHECK(fastE > slowE * 1.5f, "attack parameter shapes onset");

        setParam(*p, "sustain", 1.0f);
        setParam(*p, "decay", 2.0f);
        auto sus = renderEvents(*p, { { 2, 60, 100 } }, 300);
        setParam(*p, "sustain", 0.0f);
        setParam(*p, "decay", 0.1f);
        auto dec = renderEvents(*p, { { 2, 60, 100 } }, 300);
        const float susMid = rangeEnergy(sus, 0, 100 * 64, 80 * 64);
        const float decMid = rangeEnergy(dec, 0, 100 * 64, 80 * 64);
        CHECK(susMid > decMid * 1.5f, "sustain/decay parameters shape the body");
    }

    // --------------------------------------------------------- state
    {
        auto p1 = makeLoaded(wavFile, err);
        setParam(*p1, "planet", 3.0f);   // Mars (choice index 3)
        setParam(*p1, "brightness", 0.3f);
        setParam(*p1, "rootNote", 55.0f);

        juce::MemoryBlock stateBlock;
        p1->getStateInformation(stateBlock);

        AURAAudioProcessor p2;
        p2.setStateInformation(stateBlock.getData(), (int)stateBlock.getSize());
        CHECK(p2.hasSample(), "state restore reloads sample");
        CHECK(p2.getSampleName() == p1->getSampleName(), "sample name restored");
        CHECK(std::abs(getParamRaw(p2, "planet") - 3.0f) < 0.01f, "planet restored");
        CHECK(std::abs(getParamRaw(p2, "brightness") - 0.3f) < 0.01f, "brightness restored");
        CHECK(std::abs(getParamRaw(p2, "rootNote") - 55.0f) < 0.01f, "root note restored");

        // missing file recovery
        AURAAudioProcessor p3;
        {
            auto xml = p1->parameters.copyState();
            xml.setProperty("samplePath", "/no/such/file.wav", nullptr);
            xml.setProperty("sampleName", "gone.wav", nullptr);
            std::unique_ptr<juce::XmlElement> e(xml.createXml());
            juce::MemoryBlock mb;
            juce::AudioProcessor::copyXmlToBinary(*e, mb);
            p3.setStateInformation(mb.getData(), (int)mb.getSize());
        }
        CHECK(!p3.hasSample(), "missing sample file -> no sample, no crash");
        CHECK(p3.getSampleName().contains("missing"), "missing file reported in name");
    }

    // ------------------------------------------------------ analyser
    {
        auto p = makeLoaded(wavFile, err);
        renderEvents(*p, { { 4, 60, 100 } }, 200);
        const auto frame = p->chain.getAnalyser().readFrame();
        float maxBand = 0.0f;
        for (float v : frame.band) maxBand = juce::jmax(maxBand, v);
        CHECK(maxBand > -40.0f, "spectral analyser sees the signal");
        CHECK(p->chain.getAnalyser().readPeak() >= 0.0f, "peak meter readable");
    }

    // cleanup
    tempDir.deleteRecursively();

    std::cout << "================\n";
    std::cout << g_checks - g_failures << "/" << g_checks << " checks passed";
    if (g_failures > 0) std::cout << " — " << g_failures << " FAILED";
    std::cout << "\n";
    return g_failures == 0 ? 0 : 1;
}
