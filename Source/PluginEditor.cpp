#include "PluginEditor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//==============================================================================
// Note & Envelope panel (shown from the menu): root note + ADSR + velocity
class NoteEnvelopePanel final : public juce::Component
{
public:
    explicit NoteEnvelopePanel(AURAAudioProcessor& p) : proc(p)
    {
        auto make = [this](juce::Slider& s, const char* id, const char* label, int row)
        {
            s.setSliderStyle(juce::Slider::LinearHorizontal);
            s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 58, 20);
            s.setColour(juce::Slider::trackColourId, juce::Colour(0xff1a2438));
            s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff42d9ff));
            s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdce8ff));
            s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0x44416dff));
            s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0x00000000));
            s.setName(label);
            addAndMakeVisible(s);

            auto* l = new juce::Label(label, label);
            l->setFont(auraUI::font(10.0f));
            l->setColour(juce::Label::textColourId, auraUI::textDim());
            rowLabels[row] = l;
            addAndMakeVisible(*l);
            attachments.push_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.parameters, id, s));
            sliders[row] = &s;
        };

        root.setRange(21.0, 108.0, 1.0);
        make(root, "rootNote", "ROOT NOTE", 0);
        make(attack, "attack", "ATTACK", 1);
        make(decay, "decay", "DECAY", 2);
        make(sustain, "sustain", "SUSTAIN", 3);
        make(release, "release", "RELEASE", 4);
        make(velocity, "velDepth", "VELOCITY", 5);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff081120));
    }

    void resized() override
    {
        const int y0 = 14;
        for (int i = 0; i < 6; ++i)
        {
            const int y = y0 + i * 46;
            rowLabels[i]->setBounds(16, y + 12, 110, 18);
            sliders[i]->setBounds(130, y + 6, getWidth() - 146, 26);
        }
    }

private:
    AURAAudioProcessor& proc;
    juce::Slider root, attack, decay, sustain, release, velocity;
    juce::Label* rowLabels[6] = { nullptr };
    juce::Slider* sliders[6] = { nullptr };
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
};

//==============================================================================
void AURAAudioProcessorEditor::SourceButton::paintButton(juce::Graphics& g, bool over, bool down)
{
    auto b = getLocalBounds().toFloat();
    const bool has = proc.hasSample();

    g.setColour(down ? juce::Colour(0xff14213a) : (over ? juce::Colour(0xff0e1a30) : juce::Colour(0xff0a1322)));
    g.fillRoundedRectangle(b, 12.0f);
    g.setColour(over ? juce::Colour(0x8842d9ff) : juce::Colour(0x55416dff));
    g.drawRoundedRectangle(b, 12.0f, 1.0f);

    const float ix = b.getX() + 16.0f;
    const float ic = b.getCentreY();
    g.setColour(has ? auraUI::cyan() : auraUI::textDim());

    bool sampleAudible = false;
    for (float pk : proc.getWaveformPeaks())
    {
        if (pk > 1e-4f) { sampleAudible = true; break; }
    }
    if (has && sampleAudible)
    {
        // mini waveform from the loaded sample
        const auto& peaks = proc.getWaveformPeaks();
        const int n = 16;
        const float gap = 3.4f;
        float scale = 0.0f;
        for (int i = 0; i < 128; ++i) scale = juce::jmax(scale, peaks[static_cast<size_t>(i)]);
        scale = scale > 0.0f ? 1.0f / scale : 1.0f;
        for (int i = 0; i < n; ++i)
        {
            const int from = 128 * i / n, to = juce::jmin(128, 128 * (i + 1) / n + 1);
            float pk = 0.0f;
            for (int k = from; k < to; ++k) pk = juce::jmax(pk, peaks[static_cast<size_t>(k)]);
            const float h = juce::jmax(2.0f, pk * scale * 26.0f);
            g.fillRoundedRectangle(juce::Rectangle<float>(ix + i * gap, ic - h * 0.5f, 2.0f, h), 1.0f);
        }
    }
    else
    {
        // static waveform icon
        const float hs[5] = { 6.0f, 12.0f, 20.0f, 10.0f, 16.0f };
        for (int i = 0; i < 5; ++i)
            g.fillRoundedRectangle(juce::Rectangle<float>(ix + i * 3.4f, ic - hs[i] * 0.5f, 2.0f, hs[i]), 1.0f);
    }

    const float tx = ix + 34.0f;
    const float tw = b.getWidth() - (tx - b.getX()) - 14.0f;

    g.setFont(auraUI::font(11.0f, true));
    g.setColour(juce::Colour(0xffdce8ff));
    juce::String line1 = has ? proc.getSampleName() : "PIANO SOURCE";
    if (auraUI::textWidth(g, line1) > tw)
    {
        line1 = line1.dropLastCharacters(1);
        while (line1.length() > 3 && auraUI::textWidth(g, line1 + "…") > tw)
            line1 = line1.dropLastCharacters(1);
        line1 << "…";
    }
    g.drawText(line1, static_cast<int>(tx), b.getY() + 9, static_cast<int>(tw), 18, juce::Justification::left);

    g.setFont(auraUI::font(9.5f));
    g.setColour(auraUI::textDim());
    juce::String line2;
    if (has)
    {
        const double d = proc.getSampleDurationSeconds();
        const int mm = (int)(d / 60.0), ss = (int)(d) % 60;
        const int dt = (int)((d - (int)d) * 100.0);
        line2 = String::formatted("%d:%02d.%02d", mm, ss, dt) + "  •  ONE-SHOT";
    }
    else
        line2 = "DROP A ONE-SHOT";
    g.drawText(line2, static_cast<int>(tx), b.getY() + 29, static_cast<int>(tw), 16, juce::Justification::left);
}

