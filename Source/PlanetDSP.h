#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <JuceHeader.h>
#include <cmath>
#include "PlanetProfiles.h"

namespace aura
{

//==============================================================================
// Per-sample one-pole smoothing for click-free parameter changes.
class Smoothed
{
public:
    void prepare(double sampleRate)
    {
        alpha = static_cast<float>(1.0 - std::exp(-1.0 / (sampleRate * 0.018)));
        value = target;
    }
    void setTarget(float t) { target = t; }
    float next() { value += (target - value) * alpha; return value; }
    float get() const { return value; }
    void snapTo(float v) { value = v; target = v; }
private:
    float value = 0.0f, target = 0.0f, alpha = 0.01f;
};

//==============================================================================
// RBJ cookbook biquad. Coefficients are re-designed per block (from smoothed
// parameters); filter state is kept for continuity.
class Biquad
{
public:
    enum class Type { lowpass, highpass, peaking, lowshelf, highshelf };

    void design(Type type, double sampleRate, double freq, double gainDb, double q)
    {
        freq = juce::jlimit(20.0, sampleRate * 0.45, freq);
        q = juce::jmax(0.05, q);
        const double w0 = 2.0 * M_PI * freq / sampleRate;
        const double c = std::cos(w0), s = std::sin(w0);
        const double A = std::pow(10.0, gainDb / 40.0);
        const double alpha = s / (2.0 * q);

        double b0 = 1.0, b1 = 0.0, b2 = 0.0, a0 = 1.0, a1 = 0.0, a2 = 0.0;
        switch (type)
        {
            case Type::lowpass:
                b0 = (1.0 - c) * 0.5; b1 = 1.0 - c; b2 = b0;
                a0 = 1.0 + alpha; a1 = -2.0 * c; a2 = 1.0 - alpha;
                break;
            case Type::highpass:
                b0 = (1.0 + c) * 0.5; b1 = -(1.0 + c); b2 = b0;
                a0 = 1.0 + alpha; a1 = -2.0 * c; a2 = 1.0 - alpha;
                break;
            case Type::peaking:
                b0 = 1.0 + alpha * A; b1 = -2.0 * c; b2 = 1.0 - alpha * A;
                a0 = 1.0 + alpha / A; a1 = -2.0 * c; a2 = 1.0 - alpha / A;
                break;
            case Type::lowshelf:
            {
                const double sa = s * std::sqrt(A) / 2.0;
                b0 = A * (A + 1.0 - (A - 1.0) * c + 2.0 * sa);
                b1 = 2.0 * A * (A - 1.0 - (A + 1.0) * c);
                b2 = A * (A + 1.0 - (A - 1.0) * c - 2.0 * sa);
                a0 = A + 1.0 + (A - 1.0) * c + 2.0 * sa;
                a1 = 2.0 * (-(A + 1.0) + (A - 1.0) * c);
                a2 = A + 1.0 + (A - 1.0) * c - 2.0 * sa;
                break;
            }
            case Type::highshelf:
            {
                const double sa = s * std::sqrt(A) / 2.0;
                b0 = A * (A + 1.0 + (A - 1.0) * c + 2.0 * sa);
                b1 = -2.0 * A * (A - 1.0 + (A + 1.0) * c);
                b2 = A * (A + 1.0 + (A - 1.0) * c - 2.0 * sa);
                a0 = A + 1.0 - (A - 1.0) * c + 2.0 * sa;
                a1 = 2.0 * ((A + 1.0) - (A - 1.0) * c);
                a2 = A + 1.0 - (A - 1.0) * c - 2.0 * sa;
                break;
            }
        }

        b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;
    }

    float process(float x)
    {
        const float y = b0 * x + b1 * xm1 + b2 * xm2 - a1 * ym1 - a2 * ym2;
        xm2 = xm1; xm1 = x; ym2 = ym1; ym1 = y;
        return y;
    }

    void reset() { xm1 = xm2 = ym1 = ym2 = 0.0f; }

private:
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
    float xm1 = 0.0f, xm2 = 0.0f, ym1 = 0.0f, ym2 = 0.0f;
};

//==============================================================================
// Interpolating delay line. All memory is allocated in prepare().
class DelayLine
{
public:
    void prepare(int maxDelaySamples)
    {
        buf.assign(static_cast<size_t>(maxDelaySamples) + 8, 0.0f);
        w = 0;
    }

