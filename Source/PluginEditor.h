#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UIWidgets.h"

#include <array>
#include <functional>
#include <memory>
#include <set>

class NoteEnvelopePanel;

//==============================================================================
class AURAAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                       private juce::FileDragAndDropTarget,
                                       private juce::Timer
{
public:
    explicit AURAAudioProcessorEditor(AURAAudioProcessor&);
    ~AURAAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;

private:
    // ---- small custom buttons ---------------------------------------------
    struct ArrowButton : public juce::Button
    {
        explicit ArrowButton(bool right) : juce::Button(""), flip(right) {}
        void paintButton(juce::Graphics& g, bool over, bool down) override
        {
            auto b = getLocalBounds().toFloat().toFloat();
            g.setColour((over || down) ? auraUI::cyan() : auraUI::textDim());
            juce::Path p;
            const float cx = b.getCentreX(), cy = b.getCentreY(), s = 7.0f;
            if (!flip)
            {
                p.startNewSubPath(cx + s * 0.4f, cy - s);
                p.lineTo(cx - s * 0.6f, cy);
                p.lineTo(cx + s * 0.4f, cy + s);
            }
            else
            {
                p.startNewSubPath(cx - s * 0.4f, cy - s);
                p.lineTo(cx + s * 0.6f, cy);
                p.lineTo(cx - s * 0.4f, cy + s);
            }
            g.strokePath(p, juce::PathStrokeType(2.0f));
        }
        bool flip = false;
    };

    struct ModeButton final : public juce::Button
    {
        ModeButton() : juce::Button("") {}
        void setModeText(const juce::String& t) { label = t; repaint(); }
        juce::String getModeText() const { return label; }
        juce::String label = "Realistic";
        void paintButton(juce::Graphics& g, bool, bool down) override
        {
            auto b = getLocalBounds();
            g.setColour(down ? juce::Colour(0xff14213a) : juce::Colour(0xff0a1322));
            g.fillRoundedRectangle(b.toFloat(), 12.0f);
            g.setColour(juce::Colour(0x55416dff));
            g.drawRoundedRectangle(b.toFloat(), 12.0f, 1.0f);

            g.setColour(juce::Colour(0xffdce8ff));
            g.setFont(auraUI::font(13.0f));
            g.drawText(label, b.reduced(26, 0), juce::Justification::centredLeft);
            g.setColour(auraUI::textDim());
            juce::Path p;
            const float cx = b.getRight() - 16.0f, cy = b.getCentreY();
            p.startNewSubPath(cx - 4.0f, cy - 2.5f);
            p.lineTo(cx, cy + 2.5f);
            p.lineTo(cx + 4.0f, cy - 2.5f);
            g.strokePath(p, juce::PathStrokeType(1.5f));
        }
    };

    struct SourceButton final : public juce::Button
    {
        explicit SourceButton(AURAAudioProcessor& p) : juce::Button(""), proc(p) {}
        void paintButton(juce::Graphics& g, bool over, bool down) override;
        AURAAudioProcessor& proc;
    };

    struct MenuButton final : public juce::Button
    {
        MenuButton() : juce::Button("") {}
        void paintButton(juce::Graphics& g, bool over, bool down) override
        {
            auto b = getLocalBounds().toFloat();
            g.setColour(down ? juce::Colour(0xff14213a) : (over ? juce::Colour(0xff0d1830) : juce::Colour(0xff0a1322)));
            g.fillRoundedRectangle(b, 12.0f);
            g.setColour(juce::Colour(0x55416dff));
            g.drawRoundedRectangle(b, 12.0f, 1.0f);
            g.setColour(over ? auraUI::cyan() : juce::Colour(0xffb8c8e8));
            for (int i = -1; i <= 1; ++i)
                g.fillRoundedRectangle(juce::Rectangle<float>(b.getCentreX() - 9.0f,
                                                              b.getCentreY() + i * 7.0f - 1.0f,
                                                              18.0f, 2.0f), 1.0f);
        }
    };

    struct TabButton final : public juce::Button
    {
        explicit TabButton(const juce::String& t) : juce::Button(t) {}
        int viewId = 0;
        int activeView = -1;
        void paintButton(juce::Graphics& g, bool, bool down) override
        {
            const bool active = (viewId == activeView);
            g.setColour(active ? auraUI::cyan() : juce::Colour(0x558a9ab8));
            g.fillEllipse(4.0f, getLocalBounds().getCentreY() - 2.5f, 5.0f, 5.0f);
            g.setColour(active ? juce::Colour(0xffeaf6ff) : juce::Colour(0xff7484a8));
            g.setFont(auraUI::font(10.5f, active));
            g.drawText(getButtonText(), 16.0f, 0,
                       getLocalBounds().getWidth() - 16, getLocalBounds().getHeight(),
                       juce::Justification::centredLeft);
            if (active)
            {
                g.setColour(auraUI::cyan());
                g.fillRect(16.0f, (float)getLocalBounds().getBottom() - 5.0f,
                           (float)getLocalBounds().getWidth() - 24.0f, 1.5f);
            }
            (void)down;
        }
    };

    struct ThumbButton final : public juce::Button
    {
        explicit ThumbButton(std::function<int()> idxGetter) : juce::Button(""), getIdx(std::move(idxGetter)) {}
        void paintButton(juce::Graphics& g, bool, bool) override
        {
            const int idx = getIdx ? getIdx() : 2;
            auto b = getLocalBounds().toFloat();
            const auto circle = juce::Rectangle<float>(b.getX(), b.getCentreY() - 26.0f, 52.0f, 52.0f);
            auraUI::drawPlanet(g, circle, idx);
            g.setColour(juce::Colour(0x8842d9ff));
            g.drawEllipse(circle, 1.0f);
        }
        std::function<int()> getIdx;
    };

    // ---- members -----------------------------------------------------------
    AURAAudioProcessor& audioProcessor;

    auraUI::AuraKnob gravityKnob, atmosphereKnob, temperatureKnob, densityKnob;
    auraUI::AuraKnob brightnessKnob, harmonicsKnob, reverbKnob, widthKnob;
    auraUI::AuraKnob sizeKnob, decayKnob;
    auraUI::AuraBarSlider brightnessBar, harmonicsBar, reverbBar, widthBar;
    auraUI::AuraBarSlider sizeBar, decayBar;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> gravityAtt, atmosphereAtt, temperatureAtt, densityAtt;
    std::unique_ptr<Attachment> brightnessAtt, harmonicsAtt, reverbAtt, widthAtt;
    std::unique_ptr<Attachment> sizeAtt, decayAtt;

    ArrowButton planetLeft { false }, planetRight { true }, orbitLeft { false }, orbitRight { true };
    ThumbButton planetThumb;
    ModeButton modeBtn;
    SourceButton sourceBtn;
    MenuButton menuBtn;
    TabButton tabSpectrum { "SPECTRUM" }, tabPartials { "PARTIALS" }, tabEnvelope { "ENVELOPE" };
    ArrowButton octDown { false }, octUp { true };
    auraUI::AuraKeyboard keyboard;

    aura::PlanetProfile animTargets;
    bool animActive = false;
    int lastPlanetSeen = 2;
    int planetIndex = 2;
    int octaveShift = 0;
    int spectrumView = 0;
    void setSpectrumView(int v);
    bool isDragOver = false;
    bool isLoadingSample = false;

    std::unique_ptr<juce::DocumentWindow> noteEnvWindow;
    std::unique_ptr<NoteEnvelopePanel> noteEnvContent;

    aura::Analyser::Frame specFrame;
    std::array<float, 256> envBuf { 0.0f };
    float inLevel = 0.0f, outLevel = 0.0f;
    std::set<juce::String> userDragging;

    // layout caches (computed in resized)
    juce::Rectangle<int> topBar, orbitPanel, panelsRow, keyRow;
    juce::Rectangle<int> psPanel, specPanel, scPanel, orbPanel;
    std::array<juce::Rectangle<float>, 9> planetRects {};
    juce::Rectangle<int> sourceZone, modeZone, pillZone;

    // ---- helpers -------------------------------------------------------------
    int currentPlanet() const;
    void setPlanet(int idx);
    void styleKnob(auraUI::AuraKnob&, const juce::String& paramId);
    void styleBar(auraUI::AuraBarSlider&, const juce::String& paramId);
    void loadSampleFromChooser();
    void showMainMenu();
    void showNoteEnvelopeWindow();
    void drawVUMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds, float level);
    void drawWaveformIcon(juce::Graphics& g, const juce::Rectangle<float>& r, juce::Colour c);
    void drawMiniWave(juce::Graphics& g, const juce::Rectangle<float>& r);
    void drawControlIcon(juce::Graphics& g, const juce::Rectangle<float>& r, int kind);
    void updateVisuals();
    void animatePlanetProfile();

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray&) override;
    void filesDropped(const juce::StringArray&, int, int) override;
    // Timer
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AURAAudioProcessorEditor)
};
