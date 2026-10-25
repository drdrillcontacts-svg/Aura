// Editor factory - kept in its own translation unit so the audio engine can
// also be compiled for the headless self-test console app (no GUI modules).
#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorEditor* AURAAudioProcessor::createEditor()
{
    return new AURAAudioProcessorEditor(*this);
}