    void reset() { std::fill(buf.begin(), buf.end(), 0.0f); w = 0; }

    float readAt(float delay) const
    {
        if (buf.empty()) return 0.0f;
        const float d = juce::jlimit(0.5f, static_cast<float>(buf.size()) - 4.0f, delay);
        const float idx = static_cast<float>(w) - d;
        float wrapped = static_cast<float>(std::fmod(idx, static_cast<double>(buf.size())));
        if (wrapped < 0.0f)
            wrapped += static_cast<float>(buf.size());
        const int i0 = static_cast<int>(wrapped) % static_cast<int>(buf.size());
        const int i1 = (i0 + 1) % static_cast<int>(buf.size());
        const float f = wrapped - static_cast<float>(i0);
        return buf[static_cast<size_t>(i0)] + (buf[static_cast<size_t>(i1)] - buf[static_cast<size_t>(i0)]) * f;
    }

    void write(float x)
    {
        if (buf.empty()) return;
        buf[static_cast<size_t>(w)] = x;
        w = (w + 1) % static_cast<int>(buf.size());
    }

private:
    std::vector<float> buf;
    int w = 0;
};

//==============================================================================
// Schroeder-style reverb channel (4 combs + 2 allpasses). Fixed-size state,
// no allocation in process().
class SchroederChannel
{
public:
    static constexpr int numCombs = 4;
    static constexpr int numAllpass = 2;
    static constexpr float combBaseMs[numCombs] = { 29.0f, 37.0f, 53.0f, 71.0f };
    static constexpr float apBaseMs[numAllpass] = { 25.0f, 41.0f };

    void prepare(double sampleRate)
    {
        for (auto& c : combs) c.prepare(static_cast<int>(71.0e-3 * 1.35 * sampleRate) + 16);
        for (auto& a : allpass) a.prepare(static_cast<int>(41.0e-3 * 0.85 * sampleRate) + 16);
        for (auto& m : fbMem) m = 0.0f;
    }

    void reset()
    {
        for (auto& c : combs) c.reset();
        for (auto& a : allpass) a.reset();
        for (auto& m : fbMem) m = 0.0f;
    }

    // Returns the wet signal for a dry input.
    float processWet(float in, float size01, double rt60Seconds, double sampleRate)
    {
        const float sizeScale = 0.15f + 1.1f * size01;

        float y = in;
        for (int c = 0; c < numCombs; ++c)
        {
            const float delay = combBaseMs[c] * sizeScale * scale * 1e-3f * static_cast<float>(sampleRate);
            const double rt60 = juce::jmax(0.05, rt60Seconds);
            float fb = std::pow(0.5, delay / (rt60 * sampleRate));
            fb = juce::jlimit(0.20f, 0.86f, fb);

            const float delayed = combs[c].readAt(delay);
            fbMem[c] = fb * (0.55f * delayed + 0.45f * fbMem[c]); // low-passed feedback
            y = delayed + fbMem[c];
            combs[c].write(y);
        }

        for (int a = 0; a < numAllpass; ++a)
        {
            const float delay = apBaseMs[a] * (0.15f + 0.6f * size01) * scale * 1e-3f
                                * static_cast<float>(sampleRate);
            const float d = allpass[a].readAt(delay);
            allpass[a].write(y);
            y = d + 0.5f * (y - d);
        }
        return y;
    }

    void setScale(float s) { scale = s; }

private:
    DelayLine combs[numCombs];
    DelayLine allpass[numAllpass];
    float fbMem[numCombs] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float scale = 1.0f;
};

class OrbitalReverb
{
public:
    void prepare(double sampleRate)
    {
        left.prepare(sampleRate);
        right.prepare(sampleRate);
        right.setScale(0.963f);
    }

    void reset() { left.reset(); right.reset(); }

