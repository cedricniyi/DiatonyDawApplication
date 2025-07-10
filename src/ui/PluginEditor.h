#pragma once

#include <JuceHeader.h>
#include "../core/PluginProcessor.h"
#include "../ui_reusable/base/DiatonyLookAndFeel.h"
#include "../ui_reusable/dialogs/SimpleToastComponent.h"
#include "../ui_reusable/dialogs/DiatonyAlertWindow.h"

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // Méthodes appelées par le processeur
    void handlePlaybackFinished();

private:
    // Référence au processeur
    AudioPluginAudioProcessor& audioProcessor;
    
    // Look and feel personnalisé
    DiatonyLookAndFeel diatonyLookAndFeel;
    
    // Composants de notification
    std::unique_ptr<SimpleToastComponent> toast;
    
    // Éléments de base pour commencer
    juce::Label titleLabel;
    juce::TextButton testButton;
    
    // Méthodes utilitaires
    void showToast(const juce::String& message);
    void showAlert(const juce::String& title, const juce::String& message);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 