//==============================================================================
AURAAudioProcessorEditor::AURAAudioProcessorEditor(AURAAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), planetThumb([this] { return planetIndex; }),
      sourceBtn(p), keyboard(p)
{
    setSize(1640, 930);
    setResizable(true, true);
    setResizeLimits(1440, 818, 2200, 1248);
    setWantsKeyboardFocus(true);

    // knobs + bars
    styleKnob(gravityKnob, "gravity");
    styleKnob(atmosphereKnob, "atmosphere");
    styleKnob(temperatureKnob, "temperature");
    styleKnob(densityKnob, "density");
    styleKnob(brightnessKnob, "brightness");
    styleKnob(harmonicsKnob, "harmonics");
    styleKnob(reverbKnob, "reverb");
    styleKnob(widthKnob, "width");
    styleKnob(sizeKnob, "orbSize");
    styleKnob(decayKnob, "orbDecay");

    styleBar(brightnessBar, "brightness");
    styleBar(harmonicsBar, "harmonics");
    styleBar(reverbBar, "reverb");
    styleBar(widthBar, "width");
    styleBar(sizeBar, "orbSize");
    styleBar(decayBar, "orbDecay");

    gravityAtt = std::make_unique<Attachment>(audioProcessor.parameters, "gravity", gravityKnob);
    atmosphereAtt = std::make_unique<Attachment>(audioProcessor.parameters, "atmosphere", atmosphereKnob);
    temperatureAtt = std::make_unique<Attachment>(audioProcessor.parameters, "temperature", temperatureKnob);
    densityAtt = std::make_unique<Attachment>(audioProcessor.parameters, "density", densityKnob);
    brightnessAtt = std::make_unique<Attachment>(audioProcessor.parameters, "brightness", brightnessKnob);
    harmonicsAtt = std::make_unique<Attachment>(audioProcessor.parameters, "harmonics", harmonicsKnob);
    reverbAtt = std::make_unique<Attachment>(audioProcessor.parameters, "reverb", reverbKnob);
    widthAtt = std::make_unique<Attachment>(audioProcessor.parameters, "width", widthKnob);
    sizeAtt = std::make_unique<Attachment>(audioProcessor.parameters, "orbSize", sizeKnob);
    decayAtt = std::make_unique<Attachment>(audioProcessor.parameters, "orbDecay", decayKnob);

    // sliders also drive the bars (visual sync; dontSendNotification prevents loops)
    brightnessKnob.onValueChange = [this] { brightnessBar.setValue(brightnessKnob.getValue(), juce::dontSendNotification); };
    harmonicsKnob.onValueChange = [this] { harmonicsBar.setValue(harmonicsKnob.getValue(), juce::dontSendNotification); };
    reverbKnob.onValueChange = [this] { reverbBar.setValue(reverbKnob.getValue(), juce::dontSendNotification); };
    widthKnob.onValueChange = [this] { widthBar.setValue(widthKnob.getValue(), juce::dontSendNotification); };
    sizeKnob.onValueChange = [this] { sizeBar.setValue(sizeKnob.getValue(), juce::dontSendNotification); };
    decayKnob.onValueChange = [this] { decayBar.setValue(decayKnob.getValue(), juce::dontSendNotification); };
    brightnessBar.onValueChange = [this] { brightnessKnob.setValue(brightnessBar.getValue(), juce::dontSendNotification); };
    harmonicsBar.onValueChange = [this] { harmonicsKnob.setValue(harmonicsBar.getValue(), juce::dontSendNotification); };
    reverbBar.onValueChange = [this] { reverbKnob.setValue(reverbBar.getValue(), juce::dontSendNotification); };
    widthBar.onValueChange = [this] { widthKnob.setValue(widthBar.getValue(), juce::dontSendNotification); };
    sizeBar.onValueChange = [this] { sizeKnob.setValue(sizeBar.getValue(), juce::dontSendNotification); };
    decayBar.onValueChange = [this] { decayKnob.setValue(decayBar.getValue(), juce::dontSendNotification); };

    for (auto* k : { &gravityKnob, &atmosphereKnob, &temperatureKnob, &densityKnob,
                     &brightnessKnob, &harmonicsKnob, &reverbKnob, &widthKnob, &sizeKnob, &decayKnob })
    {
        addAndMakeVisible(*k);
        k->onDragStart = [this, k] { userDragging.insert(k->getName()); };
        k->onDragEnd = [this, k] { userDragging.erase(k->getName()); };
    }
    for (auto* bar : { &brightnessBar, &harmonicsBar, &reverbBar, &widthBar, &sizeBar, &decayBar })
    {
        addAndMakeVisible(*bar);
        bar->onDragStart = [this, bar] { userDragging.insert(bar->getName()); };
        bar->onDragEnd = [this, bar] { userDragging.erase(bar->getName()); };
    }

    // top bar buttons
    addAndMakeVisible(planetLeft);
    addAndMakeVisible(planetRight);
    addAndMakeVisible(planetThumb);
    addAndMakeVisible(modeBtn);
    addAndMakeVisible(sourceBtn);
    addAndMakeVisible(menuBtn);
    addAndMakeVisible(orbitLeft);
    addAndMakeVisible(orbitRight);
    addAndMakeVisible(tabSpectrum);
    addAndMakeVisible(tabPartials);
    addAndMakeVisible(tabEnvelope);
    addAndMakeVisible(octDown);
    addAndMakeVisible(octUp);
    addAndMakeVisible(keyboard);

    planetLeft.onClick = [this] { setPlanet(planetIndex - 1); };
    planetRight.onClick = [this] { setPlanet(planetIndex + 1); };
    orbitLeft.onClick = [this] { setPlanet(planetIndex - 1); };
    orbitRight.onClick = [this] { setPlanet(planetIndex + 1); };
    planetThumb.onClick = [this] { setPlanet(planetIndex + 1); };
    octDown.onClick = [this] { octaveShift = juce::jlimit(-4, 4, octaveShift - 1); keyboard.setOctave(octaveShift); };
    octUp.onClick = [this] { octaveShift = juce::jlimit(-4, 4, octaveShift + 1); keyboard.setOctave(octaveShift); };

    tabSpectrum.viewId = 0; tabPartials.viewId = 1; tabEnvelope.viewId = 2;
    setSpectrumView(0);
    tabSpectrum.onClick = [this] { setSpectrumView(0); };
    tabPartials.onClick = [this] { setSpectrumView(1); };
    tabEnvelope.onClick = [this] { setSpectrumView(2); };

    modeBtn.onClick = [this]
    {
        juce::PopupMenu m;
        m.addItem(1, "Realistic");
        m.addItem(2, "Cinematic");
        m.addItem(3, "Alien");
        m.showMenuAsync(juce::PopupMenu::Options().withMinimumWidth(140).withTargetComponent(&modeBtn),
                        [this](int id)
                        {
                            if (id == 0) return;
                            auto* pm = audioProcessor.parameters.getParameter("mode");
                            pm->beginChangeGesture();
                            pm->setValueNotifyingHost(pm->convertTo0to1((float)(id - 1)));
                            pm->endChangeGesture();
                        });
    };

    sourceBtn.onClick = [this] { loadSampleFromChooser(); };
    menuBtn.onClick = [this] { showMainMenu(); };

    audioProcessor.setSampleChangedCallback([this]
    {
        isLoadingSample = false;
        planetIndex = currentPlanet();
        repaint();
    });

    lastPlanetSeen = currentPlanet();
    planetIndex = lastPlanetSeen;
    animTargets = aura::getPlanetProfiles()[planetIndex];
    startTimerHz(30);
}

AURAAudioProcessorEditor::~AURAAudioProcessorEditor()
{
    stopTimer();
}

int AURAAudioProcessorEditor::currentPlanet() const
{
    const auto* pr = audioProcessor.parameters.getRawParameterValue("planet");
    return juce::jlimit(0, 8, static_cast<int>(pr->load()) - 1);
}

void AURAAudioProcessorEditor::setSpectrumView(int v)
{
    v = juce::jlimit(0, 2, v);
    if (spectrumView == v)
        return;
    spectrumView = v;
    tabSpectrum.activeView = v;
    tabPartials.activeView = v;
    tabEnvelope.activeView = v;
    tabSpectrum.repaint();
    tabPartials.repaint();
    tabEnvelope.repaint();
}

void AURAAudioProcessorEditor::setPlanet(int idx)
{
    idx = juce::jlimit(0, 8, idx);
    auto* p = audioProcessor.parameters.getParameter("planet");
    p->beginChangeGesture();
    p->setValueNotifyingHost(p->convertTo0to1((float)idx));
    p->endChangeGesture();
    planetIndex = idx;
    repaint();
}

void AURAAudioProcessorEditor::styleKnob(auraUI::AuraKnob& s, const juce::String& paramId)
{
    auto* param = audioProcessor.parameters.getParameter(paramId);
    s.setName(paramId);
    const auto nr = param->getNormalisableRange();
    s.setRange(nr.start, nr.end, nr.interval);
    s.setDoubleClickReturnValue(true, param->convertFrom0to1(param->getDefaultValue()));
}

void AURAAudioProcessorEditor::styleBar(auraUI::AuraBarSlider& s, const juce::String& paramId)
{
    auto* param = audioProcessor.parameters.getParameter(paramId);
    s.setName(paramId);
    const auto nr = param->getNormalisableRange();
    s.setRange(nr.start, nr.end, nr.interval);
    s.setValue(param->convertFrom0to1(param->getDefaultValue()), juce::dontSendNotification);
}

//==============================================================================
// File drag & drop
bool AURAAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& f : files)
    {
        const auto ext = juce::File(f).getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".aiff" || ext == ".aif" || ext == ".flac" || ext == ".ogg")
            return true;
    }
    return false;
}

void AURAAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)
{
    isDragOver = false;
    if (files.size() > 0)
    {
        isLoadingSample = true;
        audioProcessor.loadSampleAsync(juce::File(files[0]));
    }
    repaint();
}

