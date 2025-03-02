#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Button::Listener, public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Utilisation de std::unique_ptr pour une meilleure gestion de la mémoire
    std::unique_ptr<juce::TextButton> generateButton;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<juce::Label> statusLabel;

    // État 
    bool midiFileGenerated = false;

    // Callback for button clicks
    void buttonClicked(juce::Button* button) override;

    void timerCallback() override;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