    void process(float& inL, float& inR, float size01, double rt60, float wet01, double sampleRate)
    {
        const float wetL = left.processWet(inL, size01, rt60, sampleRate);
        const float wetR = right.processWet(inR, size01, rt60, sampleRate);
        inL += wetL * wet01;
        inR += wetR * wet01;
    }

private:
    SchroederChannel left, right;
};

//==============================================================================
// Two-voice stereo chorus with per-channel LFOs. Bypasses when depth ~ 0.
class StereoChorus
{
public:
    void prepare(double sampleRate)
    {
        const int maxDelay = static_cast<int>(0.024 * sampleRate) + 8;
        left.prepare(maxDelay);
        right.prepare(maxDelay);
        phaseL = phaseR = 0.0f;
    }

    void reset() { left.reset(); right.reset(); }

    void process(float& inL, float& inR, float depthMs, float rateHz, double sampleRate)
    {
        if (depthMs < 0.05f)
            return;

        const float incL = static_cast<float>(2.0 * M_PI * juce::jmax(0.01f, rateHz) / sampleRate);
        phaseL += incL;
        phaseR += incL + 0.017f;
        if (phaseL > 2.0f * M_PI) phaseL -= 2.0f * M_PI;
        if (phaseR > 2.0f * M_PI) phaseR -= 2.0f * M_PI;

        const float dL = depthMs * (0.5f + 0.5f * std::sin(phaseL)) * 1e-3f * static_cast<float>(sampleRate);
        const float dR = depthMs * (0.5f + 0.5f * std::sin(phaseR)) * 1e-3f * static_cast<float>(sampleRate);

        const float l = left.readAt(dL);
        const float r = right.readAt(dR);
        left.write(inL);
        right.write(inR);
        inL = 0.65f * inL + 0.40f * l;
        inR = 0.65f * inR + 0.40f * r;
    }

private:
    DelayLine left, right;
    float phaseL = 0.0f, phaseR = 0.0f;
};

//==============================================================================
// Spectral analyser: log-spaced FFT spectrum (with per-band smoothing),
// partial peak picking, amplitude envelope ring buffer and peak meter.
//
// Thread safety: audio thread writes slot A then flips an atomic index; the
// UI thread reads the *other* slot via a plain copy. No audio buffers are
// ever shared.
class Analyser
{
public:
    static constexpr int numBands = 80;
    static constexpr int envSize = 1024;
    static constexpr int maxPartials = 20;

    struct Frame
    {
        float band[numBands];
        int partialCount = 0;
        float pf[maxPartials];
        float pa[maxPartials];
    };

    void prepare(double sampleRate)
    {
        sr = sampleRate;
        // The real-only forward transform writes a complex result back into
        // this buffer, so it must hold 2 * fftSize floats (the input reals
        // use the first fftSize of them).
        fftInput.assign(2 * fftSize, 0.0f);
        window.assign(fftSize / 2, 0.0f);
        for (int i = 0; i < fftSize / 2; ++i)
            window[static_cast<size_t>(i)] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / fftSize));

        const double f0 = 30.0, f1 = 20000.0;
        for (int i = 0; i < numBands; ++i)
            bandFreq[static_cast<size_t>(i)] = f0 * std::pow(f1 / f0, i / static_cast<double>(numBands - 1));