void AURAAudioProcessorEditor::loadSampleFromChooser()
{
    auto chooser = std::make_shared<juce::FileChooser>("Choose a one-shot sample",
                                                       juce::File{},
                                                       "*.wav;*.aiff;*.aif;*.flac;*.ogg");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                         [this, chooser](const juce::FileChooser& fc)
                         {
                             const auto file = fc.getResult();
                             if (file.existsAsFile())
                             {
                                 isLoadingSample = true;
                                 audioProcessor.loadSampleAsync(file);
                                 repaint();
                             }
                         });
}

//==============================================================================
// Menu
void AURAAudioProcessorEditor::showMainMenu()
{
    juce::PopupMenu m;
    m.addItem(1, "Load One-Shot…");
    m.addItem(2, "Clear Sample", audioProcessor.hasSample());
    m.addSeparator();
    m.addItem(3, "Note & Envelope…");
    m.addItem(4, "Reset to Planet Defaults");
    m.addSeparator();
    m.addItem(5, "About AURA");
    m.showMenuAsync(juce::PopupMenu::Options().withMinimumWidth(200),
                    [this](int id)
    {
        switch (id)
        {
            case 1: loadSampleFromChooser(); break;
            case 2:
            {
                audioProcessor.clearSample();
                break;
            }
            case 3: showNoteEnvelopeWindow(); break;
            case 4:
            {
                const auto& pr = aura::getPlanetProfiles()[currentPlanet()];
                auto snap = [this](const char* id, float target)
                {
                    auto* p = audioProcessor.parameters.getParameter(id);
                    p->beginChangeGesture();
                    p->setValueNotifyingHost(p->convertTo0to1(target));
                    p->endChangeGesture();
                };
                snap("gravity", pr.gravity); snap("atmosphere", pr.atmosphere);
                snap("temperature", pr.temperature); snap("density", pr.density);
                snap("brightness", pr.brightness); snap("harmonics", pr.harmonics);
                snap("reverb", pr.reverb); snap("width", pr.width);
                snap("orbSize", pr.orbSize); snap("orbDecay", pr.orbDecaySeconds);
                animActive = false;
                break;
            }
            case 5:
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "AURA",
                    "AURA — Planetary One-Shot Instrument\n\n"
                    "D.A Production Instruments  •  v1.0.0\n\n"
                    "Load a one-shot, choose a planet and play it\n"
                    "chromatically with MIDI. Each planet transforms\n"
                    "the sample into a distinct, playable voice.");
                break;
            }
            default: break;
        }
    });
}

void AURAAudioProcessorEditor::showNoteEnvelopeWindow()
{
    if (noteEnvWindow == nullptr)
    {
        noteEnvContent = std::make_unique<NoteEnvelopePanel>(audioProcessor);
        noteEnvWindow = std::make_unique<juce::DocumentWindow>("Note & Envelope",
                                                               juce::Colour(0xff081120),
                                                               juce::DocumentWindow::closeButton);
        noteEnvWindow->setUsingNativeTitleBar(false);
        noteEnvWindow->setTitleBarHeight(28);
        noteEnvContent->setBounds(0, 28, 340, 300);
        noteEnvWindow->addAndMakeVisible(*noteEnvContent);
        noteEnvWindow->setSize(340, 328);
    }
    noteEnvWindow->setVisible(!noteEnvWindow->isVisible());
    noteEnvWindow->toFront(true);
}

//==============================================================================
// Per-tick updates (30 Hz)
void AURAAudioProcessorEditor::timerCallback()
{
    updateVisuals();
    animatePlanetProfile();

    static const char* modeNames[3] = { "Realistic", "Cinematic", "Alien" };
    const int mode = juce::jlimit(0, 2, static_cast<int>(
        audioProcessor.parameters.getRawParameterValue("mode")->load()));
    const juce::String modeText = modeNames[mode];
    if (modeBtn.getModeText() != modeText)
        modeBtn.setModeText(modeText);

    repaint();
}

void AURAAudioProcessorEditor::updateVisuals()
{
    const auto& a = audioProcessor.chain.getAnalyser();
    specFrame = a.readFrame();
    a.readEnvelope(envBuf.data(), (int)envBuf.size());
    inLevel = juce::jmax(audioProcessor.inputActivity.load(std::memory_order_relaxed), inLevel * 0.90f);
    outLevel = juce::jmax(a.readPeak(), outLevel * 0.86f);
}

void AURAAudioProcessorEditor::animatePlanetProfile()
{
    const int idx = currentPlanet();
    planetIndex = idx;

    if (idx != lastPlanetSeen)
    {
        animTargets = aura::getPlanetProfiles()[idx];
        animActive = true;
        lastPlanetSeen = idx;
    }

    if (!animActive)
        return;

    auto step = [this](const char* id, float targetRaw) -> bool
    {
        if (userDragging.count(id))
            return false;
        auto* p = audioProcessor.parameters.getParameter(id);
        const float cur = p->convertFrom0to1(p->getValue());
        float next = cur + (targetRaw - cur) * 0.18f;
        if (std::abs(targetRaw - next) < 0.0012f)
            next = targetRaw;
        p->setValueNotifyingHost(p->convertTo0to1(next));
        return std::abs(targetRaw - next) < 1e-6f;
    };

    bool done = true;
    done &= step("gravity", animTargets.gravity);
    done &= step("atmosphere", animTargets.atmosphere);
    done &= step("temperature", animTargets.temperature);
    done &= step("density", animTargets.density);
    done &= step("brightness", animTargets.brightness);
    done &= step("harmonics", animTargets.harmonics);
    done &= step("reverb", animTargets.reverb);
    done &= step("width", animTargets.width);
    done &= step("orbSize", animTargets.orbSize);
    done &= step("orbDecay", animTargets.orbDecaySeconds);
    if (done)
        animActive = false;
}

//==============================================================================
// Orbit planet hit testing
void AURAAudioProcessorEditor::mouseDown(const juce::MouseEvent& e)
{
    for (int i = 0; i < 9; ++i)
    {
        auto r = planetRects[static_cast<size_t>(i)].reduced(-14.0f);
        if (r.contains(e.position))
        {
            setPlanet(aura::getOrbitVisualOrder()[static_cast<size_t>(i)]);
            return;
        }
    }
}

//==============================================================================
// Painting helpers
static void drawPanel(juce::Graphics& g, const juce::Rectangle<int>& r, const juce::String& title, int icon)
{
    g.setColour(juce::Colour(0xff070d1c));
    g.fillRoundedRectangle(r.toFloat().reduced(1.0f), 15.0f);
    g.setColour(juce::Colour(0x55416dff));
    g.drawRoundedRectangle(r.toFloat().reduced(1.0f), 15.0f, 1.0f);

    auto ir = juce::Rectangle<float>(r.getX() + 18.0f, r.getY() + 13.0f, 16.0f, 16.0f);
    g.setColour(auraUI::cyan());
    switch (icon)
    {
        case 0: // planet + orbit
            g.fillEllipse(ir.withX(ir.getCentreX() - 4.5f).withWidth(9.0f).withHeight(9.0f));
            g.drawEllipse(ir, 1.0f);
            break;
        case 1: // spectrum bars
            g.fillRect(ir.getX(), ir.getY() + 7.0f, 2.5f, 9.0f);
            g.fillRect(ir.getX() + 5.0f, ir.getY() + 3.0f, 2.5f, 13.0f);
            g.fillRect(ir.getX() + 10.0f, ir.getY() + 5.0f, 2.5f, 11.0f);
            g.fillRect(ir.getX() + 15.0f, ir.getY() + 8.0f, 2.5f, 8.0f);
            break;
        case 2: // sound / sun
            g.fillEllipse(ir.withX(ir.getCentreX() - 4.0f).withWidth(8.0f).withHeight(8.0f));
            for (int a = 0; a < 8; ++a)
            {
                const double ang = a * M_PI / 4.0;
                g.drawLine(ir.getCentreX() + (float)std::cos(ang) * 6.0f,
                           ir.getCentreY() + (float)std::sin(ang) * 6.0f,
                           ir.getCentreX() + (float)std::cos(ang) * 8.0f,
                           ir.getCentreY() + (float)std::sin(ang) * 8.0f, 1.2f);
            }
            break;
        case 3: // orbital reverb
            g.fillEllipse(ir.withX(ir.getCentreX() - 3.0f).withWidth(6.0f).withHeight(6.0f));
            g.drawEllipse(ir.reduced(1.0f), 1.0f);
            break;
    }

    g.setColour(auraUI::cyan());
    g.setFont(auraUI::font(11.0f, true));
    juce::String spaced;
    for (int i = 0; i < title.length(); ++i)
        spaced << (title[i] == ' ' ? "  " : juce::String::charToString(title[i]));
    g.drawText(spaced, static_cast<int>(ir.getRight()) + 8, r.getY() + 12, 260, 18, juce::Justification::left);
}

