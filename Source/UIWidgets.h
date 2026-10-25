#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Shared AURA palette
namespace auraUI
{
    inline juce::Colour bg()          { return juce::Colour(0xff050a14); }
    inline juce::Colour panel()       { return juce::Colour(0xe90a1425); }
    inline juce::Colour panelDeep()   { return juce::Colour(0xff060d1c); }
    inline juce::Colour cyan()        { return juce::Colour(0xff42d9ff); }
    inline juce::Colour blue()        { return juce::Colour(0xff416dff); }
    inline juce::Colour violet()      { return juce::Colour(0xffa45cff); }
    inline juce::Colour textDim()     { return juce::Colour(0xff8a9ab8); }
    inline juce::Colour textFaint()   { return juce::Colour(0xff55648a); }
    inline juce::Colour track()       { return juce::Colour(0xff1a2438); }
    inline juce::Colour outline()     { return juce::Colour(0x73416dff); }

    inline juce::Font font(float size, bool bold = false)
    {
        return juce::Font(juce::FontOptions(size,
                                            bold ? juce::Font::bold : juce::Font::plain));
    }

    inline float textWidth(juce::Graphics& g, const juce::String& t)
    {
        return g.getCurrentFont().getStringWidthFloat(t);
    }

    // This JUCE build's Graphics has no drawArc(): stroke a Path arc instead.
    inline void drawArc(juce::Graphics& g, const juce::Rectangle<float>& r,
                        float startDeg, float sweepDeg, float thickness)
    {
        juce::Path path;
        const float pi = static_cast<float>(M_PI);
        path.addArc(r.getX(), r.getY(), r.getWidth(), r.getHeight(),
                    startDeg * pi / 180.0f, (startDeg + sweepDeg) * pi / 180.0f, true);
        g.strokePath(path, juce::PathStrokeType(thickness));
    }

    // Wide-tracked text like the reference lettering.
    inline void drawTracked(juce::Graphics& g, const juce::String& text,
                            const juce::Rectangle<int>& bounds, int tracking, float size,
                            juce::Justification j = juce::Justification::centred)
    {
        g.setFont(font(size));
        juce::StringArray chars;
        for (int i = 0; i < text.length(); ++i)
            chars.add(juce::String::charToString(text[i]));
        float total = 0.0f;
        for (int i = 0; i < chars.size(); ++i)
            total += textWidth(g, chars[i]) + (i < chars.size() - 1 ? (float)tracking : 0.0f);
        float x = (float)bounds.getX();
        if (j == juce::Justification::centred)
            x = bounds.getCentreX() - total * 0.5f;
        else if (j == juce::Justification::right)
            x = (float)bounds.getRight() - total;
        const float y = bounds.getCentreY();
        for (int i = 0; i < chars.size(); ++i)
        {
            const float w = textWidth(g, chars[i]);
            g.drawText(chars[i], static_cast<int>(x), y,
                       static_cast<int>(w) + 2, bounds.getHeight(),
                       juce::Justification::centredLeft);
            x += w + (float)tracking;
        }
    }

