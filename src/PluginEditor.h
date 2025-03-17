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
    void handlePlaybackFinished();

private:
    // Utilisation de std::unique_ptr pour une meilleure gestion de la mémoire
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    std::unique_ptr<juce::TextButton> generateButton;
    std::unique_ptr<juce::TextButton> playButton;
    
    std::unique_ptr<juce::Label> generationStatusLabel;  // Label pour l'état de génération
    std::unique_ptr<juce::Label> playbackStatusLabel;    // Label pour l'état de lecture

    std::unique_ptr<juce::ComboBox> tonalityComboBox;
    std::unique_ptr<juce::ComboBox> modeComboBox;     // Nouveau
    std::unique_ptr<juce::Label> tonalityLabel;  // Nouveau label pour le ComboBox

    std::unique_ptr<juce::TextEditor> progressionInput;
    std::unique_ptr<juce::Label> progressionLabel;
    std::unique_ptr<juce::TextEditor> progressionStateInput;  // Nouvel input pour les états
    std::unique_ptr<juce::Label> progressionStateLabel;       // Label correspondant
    std::unique_ptr<juce::Label> modeLabel;
    // État 
    bool midiFileGenerated = false;

    // Callback for button clicks
    void buttonClicked(juce::Button* button) override;

    void timerCallback() override;
    void setupTonalityComboBox();
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