void AURAAudioProcessorEditor::drawControlIcon(juce::Graphics& g, const juce::Rectangle<float>& r, int kind)
{
    g.setColour(juce::Colour(0xff7484a8));
    const float cx = r.getCentreX(), cy = r.getCentreY();
    switch (kind)
    {
        case 4: // gravity: planet + orbit
            g.fillEllipse(cx - 3.5f, cy - 3.5f, 7.0f, 7.0f);
            g.drawEllipse(r.reduced(1.0f), 1.0f);
            break;
        case 5: // atmosphere: waves
            for (int i = 0; i < 3; ++i)
            {
                juce::Path w;
                const float y = cy - 5.0f + i * 5.0f;
                w.startNewSubPath(cx - 8.0f, y);
                w.quadraticTo(cx - 4.0f, y - 3.0f, cx, y);
                w.quadraticTo(cx + 4.0f, y + 3.0f, cx + 8.0f, y);
                g.strokePath(w, juce::PathStrokeType(1.2f));
            }
            break;
        case 6: // temperature: thermometer
            g.drawLine(cx, cy - 8.0f, cx, cy + 3.0f, 1.4f);
            g.fillEllipse(cx - 3.0f, cy + 3.0f, 6.0f, 6.0f);
            g.setColour(auraUI::cyan());
            g.fillEllipse(cx - 1.5f, cy + 4.5f, 3.0f, 3.0f);
            break;
        case 7: // density: molecule
            g.drawLine(cx - 5.0f, cy + 4.0f, cx, cy - 4.0f, 1.0f);
            g.drawLine(cx, cy - 4.0f, cx + 5.0f, cy + 4.0f, 1.0f);
            g.drawEllipse(cx - 7.0f, cy + 2.0f, 4.5f, 4.5f, 1.0f);
            g.drawEllipse(cx - 2.2f, cy - 6.0f, 4.5f, 4.5f, 1.0f);
            g.drawEllipse(cx + 2.5f, cy + 2.0f, 4.5f, 4.5f, 1.0f);
            break;
        case 8: // brightness: sun
            g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
            for (int a = 0; a < 8; ++a)
            {
                const double ang = a * M_PI / 4.0;
                g.drawLine(cx + (float)std::cos(ang) * 5.0f, cy + (float)std::sin(ang) * 5.0f,
                           cx + (float)std::cos(ang) * 8.0f, cy + (float)std::sin(ang) * 8.0f, 1.1f);
            }
            break;
        case 9: // harmonics: pulse
        {
            juce::Path p;
            p.startNewSubPath(cx - 8.0f, cy);
            p.lineTo(cx - 3.0f, cy);
            p.lineTo(cx - 1.0f, cy - 6.0f);
            p.lineTo(cx + 1.5f, cy + 6.0f);
            p.lineTo(cx + 3.5f, cy);
            p.lineTo(cx + 8.0f, cy);
            g.strokePath(p, juce::PathStrokeType(1.3f));
            break;
        }
        case 10: // reverb: arcs
            for (int i = 0; i < 3; ++i)
                auraUI::drawArc(g, juce::Rectangle<float>(cx - 4.0f - i * 3.0f, cy - 4.0f - i * 3.0f,
                                                          8.0f + i * 6.0f, 8.0f + i * 6.0f),
                                -60.0f, 120.0f, 1.1f);
            break;
        case 11: // width: brackets
            g.drawLine(cx - 8.0f, cy - 5.0f, cx - 8.0f, cy + 5.0f, 1.2f);
            g.drawLine(cx - 8.0f, cy - 5.0f, cx - 5.0f, cy - 5.0f, 1.2f);
            g.drawLine(cx - 8.0f, cy + 5.0f, cx - 5.0f, cy + 5.0f, 1.2f);
            g.drawLine(cx + 8.0f, cy - 5.0f, cx + 8.0f, cy + 5.0f, 1.2f);
            g.drawLine(cx + 8.0f, cy - 5.0f, cx + 5.0f, cy - 5.0f, 1.2f);
            g.drawLine(cx + 8.0f, cy + 5.0f, cx + 5.0f, cy + 5.0f, 1.2f);
            g.setColour(auraUI::cyan());
            g.drawLine(cx - 2.0f, cy, cx + 2.0f, cy, 1.2f);
            break;
    }
}

void AURAAudioProcessorEditor::drawVUMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds, float level)
{
    const int segs = 12;
    const float sh = (float)bounds.getHeight() / segs;
    const int lit = (int)std::round(segs * juce::jmin(1.0f, level * 2.2f));
    for (int i = 0; i < segs; ++i)
    {
        const int fromBottom = i;
        auto s = juce::Rectangle<float>(bounds.toFloat().getX(),
                                        bounds.getBottom() - (fromBottom + 1) * sh,
                                        (float)bounds.getWidth(), sh - 1.6f);
        if (i < lit)
        {
            g.setColour(i >= segs - 3 ? juce::Colour(0xffe8c355) : (i >= segs - 5 ? juce::Colour(0xff58d68a) : juce::Colour(0xff2fbf6e)));
        }
        else
        {
            g.setColour(juce::Colour(0xff0d2018));
        }
        g.fillRoundedRectangle(s, 1.0f);
    }
}