        for (auto& f : slots)
        {
            for (int b = 0; b < numBands; ++b) f.band[b] = -100.0f;
            f.partialCount = 0;
        }
        envBuffer.assign(envSize, 0.0f);
        envWriteIndex.store(0);
        rmsAccum = 0.0f;
        frameCount = 0;
    }

    // Audio thread: one call per sample with the processed stereo signal.
    void sample(float inL, float inR)
    {
        const float mag = 0.5f * std::sqrt(inL * inL + inR * inR);
        const float decayed = outPeak.load(std::memory_order_relaxed) * 0.995f;
        outPeak.store(mag > decayed ? mag : decayed, std::memory_order_relaxed);

        rmsAccum += mag * mag;
        fftInput[static_cast<size_t>(frameCount)] = 0.5f * (inL + inR);
        if (++frameCount < hop)
            return;
        frameCount = 0;

        for (int i = 0; i < fftSize / 2; ++i)
            fftInput[static_cast<size_t>(i)] *= window[static_cast<size_t>(i)];
        fft.performFrequencyOnlyForwardTransform(fftInput.data(), true);  // in-place: magnitudes in [0, fftSize/2]

        Frame& f = slots[static_cast<size_t>(writeIndex)];
        for (int b = 0; b < numBands; ++b)
        {
            int iLo = static_cast<int>(bandFreq[static_cast<size_t>(b)] * fftSize / sr);
            int iHi = static_cast<int>((b + 1 < numBands ? bandFreq[static_cast<size_t>(b + 1)] : 20000.0)
                                       * fftSize / sr);
            iLo = juce::jmax(1, iLo);
            iHi = juce::jmin(fftSize / 2 - 1, juce::jmax(iLo + 1, iHi));
            float peak = 0.0f;
            for (int i = iLo; i <= iHi; ++i)
                peak = juce::jmax(peak, fftInput[static_cast<size_t>(i)]);
            const float db = 20.0f * std::log10(peak + 1e-7f);
            const float s = f.band[b] + (db - f.band[b]) * (db > f.band[b] ? 0.45f : 0.12f);
            f.band[b] = s;
        }

        // envelope: RMS of this hop
        envBuffer[static_cast<size_t>(envWriteIndex.load())] =
            std::sqrt(rmsAccum / hop);
        rmsAccum = 0.0f;
        envWriteIndex.store((envWriteIndex.load() + 1) % envSize);

        // partial picking
        f.partialCount = 0;
        for (int b = 1; b < numBands - 1 && f.partialCount < maxPartials; ++b)
        {
            const float v = f.band[b];
            if (v > -52.0f && v >= f.band[b - 1] && v > f.band[b + 1])
            {
                f.pf[f.partialCount] = bandFreq[static_cast<size_t>(b)];
                f.pa[f.partialCount] = juce::jlimit(-100.0f, 0.0f, v);
                ++f.partialCount;
            }
        }

        writeIndex = 1 - writeIndex;
    }

    // UI thread -------------------------------------------------------------
    Frame readFrame() const { return slots[static_cast<size_t>(1 - writeIndex)]; }

    // Most recent `count` envelope values, oldest first.
    void readEnvelope(float* out, int count) const
    {
        const int idx = envWriteIndex.load();
        for (int i = 0; i < count; ++i)
        {
            const int pos = (idx - count + i + 2 * envSize) % envSize;
            out[i] = envBuffer[static_cast<size_t>(pos)];
        }
    }

    float readPeak() const { return outPeak.load(std::memory_order_relaxed); }

private:
    juce::dsp::FFT fft { 12 };  // 2^12 = 4096-point transform
    std::vector<float> fftInput, window;
    std::array<Frame, 2> slots;
    int writeIndex = 0;

    std::array<float, numBands> bandFreq;
    int hop = 1024, frameCount = 0;
    float rmsAccum = 0.0f;
    static constexpr int fftSize = 4096;

    std::vector<float> envBuffer;
    std::atomic<int> envWriteIndex { 0 };
    std::atomic<float> outPeak { 0.0f };
    double sr = 44100.0;
};

//==============================================================================
// Full AURA signal chain (stereo, per-sample):
//
//  voices -> gravity (low shelf) -> temperature (EQ tilt) -> brightness (LP)
//         -> harmonics (symmetric saturation) -> density (parallel thickener)
//         -> planet character (peaking + chorus + air, crossfaded per planet)
//         -> width (mid/side) -> orbital reverb -> level -> soft limiter
//         -> analyser
//
// Real-time safe: prepare() does all allocation; processSample() allocates
// nothing and takes no locks.
class PlanetChain
{
public:
    void prepare(double sampleRate)
    {
        sr = sampleRate;
        for (auto& s : { &smGravity, &smAtmosphere, &smTemperature, &smDensity, &smBrightness,
                         &smHarmonics, &smReverb, &smWidth, &smOrbSize, &smOrbDecay,
                         &smVelDepth, &smMode })
            s->prepare(sampleRate);

        reverb.prepare(sampleRate);
        chorus.prepare(sampleRate);
        const int densDelay = static_cast<int>(0.002 * sampleRate) + 8;
        densityLineL.prepare(densDelay);
        densityLineR.prepare(densDelay);
        analyser.prepare(sampleRate);
    }

    // Converge the one-pole smoothing after targets have been set, so the
    // first audited block is already at steady state.
    void settle()
    {
        for (int i = 0; i < 800; ++i) designBlock();
    }

    void reset()
    {
        for (auto& b : biquads) b.reset();
        reverb.reset();
        chorus.reset();
        densityLineL.reset();
        densityLineR.reset();
        analyser.prepare(sr);
    }

