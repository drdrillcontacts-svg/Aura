#include "PluginEditor.h"

static juce::Colour bg(0xff050a14), panel(0xe90a1425), cyan(0xff42d9ff), blue(0xff416dff), violet(0xffa45cff);

AURAAudioProcessorEditor::AURAAudioProcessorEditor(AURAAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1240, 760);
    setResizable(true, true);
    setResizeLimits(980, 650, 1700, 1050);

    titleLabel.setText("A U R A", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(30.0f, juce::Font::plain)));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("PLANETS  •  SOUNDS  •  BEYOND", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(juce::FontOptions(9.5f)));
    subtitleLabel.setColour(juce::Label::textColourId, cyan);
    addAndMakeVisible(subtitleLabel);

    planetBox.addItemList(juce::StringArray(planetNames.data(), (int)planetNames.size()), 1);
    planetBox.setSelectedId(3);
    planetBox.setColour(juce::ComboBox::backgroundColourId, panel);
    planetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    planetBox.setColour(juce::ComboBox::outlineColourId, blue.withAlpha(0.7f));
    addAndMakeVisible(planetBox);
    planetBox.onChange = [this] { planetIndex = planetBox.getSelectedId() - 1; repaint(); };

    modeBox.addItem("REALISTIC", 1);
    modeBox.addItem("CINEMATIC", 2);
    modeBox.addItem("ALIEN", 3);
    modeBox.setSelectedId(1);
    modeBox.setColour(juce::ComboBox::backgroundColourId, panel);
    modeBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    addAndMakeVisible(modeBox);

    planetLeft.onClick = [this] { planetIndex = (planetIndex + (int)planetNames.size() - 1) % (int)planetNames.size(); planetBox.setSelectedId(planetIndex + 1); };
    planetRight.onClick = [this] { planetIndex = (planetIndex + 1) % (int)planetNames.size(); planetBox.setSelectedId(planetIndex + 1); };
    for (auto* b : { &planetLeft, &planetRight, &loadButton })
    {
        b->setColour(juce::TextButton::buttonColourId, panel);
        b->setColour(juce::TextButton::textColourOffId, cyan);
        b->setColour(juce::TextButton::buttonOnColourId, blue);
        addAndMakeVisible(*b);
    }
    loadButton.onClick = [this] { loadSampleFromChooser(); };

    sampleLabel.setText(audioProcessor.getSampleName(), juce::dontSendNotification);
    sampleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sampleLabel.setFont(juce::Font(juce::FontOptions(14.0f)));
    addAndMakeVisible(sampleLabel);

    statusLabel.setText("DRAG & DROP A WAV / AIFF / FLAC / OGG ONE-SHOT", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, cyan);
    statusLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(statusLabel);

    auto setup = [this](juce::Slider& s, const juce::String& name, juce::Component& parent)
    {
        styleSlider(s);
        parent.addAndMakeVisible(s);
        s.setName(name);
    };
    setup(brightness, "Brightness", *this); setup(body, "Body", *this); setup(space, "Orbital Space", *this);
    setup(gravity, "Gravity", *this); setup(atmosphere, "Atmosphere", *this); setup(temperature, "Temperature", *this);
    setup(width, "Width", *this); setup(reverb, "Reverb", *this); setup(decay, "Decay", *this);

    auto& apvts = audioProcessor.parameters;
    brightnessAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "brightness", brightness);
    bodyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "body", body);
    spaceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "space", space);
    gravityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "gravity", gravity);
    atmosphereAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "atmosphere", atmosphere);
    temperatureAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "temperature", temperature);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "width", width);
    reverbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "reverb", reverb);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "decay", decay);
    startTimerHz(4);
}

void AURAAudioProcessorEditor::styleSlider(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, cyan);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, 0xff1d2b48);
    s.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxBackgroundColourId, 0xff0b1324);
}

void AURAAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient gradient(0xff101d39, 0, 0, bg, (float)getWidth(), (float)getHeight(), false);
    g.setGradientFill(gradient); g.fillAll();

    // Starfield
    juce::Random rng(2026);
    for (int i = 0; i < 220; ++i)
    {
        float x = rng.nextFloat() * getWidth(), y = rng.nextFloat() * getHeight();
        float r = rng.nextFloat() * 1.6f + 0.3f;
        g.setColour(juce::Colours::white.withAlpha(rng.nextFloat() * 0.5f + 0.08f));
        g.fillEllipse(x, y, r, r);
    }

    auto header = juce::Rectangle<float>(18, 14, getWidth()-36.0f, 68);
    g.setColour(0xff050914); g.fillRoundedRectangle(header, 18);
    g.setColour(blue.withAlpha(0.6f)); g.drawRoundedRectangle(header, 18, 1.0f);

    auto orbit = juce::Rectangle<float>(18, 92, getWidth()-36.0f, getHeight()*0.35f);
    g.setColour(0xff030814); g.fillRoundedRectangle(orbit, 22);
    g.setColour(cyan.withAlpha(0.17f)); g.drawRoundedRectangle(orbit, 22, 1.0f);
    auto cx = orbit.getCentreX(), cy = orbit.getCentreY();
    for (int i = 0; i < 7; ++i)
    {
        g.setColour(juce::Colours::white.withAlpha(0.16f + i*0.012f));
        g.drawEllipse(cx-(120+i*53), cy-(24+i*17), (120+i*53)*2, (24+i*17)*2, 1.0f);
    }

    const juce::Colour planetColours[] = { 0xffa6a8b0, 0xffdb8a62, 0xff398cff, 0xffdb624b, 0xffd8b16c, 0xffd9c8a5, 0xff53d8e8, 0xff4264ff, 0xffa05cff };
    const float sizes[] = { 22, 29, 70, 33, 42, 39, 35, 31, 25 };
    for (int i = 0; i < 9; ++i)
    {
        float x = cx + (i-4)*((getWidth()-150.0f)/9.0f);
        float y = cy + std::sin((float)i*1.8f)*22.0f;
        float d = sizes[i] * (i == planetIndex ? 1.25f : 1.0f);
        g.setColour(planetColours[i].withAlpha(i == planetIndex ? 0.24f : 0.12f));
        g.fillEllipse(x-d*0.75f, y-d*0.75f, d*1.5f, d*1.5f);
        g.setColour(planetColours[i].withAlpha(i == planetIndex ? 0.95f : 0.7f));
        g.fillEllipse(x-d/2, y-d/2, d, d);
        g.setColour(juce::Colours::white.withAlpha(0.45f));
        g.drawEllipse(x-d/2, y-d/2, d, d, 0.8f);
        if (i == planetIndex)
        {
            g.setColour(cyan.withAlpha(0.75f));
            g.drawEllipse(x-d*0.72f, y-d*0.72f, d*1.44f, d*1.44f, 1.5f);
        }
    }
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    g.drawFittedText(planetNames[(size_t)planetIndex].toUpperCase(), orbit.toNearestInt().withY((int)(orbit.getBottom()-39)).withHeight(25), juce::Justification::centred, 1);
    g.setColour(cyan.withAlpha(0.75f));
    g.setFont(juce::Font(juce::FontOptions(9.5f)));
    g.drawFittedText("PLANETARY TIMBRE ENGINE  •  SELECT A WORLD TO SHAPE YOUR SAMPLE", orbit.toNearestInt().withY((int)(orbit.getBottom()-18)).withHeight(15), juce::Justification::centred, 1);

    const int lowerY = (int)(getHeight()*0.49f), lowerH = (int)(getHeight()*0.36f);
    auto panelRect = [&](int x, int w) {
        juce::Rectangle<float> r((float)x, (float)lowerY, (float)w, (float)lowerH);
        g.setColour(panel); g.fillRoundedRectangle(r, 15);
        g.setColour(blue.withAlpha(0.42f)); g.drawRoundedRectangle(r, 15, 1.0f);
    };
    panelRect(20, (int)(getWidth()*0.25f));
    panelRect((int)(getWidth()*0.27f), (int)(getWidth()*0.34f));
    panelRect((int)(getWidth()*0.62f), (int)(getWidth()*0.19f));
    panelRect((int)(getWidth()*0.82f), getWidth()-(int)(getWidth()*0.82f)-20);

    g.setColour(cyan); g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    g.drawText("PLANET SETTINGS", 38, lowerY+13, 200, 18, juce::Justification::left);
    g.drawText("SAMPLE TRANSFORM", (int)(getWidth()*0.29f), lowerY+13, 220, 18, juce::Justification::left);
    g.drawText("SOUND CONTROLS", (int)(getWidth()*0.64f), lowerY+13, 180, 18, juce::Justification::left);
    g.drawText("ORBITAL SPACE", (int)(getWidth()*0.84f), lowerY+13, 150, 18, juce::Justification::left);

    // Animated-looking spectral waveform
    auto spec = juce::Rectangle<float>((float)(getWidth()*0.29f), (float)(lowerY+45), (float)(getWidth()*0.30f), (float)(lowerH-58));
    g.setColour(0xff070d1c); g.fillRoundedRectangle(spec, 9);
    juce::Path wave;
    for (int i=0; i<150; ++i)
    {
        float x = spec.getX() + (float)i/149.0f*spec.getWidth();
        float amp = (0.15f + 0.7f*std::abs(std::sin(i*0.19f))*std::exp(-std::abs(i-74)/38.0f));
        float y = spec.getCentreY() - amp*spec.getHeight()*0.43f;
        if (i==0) wave.startNewSubPath(x,y); else wave.lineTo(x,y);
    }
    g.setColour(cyan.withAlpha(0.85f)); g.strokePath(wave, juce::PathStrokeType(1.7f));
    for (int i=0; i<60; ++i)
    {
        float x=spec.getX()+rng.nextFloat()*spec.getWidth();
        float y=spec.getY()+rng.nextFloat()*spec.getHeight();
        g.setColour((i%3==0?violet:cyan).withAlpha(0.25f));
        g.fillEllipse(x,y,2.0f,2.0f);
    }

    auto drop = juce::Rectangle<float>(28, getHeight()-106.0f, getWidth()-56.0f, 78.0f);
    g.setColour(isDragOver ? 0xff102b49 : 0xff070d1b); g.fillRoundedRectangle(drop, 12);
    g.setColour(isDragOver ? cyan : blue.withAlpha(0.65f));
    g.drawRoundedRectangle(drop, 12, 1.5f);
    g.setColour(cyan.withAlpha(0.9f));
    g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    g.drawFittedText("DROP ONE-SHOT HERE  →  PLAY IT AS A PLANETARY INSTRUMENT", drop.toNearestInt().withY((int)drop.getY()+10).withHeight(20), juce::Justification::centred, 1);
    g.setColour(juce::Colours::white.withAlpha(0.72f));
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawFittedText("WAV • AIFF • FLAC • OGG    |    MIDI PLAYABLE • POLYPHONIC • ONE-SHOT", drop.toNearestInt().withY((int)drop.getY()+34).withHeight(18), juce::Justification::centred, 1);
}