//==============================================================================
void AURAAudioProcessorEditor::paint(juce::Graphics& g)
{
    const int W = getWidth(), H = getHeight();
    g.fillAll(auraUI::bg());
    {
        juce::ColourGradient grad(juce::Colour(0xff0d1730), 0.0f, 0.0f, juce::Colour(0xff03060d), (float)W * 0.5f, (float)H, true);
        g.setGradientFill(grad);
        g.fillAll();
    }

    // faint starfield across the whole window
    {
        juce::Random rng(777);
        for (int i = 0; i < 140; ++i)
        {
            const float x = rng.nextFloat() * W, y = rng.nextFloat() * H;
            g.setColour(juce::Colours::white.withAlpha(0.03f + rng.nextFloat() * 0.10f));
            const float r = 0.5f + rng.nextFloat() * 1.2f;
            g.fillEllipse(x, y, r, r);
        }
    }

    const int planetIdx = currentPlanet();
    const auto& profile = aura::getPlanetProfiles()[static_cast<size_t>(planetIdx)];

    //==========================================================================
    // Top bar
    {
        auto tb = topBar;
        g.setColour(juce::Colour(0xd90a1322));
        g.fillRoundedRectangle(tb.toFloat().reduced(1.0f), 18.0f);
        g.setColour(juce::Colour(0x55416dff));
        g.drawRoundedRectangle(tb.toFloat().reduced(1.0f), 18.0f, 1.0f);

        // logo: A U [planet] R A
        {
            const float lx = 74.0f, ly = tb.getCentreY() - 2.0f;
            g.setFont(auraUI::font(26.0f, juce::Font::bold));
            g.setColour(juce::Colour(0xffeef4ff));
            const float sp = 26.0f;
            float x = lx;
            const juce::String auraTitle("AURA");
            for (int i = 0; i < auraTitle.length(); ++i)
            {
                const auto cc = auraTitle[i];
                if (cc == 'R')
                {
                    // planet glyph where the "O" would be
                    const auto pr = juce::Rectangle<float>(x + 4.5f, ly - 8.0f, 17.0f, 17.0f);
                    juce::ColourGradient pg(juce::Colour(0xfff2b060), pr.getTopLeft(), juce::Colour(0xff8a3a10), pr.getBottomRight(), true);
                    g.setGradientFill(pg);
                    g.fillEllipse(pr);
                    g.setColour(juce::Colour(0x88ffd8a0));
                    g.drawEllipse(pr.withX(pr.getX() - 3.5f).withWidth(pr.getWidth() + 7.0f)
                                   .reduced(0.0f, 6.0f), 1.0f);
                    g.setColour(juce::Colour(0xffeef4ff));
                }
                else
                {
                    const juce::String s = juce::String::charToString(cc);
                    g.setFont(auraUI::font(26.0f, juce::Font::bold));
                    g.drawText(s, (int)x, (int)ly - 17, 20, 32, juce::Justification::centred);
                }
                x += sp;
            }
            g.setColour(auraUI::textDim());
            auraUI::drawTracked(g, "PLANETS • SOUNDS • BEYOND",
                                juce::Rectangle<int>(76, tb.getBottom() - 20, 320, 14), 4, 8.5f,
                                juce::Justification::left);
        }

        // planet pill
        {
            auto pz = pillZone.toFloat().reduced(1.0f);
            g.setColour(juce::Colour(0xe60a1424));
            g.fillRoundedRectangle(pz, 27.0f);
            g.setColour(juce::Colour(0x66416dff));
            g.drawRoundedRectangle(pz, 27.0f, 1.2f);

            g.setColour(juce::Colour(0xffeaf2ff));
            g.setFont(auraUI::font(17.0f, true));
            g.drawText(profile.name, pillZone.getX() + 118, pillZone.getY() + 14, 210, 32,
                       juce::Justification::centredLeft);
        }

        // source zone drag highlight
        if (isDragOver)
        {
            g.setColour(juce::Colour(0x5542d9ff));
            g.fillRoundedRectangle(sourceZone.toFloat().reduced(2.0f), 12.0f);
            g.setColour(auraUI::cyan());
            g.drawRoundedRectangle(sourceZone.toFloat().reduced(2.0f), 12.0f, 2.0f);
            g.setFont(auraUI::font(10.0f, true));
            g.drawText("DROP TO LOAD", sourceZone.getX() + 60, sourceZone.getCentreY() - 24,
                       160, 18, juce::Justification::centred);
        }
    }

    //==========================================================================
    // Orbit display
    {
        auto op = orbitPanel;
        juce::Rectangle<float> opr = op.toFloat().reduced(1.0f);
        juce::ColourGradient space(juce::Colour(0xff0a1124), opr.getTopLeft(), juce::Colour(0xff02040a), opr.getBottomRight(), true);
        g.setGradientFill(space);
        g.fillRoundedRectangle(opr, 20.0f);
        g.setColour(juce::Colour(0x4442d9ff));
        g.drawRoundedRectangle(opr, 20.0f, 1.0f);

        // (content below stays within the panel bounds, so no clipping needed)
        // nebula hints
        {
            auto neb1 = opr.withX(opr.getX() + opr.getWidth() * 0.12f).withY(opr.getY() + opr.getHeight() * 0.18f)
                           .withWidth(opr.getWidth() * 0.3f).withHeight(opr.getHeight() * 0.6f);
            g.setGradientFill(juce::ColourGradient(juce::Colour(0x1f6a4aff), neb1.getCentreX(), neb1.getCentreY(),
                                                   juce::Colour(0x00000000), neb1.getCentreX(), neb1.getCentreY() + neb1.getHeight() * 0.6f, true));
            g.fillEllipse(neb1);
            auto neb2 = opr.withX(opr.getX() + opr.getWidth() * 0.62f).withY(opr.getY() + opr.getHeight() * 0.25f)
                           .withWidth(opr.getWidth() * 0.34f).withHeight(opr.getHeight() * 0.6f);
            g.setGradientFill(juce::ColourGradient(juce::Colour(0x1a3a6aff), neb2.getCentreX(), neb2.getCentreY(),
                                                   juce::Colour(0x00000000), neb2.getCentreX(), neb2.getCentreY() + neb2.getHeight() * 0.6f, true));
            g.fillEllipse(neb2);
        }

        // stars
        {
            juce::Random rng(2026);
            for (int i = 0; i < 220; ++i)
            {
                const float x = opr.getX() + rng.nextFloat() * opr.getWidth();
                const float y = opr.getY() + rng.nextFloat() * opr.getHeight();
                const float r = 0.4f + rng.nextFloat() * 1.5f;
                g.setColour(juce::Colours::white.withAlpha(0.10f + rng.nextFloat() * 0.45f));
                g.fillEllipse(x, y, r, r);
            }
        }

        const float cx = opr.getCentreX();
        const float cy = opr.getCentreY() - 14.0f;

        // orbit ellipses
        for (int i = 0; i < 6; ++i)
        {
            const float rx = juce::jmin(130.0f + i * 122.0f, opr.getWidth() * 0.47f);
            const float ry = 26.0f + i * 21.0f;
            g.setColour(juce::Colours::white.withAlpha(0.10f + i * 0.015f));
            g.drawEllipse(cx - rx, cy - ry, rx * 2.0f, ry * 2.0f, 1.0f);
        }

        // top diamond marker
        {
            const float dy = opr.getY() + 14.0f;
            juce::Path d;
            d.startNewSubPath(cx, dy - 5.0f); d.lineTo(cx + 5.0f, dy);
            d.lineTo(cx, dy + 5.0f); d.lineTo(cx - 5.0f, dy);
            d.closeSubPath();
            g.setColour(juce::Colour(0x99a8dcff));
            g.fillPath(d);
        }

        // planets
        const float baseR = opr.getHeight() * 0.105f;
        const float factors[9] = { 0.55f, 0.80f, 1.15f, 0.85f, 1.0f, 1.05f, 0.85f, 0.85f, 0.50f };
        const float baselineY = opr.getBottom() - 74.0f;

        for (int slot = 0; slot < 9; ++slot)
        {
            const int idx = aura::getOrbitVisualOrder()[static_cast<size_t>(slot)];
            const bool selected = (idx == planetIdx);
            const float px = opr.getX() + opr.getWidth() * (slot + 0.5f) / 9.0f;
            float radius = baseR * factors[static_cast<size_t>(idx)];
            if (selected) radius *= 1.62f;
            radius = juce::jmin(radius, opr.getHeight() * 0.40f);
            const auto pr = juce::Rectangle<float>(px - radius, cy - radius, radius * 2.0f, radius * 2.0f);
            planetRects[static_cast<size_t>(slot)] = pr;

            // vertical guide
            g.setColour(juce::Colours::white.withAlpha(selected ? 0.30f : 0.10f));
            g.drawLine(px, pr.getBottom(), px, baselineY, 1.0f);
            g.setColour(selected ? juce::Colour(0xcc42d9ff) : juce::Colour(0x668a9ab8));
            g.fillEllipse(px - 1.8f, baselineY - 1.8f, 3.6f, 3.6f);

            if (selected)
            {
                auraUI::drawPlanetGlow(g, pr);
                // crosshair
                g.setColour(juce::Colour(0x6642d9ff));
                g.drawLine(px, opr.getY() + 8.0f, px, cy - radius - 8.0f, 1.0f);
                g.drawLine(px, cy + radius + 8.0f, px, baselineY, 1.0f);
            }

            auraUI::drawPlanet(g, pr, idx);
            g.setColour(selected ? juce::Colour(0xaa42d9ff) : juce::Colour(0x44ffffff));
            g.drawEllipse(pr, selected ? 1.4f : 0.8f);
        }

        // identity label
        {
            g.setColour(juce::Colour(0xffeef4ff));
            g.setFont(auraUI::font(17.0f, true));
            const juce::String name = juce::String(profile.name).toUpperCase();
            const auto nr = juce::Rectangle<int>(op.getX() + 200, op.getBottom() - 62, op.getWidth() - 400, 24);
            auraUI::drawTracked(g, name, nr, 8, 17.0f);
            g.setColour(juce::Colour(0x9942d9ff));
            const float dotY = nr.getCentreY();
            const float tw = 17.0f * (float)name.length() * 1.35f + 8.0f * (float)name.length();
            g.fillEllipse(op.getCentreX() - tw * 0.5f - 16.0f, dotY - 2.0f, 4.0f, 4.0f);
            g.fillEllipse(op.getCentreX() + tw * 0.5f + 12.0f, dotY - 2.0f, 4.0f, 4.0f);

            g.setColour(juce::Colour(0xff7484a8));
            g.setFont(auraUI::font(10.5f));
            g.drawText(profile.descriptor, op.getX() + 200, op.getBottom() - 38, op.getWidth() - 400, 16,
                       juce::Justification::centred);
        }
    }

    //==========================================================================
    // Bottom panels
    {
        drawPanel(g, psPanel, "PLANET SETTINGS", 0);
        drawPanel(g, specPanel, "SPECTRUM", 1);
        drawPanel(g, scPanel, "SOUND CONTROLS", 2);
        drawPanel(g, orbPanel, "ORBITAL REVERB", 3);

        const juce::Colour valueColour = juce::Colour(0xffdce8ff);
        const auto drawSettingRow = [this, &g, valueColour](const juce::Rectangle<int>& row, int icon,
                                                            const juce::String& label, float value)
        {
            auto ir = juce::Rectangle<float>(row.getX() + 24.0f, row.getCentreY() - 19.0f, 38.0f, 38.0f);
            g.setColour(juce::Colour(0x3318263e));
            g.fillEllipse(ir);
            g.setColour(juce::Colour(0x662a3c5e));
            g.drawEllipse(ir, 1.0f);
            drawControlIcon(g, ir.reduced(10.0f), icon);

            g.setColour(juce::Colour(0xff9fb0cc));
            g.setFont(auraUI::font(10.5f, true));
            g.drawText(label.toUpperCase(), row.getX() + 76, row.getCentreY() - 14, 130, 16, juce::Justification::left);
            g.setColour(valueColour);
            g.setFont(auraUI::font(13.5f));
            g.drawText(String::formatted("%.2f", value), row.getX() + 76, row.getCentreY() + 3, 110, 18,
                       juce::Justification::left);
            g.setColour(juce::Colour(0xff55648a));
            g.setFont(auraUI::font(10.5f));
            g.drawText(String::formatted("%.1f", value), row.getRight() - 30, row.getCentreY() - 6, 26, 14,
                       juce::Justification::right);
        };

        const auto rawP = [this](const char* id) { return (float)audioProcessor.parameters.getRawParameterValue(id)->load(); };

        auto rowRect = [this](const juce::Rectangle<int>& panel, int i)
        {
            return juce::Rectangle<int>(panel.getX(), panel.getY() + 48 + i * 62,
                                        panel.getWidth(), 54);
        };
        drawSettingRow(rowRect(psPanel, 0), 4, "Gravity", rawP("gravity"));
        drawSettingRow(rowRect(psPanel, 1), 5, "Atmosphere", rawP("atmosphere"));
        drawSettingRow(rowRect(psPanel, 2), 6, "Temperature", rawP("temperature"));
        drawSettingRow(rowRect(psPanel, 3), 7, "Density", rawP("density"));

        // ---- spectrum visualisation ----
        {
            auto viz = specPanel.withX(specPanel.getX() + 16).withY(specPanel.getY() + 46)
                          .withWidth(specPanel.getWidth() - 32).withHeight(specPanel.getHeight() - 88);
            g.setColour(juce::Colour(0xff040812));
            g.fillRoundedRectangle(viz.toFloat(), 10.0f);
            g.setColour(juce::Colour(0x33416dff));
            g.drawRoundedRectangle(viz.toFloat(), 10.0f, 1.0f);

            const float vx = viz.getX(), vy = viz.getY(), vw = viz.getWidth(), vh = viz.getHeight();
            const float vcx = viz.getCentreX(), vcy = viz.getCentreY();

            // perspective orbit rings
            g.setColour(juce::Colour(0x2642d9ff));
            g.drawEllipse(vcx - vw * 0.42f, vcy - vh * 0.30f, vw * 0.84f, vh * 0.60f, 1.0f);
            g.setColour(juce::Colour(0x1a42d9ff));
            g.drawEllipse(vcx - vw * 0.30f, vcy - vh * 0.42f, vw * 0.60f, vh * 0.84f, 1.0f);

            auto baseline = [vw, vcy, vh, vcx](float x)
            {
                const float t = (x - vcx) / (vw * 0.5f);
                return vcy + t * t * vh * 0.16f;
            };

            if (spectrumView == 0) // SPECTRUM
            {
                const int n = aura::Analyser::numBands;
                const float bw = vw / n;
                const uint32 msNow = juce::Time::getCurrentTime().getMillisecondCounter();
                juce::Random spark((int)(msNow / 120));
                for (int i = 0; i < n; ++i)
                {
                    const float db = specFrame.band[i];
                    const float norm = juce::jlimit(0.0f, 1.0f, (db + 80.0f) / 74.0f);
                    const float x = vx + bw * (i + 0.5f);
                    const float by = baseline(x);
                    const float h = norm * vh * 0.80f;
                    if (h < 1.5f) continue;
                    juce::ColourGradient bar(juce::Colour(0x00a45cff), x, by, juce::Colour(0xff7aeaff), x, by - h, true);
                    bar.addColour(0.4f, juce::Colour(0xb842d9ff));
                    bar.addColour(0.85f, juce::Colour(0xff7aeaff));
                    g.setGradientFill(bar);
                    g.fillRoundedRectangle(juce::Rectangle<float>(x - bw * 0.30f, by - h, bw * 0.60f, h), 1.5f);
                }
                // sparkles
                for (int i = 0; i < 46; ++i)
                {
                    const float x = vx + spark.nextFloat() * vw;
                    const float y = vy + spark.nextFloat() * vh;
                    g.setColour((i % 3 == 0 ? auraUI::violet() : auraUI::cyan())
                                    .withAlpha(0.15f + spark.nextFloat() * 0.30f));
                    g.fillEllipse(x, y, 1.6f, 1.6f);
                }
            }
            else if (spectrumView == 1) // PARTIALS
            {
                g.setColour(juce::Colour(0x331a2438));
                g.drawLine(vx + 8.0f, vcy, vx + vw - 8.0f, vcy, 1.0f);
                for (int i = 0; i < specFrame.partialCount; ++i)
                {
                    const float f = specFrame.pf[i];
                    const float amp = juce::jlimit(0.0f, 1.0f, (specFrame.pa[i] + 80.0f) / 74.0f);
                    const float t = std::log(f / 30.0f) / std::log(20000.0f / 30.0f);
                    const float x = vx + 12.0f + (vw - 24.0f) * t;
                    const float yTop = (viz.getBottom() - 14.0f) - amp * (vh - 34.0f);
                    g.setColour(juce::Colour(0x8842d9ff));
                    g.drawLine(x, viz.getBottom() - 12.0f, x, yTop, 1.3f);
                    g.setColour(juce::Colour(0xffa8eaff));
                    g.fillEllipse(x - 2.6f, yTop - 2.6f, 5.2f, 5.2f);
                }
            }
            else // ENVELOPE
            {
                const int n = (int)envBuf.size();
                juce::Path fill, line;
                for (int i = 0; i < n; ++i)
                {
                    const float x = vx + 8.0f + (vw - 16.0f) * i / (n - 1);
                    const float y = (viz.getBottom() - 12.0f) - juce::jmin(1.0f, envBuf[static_cast<size_t>(i)] * 2.0f) * (vh - 30.0f);
                    if (i == 0) { fill.startNewSubPath(x, y); line.startNewSubPath(x, y); }
                    else { fill.lineTo(x, y); line.lineTo(x, y); }
                }
                fill.lineTo(vx + vw - 8.0f, viz.getBottom() - 12.0f);
                fill.lineTo(vx + 8.0f, viz.getBottom() - 12.0f);
                fill.closeSubPath();
                juce::ColourGradient eg(juce::Colour(0x0042d9ff), viz.getBottomLeft().toFloat(), juce::Colour(0x6642d9ff), viz.getBottomLeft().toFloat() + juce::Point<float>(0.0f, -vh * 0.8f), true);
                g.setGradientFill(eg);
                g.fillPath(fill);
                g.setColour(juce::Colour(0xcc7aeaff));
                g.strokePath(line, juce::PathStrokeType(1.6f));
            }

            // caption
            const auto& names = aura::getPlanetProfiles();
            g.setColour(juce::Colour(0xff55648a));
            g.setFont(auraUI::font(10.0f));
            const juce::String caption =
                String("•  ") + juce::String(names[static_cast<size_t>(planetIdx)].name).toUpperCase() +
                String("  •  SPECTRAL PROFILE  •");
            g.drawText(caption, viz.getX() + 40, viz.getBottom() - 14, viz.getWidth() - 80, 14,
                       juce::Justification::centred);
        }

        // ---- sound controls rows ----
        {
            const auto rawP = [this](const char* id) { return (float)audioProcessor.parameters.getRawParameterValue(id)->load(); };
            struct Row { int icon; const char* id; };
            const Row rows[4] = { { 8, "brightness" }, { 9, "harmonics" }, { 10, "reverb" }, { 11, "width" } };
            for (int i = 0; i < 4; ++i)
            {
                auto row = juce::Rectangle<int>(scPanel.getX(), scPanel.getY() + 48 + i * 62, scPanel.getWidth(), 54);
                auto ir = juce::Rectangle<float>(row.getX() + 20.0f, row.getCentreY() - 16.0f, 32.0f, 32.0f);
                g.setColour(juce::Colour(0x3318263e));
                g.fillEllipse(ir);
                g.setColour(juce::Colour(0x662a3c5e));
                g.drawEllipse(ir, 1.0f);
                drawControlIcon(g, ir.reduced(8.0f), rows[i].icon);

                g.setColour(juce::Colour(0xff9fb0cc));
                g.setFont(auraUI::font(10.5f, true));
                const juce::String label = (juce::String(rows[i].id)).toUpperCase();
                g.drawText(label, row.getX() + 62, row.getCentreY() - 15, 100, 15, juce::Justification::left);
                g.setColour(juce::Colour(0xffdce8ff));
                g.setFont(auraUI::font(12.5f));
                g.drawText(String::formatted("%.2f", rawP(rows[i].id)), row.getX() + 62, row.getCentreY() + 2,
                           100, 16, juce::Justification::left);

                g.setColour(juce::Colour(0xff55648a));
                g.setFont(auraUI::font(10.5f));
                g.drawText(String::formatted("%.2f", rawP(rows[i].id)),
                           row.getRight() - 46, row.getCentreY() - 6, 40, 14,
                           juce::Justification::right);
            }
        }

        // ---- orbital reverb ----
        {
            auto op = orbPanel;
            const float cx = op.getCentreX();
            const float cy = op.getY() + 108.0f;
            const float r = juce::jmin(op.getWidth() * 0.36f, 86.0f);
            const float size01 = (float)audioProcessor.parameters.getRawParameterValue("orbSize")->load();
            const float decayS = (float)audioProcessor.parameters.getRawParameterValue("orbDecay")->load();

            // backdrop
            g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0a1226), cx, cy, juce::Colour(0xff030610), cx + r * 1.3f, cy + r * 1.3f, true));
            g.fillEllipse(cx - r * 1.18f, cy - r * 1.18f, r * 2.36f, r * 2.36f);
            g.setColour(juce::Colour(0x55416dff));
            g.drawEllipse(cx - r * 1.18f, cy - r * 1.18f, r * 2.36f, r * 2.36f, 1.0f);

            // core glow (scales with decay)
            const float glow = 0.5f + juce::jmin(1.0f, decayS / 5.0f) * 0.5f;
            juce::ColourGradient coreGlow(
                juce::Colour(0xffb060ff).withAlpha(0.55f * glow), cx, cy,
                juce::Colour(0x00000000), cx + r * 0.9f, cy + r * 0.9f, true);
            coreGlow.addColour(0.55f, juce::Colour(0xff42d9ff).withAlpha(0.28f * glow));
            g.setGradientFill(coreGlow);
            g.fillEllipse(cx - r * 0.9f, cy - r * 0.9f, r * 1.8f, r * 1.8f);
            g.setGradientFill(juce::ColourGradient(juce::Colour(0xffeaf6ff), cx, cy, juce::Colour(0x00ffffff), cx + r * 0.16f, cy + r * 0.16f, true));
            g.fillEllipse(cx - r * 0.16f, cy - r * 0.16f, r * 0.32f, r * 0.32f);

            // rotating orbit rings
            const double t = juce::Time::getMillisecondCounter() / 1000.0;
            const double speed = 0.25 + size01 * 0.55;
            for (int i = 0; i < 3; ++i)
            {
                const float rr = r * (0.50f + i * 0.24f);
                const float rot = (float)(t * speed * (i % 2 == 0 ? 1.0 : -0.7) + i * 1.1);
                g.saveState();
                g.addTransform(juce::AffineTransform::translation(cx, cy));
                g.addTransform(juce::AffineTransform::rotation(rot));
                g.setColour(auraUI::cyan().withAlpha(0.30f - i * 0.06f));
                g.drawEllipse(-rr, -rr * 0.36f, rr * 2.0f, rr * 0.72f, 1.0f);
                // orbiting dot
                const double ang = t * (0.6 + i * 0.35) * (i % 2 == 0 ? 1.0 : -1.0) + i * 2.2;
                const float dx = (float)std::cos(ang) * rr;
                const float dy = (float)std::sin(ang) * rr * 0.36f;
                g.setColour(juce::Colour(0xffa8eaff));
                g.fillEllipse(dx - 2.4f, dy - 2.4f, 4.8f, 4.8f);
                g.restoreState();
            }

            const auto rawP = [this](const char* id) { return (float)audioProcessor.parameters.getRawParameterValue(id)->load(); };
            const struct { const char* id; const char* label; } orbRows[2] = {
                { "orbSize", "SIZE" }, { "orbDecay", "DECAY" }
            };
            for (int i = 0; i < 2; ++i)
            {
                const int y = op.getY() + 216 + i * 48;
                g.setColour(juce::Colour(0xff9fb0cc));
                g.setFont(auraUI::font(10.5f, true));
                g.drawText(orbRows[i].label, op.getX() + 26, y + 8, 60, 15, juce::Justification::left);
                g.setColour(juce::Colour(0xff55648a));
                g.setFont(auraUI::font(11.0f));
                const juce::String val = juce::String::formatted("%.2f", rawP(orbRows[i].id))
                                         + (i == 1 ? "s" : "");
                g.drawText(val, op.getRight() - 52, y + 8, 46, 15, juce::Justification::right);
            }
        }
    }

    //==========================================================================
    // Keyboard row
    {
        auto kr = keyRow;
        g.setColour(juce::Colour(0xe6070d1c));
        g.fillRoundedRectangle(kr.toFloat().reduced(1.0f), 16.0f);
        g.setColour(juce::Colour(0x55416dff));
        g.drawRoundedRectangle(kr.toFloat().reduced(1.0f), 16.0f, 1.0f);

        // IN
        g.setColour(juce::Colour(0xff7484a8));
        g.setFont(auraUI::font(9.5f, true));
        g.drawText("IN", kr.getX() + 10, kr.getY() + 10, 26, 14, juce::Justification::left);
        drawVUMeter(g, juce::Rectangle<int>(kr.getX() + 10, kr.getY() + 28, 12, kr.getHeight() - 40), inLevel);

        // octave box
        {
            auto ob = juce::Rectangle<int>(kr.getX() + 44, kr.getY() + 22, 118, kr.getHeight() - 44);
            g.setColour(juce::Colour(0xff0a1424));
            g.fillRoundedRectangle(ob.toFloat(), 12.0f);
            g.setColour(juce::Colour(0x55416dff));
            g.drawRoundedRectangle(ob.toFloat(), 12.0f, 1.0f);

            const auto pr = juce::Rectangle<float>(ob.getCentreX() - 9.0f, (float)ob.getY() + 8.0f, 18.0f, 18.0f);
            auraUI::drawPlanet(g, pr, 2);
            g.setColour(juce::Colour(0xff7484a8));
            g.setFont(auraUI::font(9.0f, true));
            g.drawText("OCTAVE", ob.getX() + 16, ob.getY() + 30, ob.getWidth() - 32, 13, juce::Justification::centred);
            g.setColour(juce::Colour(0xffdce8ff));
            g.setFont(auraUI::font(16.0f, true));
            g.drawText(juce::String(octaveShift > 0 ? "+" + juce::String(octaveShift) : juce::String(octaveShift)),
                       ob.getX() + 38, ob.getY() + 50, 42, 22, juce::Justification::centred);
        }

        // OUT
        g.setColour(juce::Colour(0xff7484a8));
        g.setFont(auraUI::font(9.5f, true));
        g.drawText("OUT", kr.getRight() - 36, kr.getY() + 10, 26, 14, juce::Justification::right);
        drawVUMeter(g, juce::Rectangle<int>(kr.getRight() - 24, kr.getY() + 28, 12, kr.getHeight() - 40), outLevel);
    }
}