    // Per-block: advance smoothing, re-design coefficients.
    void designBlock()
    {
        const float g = smGravity.next();
        const float at = smAtmosphere.next();
        const float te = smTemperature.next();
        const float de = smDensity.next();
        const float br = smBrightness.next();
        const float ha = smHarmonics.next();
        const float rv = smReverb.next();
        const float wi = smWidth.next();
        const float os = smOrbSize.next();
        const float od = smOrbDecay.next();
        const float mode = smMode.next();

        gravity = g; atmosphere = at; temperature = te; density = de;
        brightness = br; harmonics = ha; reverbAmt = rv; width = wi;
        orbSize = os; orbDecay = od; modeValue = mode;

        biquads[0].design(Biquad::Type::lowshelf,  sr, 180.0,  -10.0 + 18.0 * g, 0.707); // gravity
        biquads[1].design(Biquad::Type::lowshelf,  sr, 350.0,   7.0 - 14.0 * te, 0.707); // temperature low
        biquads[2].design(Biquad::Type::highshelf, sr, 4200.0, -5.0 + 10.0 * te, 0.707); // temperature high
        biquads[3].design(Biquad::Type::lowpass,   sr, 500.0 * std::pow(28.0, br), 0.0, 0.8); // brightness
        biquads[4].design(Biquad::Type::peaking,   sr, curChar.peakHz, curChar.peakDb,
                          juce::jmax(0.4f, curChar.peakQ));
        biquads[5].design(Biquad::Type::highshelf, sr, 7500.0,
                          curChar.airDb + (at - 0.5f) * 8.0f + (mode > 1.4f ? 3.0f : 0.0f), 0.707);

        drive = 1.0f + ha * 3.5f + curChar.driveAdd + (mode > 1.4f ? 1.5f : 0.0f);
        reverbMix = juce::jlimit(0.0f, 1.0f,
            rv + juce::jmax(0.0f, at - 0.5f) * 0.6f + (mode > 0.6f ? 0.15f : 0.0f));
        chorusDepth = curChar.chorusMs * (mode > 1.4f ? 2.2f : (mode > 0.6f ? 1.5f : 1.0f));
        chorusRate  = curChar.chorusHz * (mode > 1.4f ? 2.2f : 1.0f);
    }

    // Set 0..1 parameter targets (orbDecay in seconds). planetIndex/mode switch
    // the character crossfade state machine.
    void setTargets(float g, float at, float te, float de, float br, float ha,
                    float rv, float wi, float os, float odSeconds, float vel,
                    int planetIndex, int mode)
    {
        smGravity.setTarget(g);  smAtmosphere.setTarget(at); smTemperature.setTarget(te);
        smDensity.setTarget(de); smBrightness.setTarget(br); smHarmonics.setTarget(ha);
        smReverb.setTarget(rv);  smWidth.setTarget(wi);
        smOrbSize.setTarget(os); smOrbDecay.setTarget(odSeconds);
        smVelDepth.setTarget(vel); smMode.setTarget(mode);

        planetIndex = juce::jlimit(0, 8, planetIndex);
        if (planetIndex != targetPlanet)
        {
            fromChar = curChar;                 // freeze the current blended state
            targetPlanet = planetIndex;
            crossfade = 0.0f;
        }
    }