    //==========================================================================
    // Procedural planet artwork (vector, no image assets).
    // index: 0 Mercury .. 8 Pluto
    inline void drawPlanet(juce::Graphics& g, const juce::Rectangle<float>& r, int index)
    {
        const float cx = r.getCentreX(), cy = r.getCentreY();
        const float rad = r.getWidth() * 0.5f;

        struct Colours { juce::Colour light, mid, dark; };
        const Colours c[] = {
            { juce::Colour(0xffc8ccd4), juce::Colour(0xff8a8f98), juce::Colour(0xff34363e) }, // Mercury
            { juce::Colour(0xfff2dcb8), juce::Colour(0xffcf9a5a), juce::Colour(0xff7a4a20) }, // Venus
            { juce::Colour(0xff7ac0e8), juce::Colour(0xff2a6ab0), juce::Colour(0xff0a1e3a) }, // Earth
            { juce::Colour(0xffe89068), juce::Colour(0xffc05a34), juce::Colour(0xff5a2414) }, // Mars
            { juce::Colour(0xffe8cfa8), juce::Colour(0xffb08058), juce::Colour(0xff5a3a20) }, // Jupiter
            { juce::Colour(0xfff0dcb0), juce::Colour(0xffc8a878), juce::Colour(0xff6a5238) }, // Saturn
            { juce::Colour(0xffa8ecf4), juce::Colour(0xff4aa8c8), juce::Colour(0xff1a4a6a) }, // Uranus
            { juce::Colour(0xff7a9af0), juce::Colour(0xff3a52c8), juce::Colour(0xff14205a) }, // Neptune
            { juce::Colour(0xffc8bcd4), juce::Colour(0xff8a7a98), juce::Colour(0xff3a2e48) }, // Pluto
        };
        const Colours& col = c[juce::jlimit(0, 8, index)];

        auto sphere = juce::Rectangle<float>(cx - rad, cy - rad, r.getWidth(), r.getWidth());

        // Surface detail ellipses that would extend beyond the sphere circle
        // are scaled down (about their own centre) so they stay inside it —
        // this JUCE build's Graphics has no clip-region restore, so the
        // details are fitted instead of clipped.
        auto fitInsideSphere = [&cx, &cy, rad](juce::Rectangle<float> e)
        {
            const float dx = e.getCentreX() - cx;
            const float dy = e.getCentreY() - cy;
            const float hw = e.getWidth() * 0.5f, hh = e.getHeight() * 0.5f;
            float maxD = 0.0f;
            for (float sx : { -1.0f, 1.0f })
                for (float sy : { -1.0f, 1.0f })
                    maxD = juce::jmax(maxD,
                                      std::sqrt((dx + sx * hw) * (dx + sx * hw)
                                             + (dy + sy * hh) * (dy + sy * hh)));
            if (maxD > rad * 0.985f)
            {
                const float sc = rad * 0.985f / maxD;
                e = e.withCentre({ e.getCentreX(), e.getCentreY() })
                     .withWidth(e.getWidth() * sc).withHeight(e.getHeight() * sc);
            }
            return e;
        };

        auto drawBody = [&]()
        {
            // base shading: light from upper-left
            juce::ColourGradient grad(
                col.light, sphere.getX() - rad * 0.1f, sphere.getY() + rad * 0.1f,
                col.dark,  sphere.getRight(), sphere.getBottom(), true);
            g.setGradientFill(grad);
            g.fillEllipse(sphere);

            switch (index)
            {
            case 0: // Mercury: craters
            {
                g.setColour(juce::Colour(0x66000000));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.25f).withWidth(rad * 0.4f).withHeight(rad * 0.35f));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.55f).withY(sphere.getY() + rad * 0.55f).withWidth(rad * 0.3f).withHeight(rad * 0.26f));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.15f).withY(sphere.getY() + rad * 0.6f).withWidth(rad * 0.24f).withHeight(rad * 0.2f));
                break;
            }
            case 1: // Venus: swirl bands
            {
                g.setColour(juce::Colour(0x40fff0d0));
                g.drawEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.1f).withY(sphere.getY() + rad * 0.3f)
                                              .withWidth(r.getWidth() * 0.9f).withHeight(rad * 0.28f)), 1.0f);
                g.setColour(juce::Colour(0x50000000));
                g.drawEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.05f).withY(sphere.getY() + rad * 0.6f)
                                              .withWidth(r.getWidth() * 0.95f).withHeight(rad * 0.3f)), 1.0f);
                break;
            }
            case 2: // Earth: land + cloud hints
            {
                g.setColour(juce::Colour(0xb83f8a52));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.2f).withY(sphere.getY() + rad * 0.2f)
                              .withWidth(rad * 0.55f).withHeight(rad * 0.4f));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.55f).withY(sphere.getY() + rad * 0.55f)
                              .withWidth(rad * 0.5f).withHeight(rad * 0.35f));
                g.setColour(juce::Colour(0x55ffffff));
                g.drawEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.1f).withY(sphere.getY() + rad * 0.4f)
                                              .withWidth(rad * 0.8f).withHeight(rad * 0.18f)), 1.2f);
                g.drawEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.3f).withY(sphere.getY() + rad * 0.65f)
                                              .withWidth(rad * 0.6f).withHeight(rad * 0.15f)), 1.0f);
                break;
            }
            case 3: // Mars: dark regions
            {
                g.setColour(juce::Colour(0x55301008));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.3f).withY(sphere.getY() + rad * 0.35f)
                              .withWidth(rad * 0.5f).withHeight(rad * 0.3f));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.15f).withY(sphere.getY() + rad * 0.65f)
                              .withWidth(rad * 0.35f).withHeight(rad * 0.22f));
                g.setColour(juce::Colour(0x66ffffff));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.42f).withY(sphere.getY() + rad * 0.02f)
                              .withWidth(rad * 0.3f).withHeight(rad * 0.16f)); // polar cap
                break;
            }
            case 4: // Jupiter: bands + spot
            {
                const float bandCols[5] = { 0, 0, 0, 0, 0 }; (void)bandCols;
                g.setColour(juce::Colour(0x55f0e0c0));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.05f).withY(sphere.getY() + rad * 0.18f)
                                              .withWidth(r.getWidth() * 0.95f).withHeight(rad * 0.16f)));
                g.setColour(juce::Colour(0x558a5a34));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.02f).withY(sphere.getY() + rad * 0.38f)
                                              .withWidth(r.getWidth() * 0.98f).withHeight(rad * 0.2f)));
                g.setColour(juce::Colour(0x45f0e0c0));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.05f).withY(sphere.getY() + rad * 0.6f)
                                              .withWidth(r.getWidth() * 0.95f).withHeight(rad * 0.16f)));
                g.setColour(juce::Colour(0x557a4a28));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.02f).withY(sphere.getY() + rad * 0.78f)
                                              .withWidth(r.getWidth() * 0.98f).withHeight(rad * 0.18f)));
                g.setColour(juce::Colour(0x88a04828));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.55f).withY(sphere.getY() + rad * 0.62f)
                              .withWidth(rad * 0.26f).withHeight(rad * 0.16f));
                break;
            }
            case 5: // Saturn: subtle bands (rings drawn by caller)
            {
                g.setColour(juce::Colour(0x40f0e0c0));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.05f).withY(sphere.getY() + rad * 0.3f)
                                              .withWidth(r.getWidth() * 0.95f).withHeight(rad * 0.18f)));
                g.setColour(juce::Colour(0x356a4a28));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.02f).withY(sphere.getY() + rad * 0.55f)
                                              .withWidth(r.getWidth() * 0.98f).withHeight(rad * 0.2f)));
                break;
            }
            case 6: // Uranus: faint band
            {
                g.setColour(juce::Colour(0x35ffffff));
                g.drawEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.1f).withY(sphere.getY() + rad * 0.42f)
                                              .withWidth(r.getWidth() * 0.9f).withHeight(rad * 0.2f)), 1.0f);
                break;
            }
            case 7: // Neptune: dark spot
            {
                g.setColour(juce::Colour(0x55081240));
                g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.45f).withY(sphere.getY() + rad * 0.4f)
                              .withWidth(rad * 0.4f).withHeight(rad * 0.26f));
                g.setColour(juce::Colour(0x3590a8ff));
                g.drawEllipse(sphere.withX(sphere.getX() + rad * 0.15f).withY(sphere.getY() + rad * 0.55f)
                              .withWidth(rad * 0.5f).withHeight(rad * 0.14f), 1.0f);
                break;
            }
            case 8: // Pluto: bright heart
            {
                g.setColour(juce::Colour(0x66f0e8ff));
                g.fillEllipse(fitInsideSphere(sphere.withX(sphere.getX() + rad * 0.42f).withY(sphere.getY() + rad * 0.42f)
                                              .withWidth(rad * 0.42f).withHeight(rad * 0.36f)));
                break;
            }
            }
        };

        drawBody();

        // Saturn ring: far (upper) half behind the planet, near (lower) half
        // in front — done with arc paths, then the planet body is repainted
        // over the far half (no clip-region restore available in this build).
        if (index == 5)
        {
            auto ring = sphere.withX(cx - rad * 1.55f).withY(cy - rad * 0.42f)
                          .withWidth(rad * 3.1f).withHeight(rad * 0.84f);
            auto ringArc = [&g, &ring](float a0, float a1, juce::Colour c1, float w1, juce::Colour c2, float w2)
            {
                const float pi = static_cast<float>(M_PI);
                const float r0 = a0 * pi / 180.0f, r1 = a1 * pi / 180.0f;
                auto ring2 = ring.reduced(3.0f, 1.2f);
                juce::Path p1, p2;
                p1.addArc(ring.getX(), ring.getY(), ring.getWidth(), ring.getHeight(), r0, r1, true);
                p2.addArc(ring2.getX(), ring2.getY(), ring2.getWidth(), ring2.getHeight(), r0, r1, true);
                g.setColour(c1); g.strokePath(p1, juce::PathStrokeType(w1));
                g.setColour(c2); g.strokePath(p2, juce::PathStrokeType(w2));
            };
            ringArc(180.0f, 360.0f, juce::Colour(0xaa8a7050), 1.4f, juce::Colour(0x66d8c0a0), 0.8f);
            drawBody();
            ringArc(0.0f, 180.0f, juce::Colour(0xcc8a7050), 1.4f, juce::Colour(0x80d8c0a0), 0.8f);
        }

        // limb darkening
        {
            juce::ColourGradient rim(juce::Colours::transparentBlack, cx, cy,
                                     juce::Colour(0x80000000), cx + rad * 0.95f, cy + rad * 0.95f, true);
            g.setGradientFill(rim);
            g.fillEllipse(sphere);
        }

        // specular highlight
        g.setColour(juce::Colour(0x30ffffff));
        g.fillEllipse(sphere.withX(sphere.getX() + rad * 0.12f).withY(sphere.getY() + rad * 0.06f)
                      .withWidth(rad * 0.45f).withHeight(rad * 0.35f));
    }

    inline void drawPlanetGlow(juce::Graphics& g, const juce::Rectangle<float>& r)
    {
        const float cx = r.getCentreX(), cy = r.getCentreY();
        const float rad = r.getWidth() * 0.5f;
        juce::ColourGradient glow(juce::Colour(0x0042d9ff), cx, cy, juce::Colour(0x5042d9ff), cx + rad * 1.9f, cy + rad * 1.9f, true);
        g.setGradientFill(glow);
        g.fillEllipse(cx - rad * 2.0f, cy - rad * 2.0f, rad * 4.0f, rad * 4.0f);
    }

    //==========================================================================
    // Knob: 270° rotary with cyan progress arc
    class AuraKnob final : public juce::Slider
    {
    public:
        AuraKnob()
        {
            setSliderStyle(RotaryHorizontalDrag);
            setTextBoxStyle(NoTextBox, false, 0, 0);
        }

        void paint(juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            const bool on = isMouseOver();
            const auto range = getRange();
            const float ratio = (float)juce::jlimit(0.0, 1.0, (getValue() - range.getStart())
                                     / std::max(1e-9, range.getEnd() - range.getStart()));

            // background arc
            g.setColour(track());
            drawArc(g, b, 135.0f, 270.0f, 2.0f);

            // progress arc
            if (ratio > 0.004f)
            {
                juce::Path p;
                const float pi = static_cast<float>(M_PI);
                p.addArc(b.getX(), b.getY(), b.getWidth(), b.getHeight(),
                         135.0f * pi / 180.0f, (135.0f + 270.0f * ratio) * pi / 180.0f, true);
                g.setColour(on ? juce::Colour(0xff7aeaff) : cyan());
                g.strokePath(p, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));
            }

            // body
            auto kb = b.reduced(b.getWidth() * 0.20f);
            juce::ColourGradient body(juce::Colour(0xff18263e), kb.getTopLeft(),
                                       juce::Colour(0xff070d1a), kb.getBottomRight(), true);
            g.setGradientFill(body);
            g.fillEllipse(kb);
            g.setColour(on ? juce::Colour(0xff42d9ff) : juce::Colour(0xff2a3c5e));
            g.drawEllipse(kb, 1.0f);

            // indicator
            const double angle = 135.0 + 270.0 * ratio;
            const double rad = kb.getWidth() * 0.5;
            const double dir = angle * M_PI / 180.0;
            const float x0 = kb.getCentreX() + (float)std::cos(dir) * rad * 0.55f;
            const float y0 = kb.getCentreY() + (float)std::sin(dir) * rad * 0.55f;
            const float x1 = kb.getCentreX() + (float)std::cos(dir) * rad * 0.82f;
            const float y1 = kb.getCentreY() + (float)std::sin(dir) * rad * 0.82f;
            g.setColour(on ? juce::Colours::white : juce::Colour(0xffcfe8ff));
            g.drawLine(x0, y0, x1, y1, 2.0f);
        }
    };

    //==========================================================================
    // Slim horizontal bar slider (used next to knobs)
    class AuraBarSlider final : public juce::Slider
    {
    public:
        AuraBarSlider()
        {
            setSliderStyle(LinearHorizontal);
            setTextBoxStyle(NoTextBox, false, 0, 0);
        }

        void paint(juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            const float y = b.getCentreY();
            const auto range = getRange();
            const float ratio = (float)juce::jlimit(0.0, 1.0, (getValue() - range.getStart())
                                     / std::max(1e-9, range.getEnd() - range.getStart()));

            g.setColour(track());
            g.fillRoundedRectangle(b.withHeight(3.0f), 1.5f);

            if (ratio > 0.004f)
            {
                juce::ColourGradient fill(blue(), b.getX(), y,
                                           cyan(), b.getX() + b.getWidth() * ratio, y, false);
                g.setGradientFill(fill);
                g.fillRoundedRectangle(juce::Rectangle<float>(b.getX(), y - 1.5f,
                                                              b.getWidth() * ratio, 3.0f), 1.5f);
            }

            // thumb dot
            const float tx = b.getX() + b.getWidth() * ratio;
            g.setColour(juce::Colour(0xffeaf6ff));
            g.fillEllipse(tx - 3.0f, y - 3.0f, 6.0f, 6.0f);
        }
    };

    //==========================================================================
    // Piano keyboard component: click/drag playback, octave shift,
    // highlights notes the engine is currently playing.
    class AuraKeyboard final : public juce::Component
    {
    public:
        explicit AuraKeyboard(AURAAudioProcessor& p) : proc(p)
        {
            setMouseCursor(juce::MouseCursor::NormalCursor);
        }

        void setOctave(int o) { octaveShift = juce::jlimit(-4, 4, o); repaint(); }
        int getOctave() const { return octaveShift; }

        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colour(0xff04070f));

            const int H = getHeight();
            const int W = getWidth();
            const int startMidi = 48 + 12 * octaveShift; // C3 + octave shift
            const int numWhite = 25;
            const float kw = (float)W / numWhite;

            // white keys
            for (int i = 0; i < numWhite; ++i)
            {
                const int midi = midiOfWhite(startMidi, i);
                auto kb = juce::Rectangle<float>(i * kw + 1.0f, 4.0f, kw - 2.0f, (float)H - 8.0f);
                const bool active = noteIsActive(midi);

                if (active)
                {
                    juce::ColourGradient ag(juce::Colour(0xff9adfff), kb.getTopLeft(),
                                             juce::Colour(0xff5a7aff), kb.getBottomRight(), true);
                    g.setGradientFill(ag);
                }
                else
                {
                    juce::ColourGradient wg(juce::Colour(0xffe9edf6), kb.getTopLeft(),
                                             juce::Colour(0xff9aa4b8), kb.getBottomLeft(), true);
                    wg.addColour(0.85f, juce::Colour(0xffc4cddd));
                    g.setGradientFill(wg);
                }
                g.fillRoundedRectangle(kb, 2.5f);
                g.setColour(juce::Colour(0xff060a14));
                g.fillRect((float)(i * kw), 4.0f, 1.0f, (float)H - 8.0f);
            }

            // black keys (none after E or B)
            for (int i = 0; i < numWhite - 1; ++i)
            {
                const int midi = midiOfWhite(startMidi, i);
                const int cls = midi % 12;
                if (cls == 4 || cls == 11) continue; // E / B: no sharp follows
                const int sharp = midi + 1;
                const float bw = kw * 0.62f;
                const float bx = i * kw + kw - bw * 0.52f;
                auto bk = juce::Rectangle<float>(bx, 4.0f, bw, (float)H * 0.58f);

                if (noteIsActive(sharp))
                    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff6acfff), bk.getTopLeft(),
                                                           juce::Colour(0xff3a4ae0), bk.getBottomRight(), true));
                else
                    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff232c40), bk.getTopLeft(),
                                                           juce::Colour(0xff0a0f1c), bk.getBottomRight(), true));
                g.fillRoundedRectangle(bk, 2.0f);
                g.setColour(juce::Colour(0x6642d9ff));
                g.drawRoundedRectangle(bk, 2.0f, 1.0f);
            }
        }

        void mouseDown(const juce::MouseEvent& e) override { playAt(e.x, true); }
        void mouseDrag(const juce::MouseEvent& e) override { playAt(e.x, true); }
        void mouseUp(const juce::MouseEvent&) override
        {
            for (int n : held)
                proc.synth.noteOff(1, n, 0.0f, true);
            held.clear();
            currentKey = -1;
        }

    private:
        static bool isWhiteMidi(int midi) { return (midi % 12) % 7 != 3 && (midi % 12) % 7 != 6; }

        static int midiOfWhite(int startMidi, int index)
        {
            int midi = startMidi;
            int i = 0;
            while (i < index)
            {
                ++midi;
                if (isWhiteMidi(midi)) ++i;
            }
            return midi;
        }

        int noteAtX(int x) const
        {
            const int W = getWidth();
            const int startMidi = 48 + 12 * octaveShift;
            const int numWhite = 25;
            const float kw = (float)W / numWhite;
            const int i = juce::jlimit(0, numWhite - 1, static_cast<int>(x / kw));
            return midiOfWhite(startMidi, i);
        }

        bool noteIsActive(int midi) const
        {
            const auto& notes = proc.activeNotes;
            const uint64_t bits = (midi < 64) ? notes[0].load(std::memory_order_relaxed)
                                              : notes[1].load(std::memory_order_relaxed);
            const int bit = midi & 63;
            return (bits >> bit) & 1ULL;
        }

        void playAt(int x, bool allow)
        {
            if (!allow) return;
            const int note = noteAtX(x);
            if (note == currentKey)
                return;
            if (currentKey >= 0)
            {
                proc.synth.noteOff(1, currentKey, 0.0f, true);
                held.erase(currentKey);
            }
            currentKey = note;
            if (!held.count(note))
            {
                proc.synth.noteOn(1, note, 0.85f);
                held.insert(note);
            }
        }

        AURAAudioProcessor& proc;
        int octaveShift = 0;
        int currentKey = -1;
        std::set<int> held;
    };

} // namespace auraUI