void AURAAudioProcessorEditor::resized()
{
    auto w = getWidth();
    titleLabel.setBounds(40, 17, 210, 38);
    subtitleLabel.setBounds(44, 53, 240, 18);
    planetLeft.setBounds(w/2-185, 25, 35, 30);
    planetBox.setBounds(w/2-145, 24, 190, 32);
    planetRight.setBounds(w/2+49, 25, 35, 30);
    modeBox.setBounds(w/2+105, 25, 125, 30);
    loadButton.setBounds(w-220, 25, 170, 30);
    sampleLabel.setBounds(36, 95, 350, 24);
    statusLabel.setBounds(36, 119, 500, 20);

    const int y = (int)(getHeight()*0.55f);
    gravity.setBounds(35, y, 70, 78);
    atmosphere.setBounds(115, y, 70, 78);
    temperature.setBounds(195, y, 70, 78);
    brightness.setBounds((int)(w*0.64f), y, 72, 78);
    body.setBounds((int)(w*0.70f), y, 72, 78);
    space.setBounds((int)(w*0.76f), y, 72, 78);
    width.setBounds((int)(w*0.82f), y, 65, 78);
    reverb.setBounds((int)(w*0.87f), y, 65, 78);
    decay.setBounds((int)(w*0.92f), y, 65, 78);
}

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
        if (audioProcessor.loadSample(juce::File(files[0])))
            statusLabel.setText("SAMPLE LOADED • PLAY MIDI NOTES", juce::dontSendNotification);
        else
            statusLabel.setText("COULD NOT LOAD FILE — TRY WAV OR AIFF", juce::dontSendNotification);
        sampleLabel.setText(audioProcessor.getSampleName(), juce::dontSendNotification);
    }
    repaint();
}

void AURAAudioProcessorEditor::timerCallback()
{
    sampleLabel.setText(audioProcessor.getSampleName(), juce::dontSendNotification);
    if (audioProcessor.hasSample() && statusLabel.getText().startsWith("DRAG"))
        statusLabel.setText("SAMPLE READY • PLAY MIDI NOTES", juce::dontSendNotification);
}

void AURAAudioProcessorEditor::loadSampleFromChooser()
{
    auto chooser = std::make_shared<juce::FileChooser>("Choose a one-shot sample", juce::File{}, "*.wav;*.aiff;*.aif;*.flac;*.ogg");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (file.existsAsFile() && audioProcessor.loadSample(file))
            {
                sampleLabel.setText(audioProcessor.getSampleName(), juce::dontSendNotification);
                statusLabel.setText("SAMPLE LOADED • PLAY MIDI NOTES", juce::dontSendNotification);
            }
        });
}