//==============================================================================
void AURAAudioProcessorEditor::resized()
{
    const int W = getWidth(), H = getHeight();

    topBar = juce::Rectangle<int>(16, 12, W - 32, 74);
    orbitPanel = juce::Rectangle<int>(16, 96, W - 32, (int)(H * 0.354f));
    panelsRow = juce::Rectangle<int>(16, orbitPanel.getBottom() + 14, W - 32, (int)(H * 0.342f));
    keyRow = juce::Rectangle<int>(16, panelsRow.getBottom() + 14, W - 32, H - panelsRow.getBottom() - 30);

    // panels row split
    const int rowW = panelsRow.getWidth();
    const int gap = 14;
    const int psW = (int)(rowW * 0.195f);
    const int specW = (int)(rowW * 0.385f);
    const int scW = (int)(rowW * 0.205f);
    psPanel = juce::Rectangle<int>(panelsRow.getX(), panelsRow.getY(), psW, panelsRow.getHeight());
    specPanel = juce::Rectangle<int>(psPanel.getRight() + gap, panelsRow.getY(), specW, panelsRow.getHeight());
    scPanel = juce::Rectangle<int>(specPanel.getRight() + gap, panelsRow.getY(), scW, panelsRow.getHeight());
    orbPanel = juce::Rectangle<int>(scPanel.getRight() + gap, panelsRow.getY(),
                                    panelsRow.getRight() - scPanel.getRight() - gap, panelsRow.getHeight());

    // top bar components
    const int pillW = 430;
    pillZone = juce::Rectangle<int>(W / 2 - pillW / 2, topBar.getY() + 9, pillW, 56);
    planetLeft.setBounds(pillZone.getX() + 8, pillZone.getY() + 8, 40, 40);
    planetThumb.setBounds(pillZone.getX() + 50, pillZone.getY() + 2, 60, 52);
    planetRight.setBounds(pillZone.getRight() - 48, pillZone.getY() + 8, 40, 40);

    modeZone = juce::Rectangle<int>(W / 2 + pillW / 2 + 26, topBar.getY() + 13, 140, 48);
    modeBtn.setBounds(modeZone);
    sourceZone = juce::Rectangle<int>(W - 372, topBar.getY() + 11, 262, 52);
    sourceBtn.setBounds(sourceZone);
    menuBtn.setBounds(W - 96, topBar.getY() + 13, 48, 48);

    // orbit arrows
    orbitLeft.setBounds(orbitPanel.getX() + 16, orbitPanel.getCentreY() - 18, 34, 36);
    orbitRight.setBounds(orbitPanel.getRight() - 50, orbitPanel.getCentreY() - 18, 34, 36);

    // planet settings knobs (right side of the panel)
    const int ks = 50;
    const auto knobPos = [this, ks](int i)
    {
        const int y = psPanel.getY() + 48 + i * 62 + (54 - ks) / 2;
        return juce::Rectangle<int>(psPanel.getRight() - ks - 34, y, ks, ks);
    };
    gravityKnob.setBounds(knobPos(0));
    atmosphereKnob.setBounds(knobPos(1));
    temperatureKnob.setBounds(knobPos(2));
    densityKnob.setBounds(knobPos(3));

    // spectrum tabs
    tabSpectrum.setBounds(specPanel.getX() + 20, specPanel.getY() + 12, 110, 22);
    tabPartials.setBounds(specPanel.getX() + 138, specPanel.getY() + 12, 110, 22);
    tabEnvelope.setBounds(specPanel.getX() + 256, specPanel.getY() + 12, 110, 22);

    // sound controls: bar + knob rows
    const auto scRow = [this](int i)
    {
        return juce::Rectangle<int>(scPanel.getX(), scPanel.getY() + 48 + i * 62, scPanel.getWidth(), 54);
    };
    const int sk = 42;
    const int knobX = scPanel.getRight() - sk - 62;
    const int barX = scPanel.getX() + 168;
    const int barW = juce::jmax(40, knobX - 14 - barX);
    brightnessBar.setBounds(barX, scRow(0).getCentreY() - 2, barW, 14);
    harmonicsBar.setBounds(barX, scRow(1).getCentreY() - 2, barW, 14);
    reverbBar.setBounds(barX, scRow(2).getCentreY() - 2, barW, 14);
    widthBar.setBounds(barX, scRow(3).getCentreY() - 2, barW, 14);
    brightnessKnob.setBounds(knobX, scRow(0).getCentreY() - sk / 2, sk, sk);
    harmonicsKnob.setBounds(knobX, scRow(1).getCentreY() - sk / 2, sk, sk);
    reverbKnob.setBounds(knobX, scRow(2).getCentreY() - sk / 2, sk, sk);
    widthKnob.setBounds(knobX, scRow(3).getCentreY() - sk / 2, sk, sk);

    // orbital reverb sliders + knobs
    const auto& ob = orbPanel;
    const int orbKnobX = ob.getRight() - sk - 66;
    const int orbBarX = ob.getX() + 88;
    const int orbBarW = juce::jmax(40, orbKnobX - 14 - orbBarX);
    sizeBar.setBounds(orbBarX, ob.getY() + 222, orbBarW, 14);
    decayBar.setBounds(orbBarX, ob.getY() + 270, orbBarW, 14);
    sizeKnob.setBounds(orbKnobX, ob.getY() + 216, sk, sk);
    decayKnob.setBounds(orbKnobX, ob.getY() + 264, sk, sk);

    // keyboard row
    octDown.setBounds(keyRow.getX() + 54, keyRow.getY() + 52, 26, 26);
    octUp.setBounds(keyRow.getX() + 126, keyRow.getY() + 52, 26, 26);
    keyboard.setBounds(keyRow.getX() + 182, keyRow.getY() + 12,
                       keyRow.getWidth() - 182 - 76, keyRow.getHeight() - 24);
}
