#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AURAAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                       private juce::FileDragAndDropTarget,
                                       private juce::Timer
{
public:
    explicit AURAAudioProcessorEditor(AURAAudioProcessor&);
    ~AURAAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    AURAAudioProcessor& audioProcessor;
    juce::TextButton planetLeft { "<" }, planetRight { ">" }, loadButton { "LOAD ONE-SHOT" };
    juce::ComboBox planetBox, modeBox;
    juce::Label titleLabel, subtitleLabel, sampleLabel, statusLabel;
    juce::Slider brightness, body, space, gravity, atmosphere, temperature, width, reverb, decay;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> brightnessAttachment, bodyAttachment,
        spaceAttachment, gravityAttachment, atmosphereAttachment, temperatureAttachment,
        widthAttachment, reverbAttachment, decayAttachment;
    bool isDragOver = false;
    int planetIndex = 2;
    std::vector<juce::String> planetNames { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto" };

    bool isInterestedInFileDrag(const juce::StringArray&) override;
    void filesDropped(const juce::StringArray&, int, int) override;
    void timerCallback() override;
    void styleSlider(juce::Slider&);
    void addSlider(juce::Slider&, const juce::String&, juce::Component& parent);
    void loadSampleFromChooser();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AURAAudioProcessorEditor)
};