    void processSample(float& inL, float& inR)
    {
        // 1. gravity / tonal weight
        inL = biquads[0].process(inL);
        inR = biquads[0].process(inR);
        // 2. temperature tilt
        inL = biquads[2].process(biquads[1].process(inL));
        inR = biquads[2].process(biquads[1].process(inR));
        // 3. brightness
        inL = biquads[3].process(inL);
        inR = biquads[3].process(inR);
        // 4. harmonics
        inL = saturate(inL, drive);
        inR = saturate(inR, drive);
        // 5. density - parallel short feedback delay + mild drive
        {
            const float d = 1.8e-3f * static_cast<float>(sr);
            const float dl = saturate(densityLineL.readAt(d), 2.4f);
            const float dr = saturate(densityLineR.readAt(d), 2.4f);
            densityLineL.write(inL);
            densityLineR.write(inR);
            const float wet = density * 0.8f;
            inL = (1.0f - wet) * inL + wet * dl;
            inR = (1.0f - wet) * inR + wet * dr;
        }
        // 6. planet character
        inL = biquads[4].process(inL);
        inR = biquads[4].process(inR);
        chorus.process(inL, inR, chorusDepth, chorusRate, sr);
        inL = biquads[5].process(inL);
        inR = biquads[5].process(inR);
        // 7. width (mid/side; width 0 = perfect mono)
        {
            const float m = 0.5f * (inL + inR);
            const float s = 0.5f * (inL - inR);
            const float k = width * 2.0f;
            inL = m + s * k;
            inR = m - s * k;
        }
        // 8. orbital reverb
        reverb.process(inL, inR, orbSize, orbDecay, reverbMix, sr);
        // 9. planet level + safety soft-clip
        const float level = std::pow(10.0, curChar.levelDb / 20.0);
        inL = softLimit(inL * level);
        inR = softLimit(inR * level);

        analyser.sample(inL, inR);
    }

    // End of block: advance the planet character crossfade (~0.5 s).
    void endBlock()
    {
        if (crossfade < 1.0f)
        {
            crossfade = juce::jmin(1.0f, crossfade + 0.0035f);
            curChar = lerpChar(fromChar, getPlanetProfiles()[targetPlanet], crossfade);
        }
    }

    const Analyser& getAnalyser() const { return analyser; }

private:
    static float saturate(float x, float drive)
    {
        const float d = juce::jmax(1.0f, drive);
        return std::tanh(d * x) / std::tanh(d);
    }

    static float softLimit(float x)
    {
        return std::tanh(1.3f * x) / std::tanh(1.3f);
    }

    static PlanetProfile lerpChar(const PlanetProfile& a, const PlanetProfile& b, float t)
    {
        auto lv = [t](float x, float y) { return x + (y - x) * t; };
        PlanetProfile c;
        c.name = a.name;
        c.descriptor = a.descriptor;
        c.gravity = lv(a.gravity, b.gravity);   c.atmosphere = lv(a.atmosphere, b.atmosphere);
        c.temperature = lv(a.temperature, b.temperature); c.density = lv(a.density, b.density);
        c.brightness = lv(a.brightness, b.brightness); c.harmonics = lv(a.harmonics, b.harmonics);
        c.reverb = lv(a.reverb, b.reverb);       c.width = lv(a.width, b.width);
        c.orbSize = lv(a.orbSize, b.orbSize);    c.orbDecaySeconds = lv(a.orbDecaySeconds, b.orbDecaySeconds);
        c.peakHz = std::exp(std::log(a.peakHz) + (std::log(b.peakHz) - std::log(a.peakHz)) * t);
        c.peakDb = lv(a.peakDb, b.peakDb);  c.peakQ = lv(a.peakQ, b.peakQ);
        c.chorusMs = lv(a.chorusMs, b.chorusMs); c.chorusHz = lv(a.chorusHz, b.chorusHz);
        c.airDb = lv(a.airDb, b.airDb);      c.driveAdd = lv(a.driveAdd, b.driveAdd);
        c.levelDb = lv(a.levelDb, b.levelDb);
        return c;
    }

    double sr = 44100.0;

    Smoothed smGravity, smAtmosphere, smTemperature, smDensity, smBrightness,
             smHarmonics, smReverb, smWidth, smOrbSize, smOrbDecay, smVelDepth, smMode;

    float gravity = 0.68f, atmosphere = 0.5f, temperature = 0.45f, density = 0.6f;
    float brightness = 0.62f, harmonics = 0.48f, reverbAmt = 0.35f, width = 0.7f;
    float orbSize = 0.68f, orbDecay = 1.8f, modeValue = 0.0f;
    float drive = 1.0f, reverbMix = 0.35f, chorusDepth = 0.0f, chorusRate = 0.5f;

    Biquad biquads[6];
    OrbitalReverb reverb;
    StereoChorus chorus;
    DelayLine densityLineL, densityLineR;
    Analyser analyser;

    int targetPlanet = 2;
    float crossfade = 1.0f;
    PlanetProfile fromChar = getPlanetProfiles()[2];
    PlanetProfile curChar = getPlanetProfiles()[2];
};

} // namespace aura
