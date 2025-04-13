#pragma once

#include "../core/PluginProcessor.h"
#include "components/TonalityPanel.h"
#include "components/ProgressionPanel.h"
#include "components/GenerationPanel.h"
#include "components/StatusPanel.h"
#include "LookAndFeel/DiatonyLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void handlePlaybackFinished();

private:
    // Look and feel personnalisé
    DiatonyLookAndFeel diatonyLookAndFeel;
    
    // Tooltip window
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;
    
    // Panels
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<TonalityPanel> tonalityPanel;
    std::unique_ptr<ProgressionPanel> progressionPanel;
    std::unique_ptr<GenerationPanel> generationPanel;
    
    // Zone du titre
    juce::Rectangle<int> titleBounds;
    
    // Progression courante
    Progression currentProgression;
    
    // Méthodes de gestion des événements
    void setupPanels();
    void handleGenerateButtonClicked();
    void handlePlayButtonClicked();
    
    // Référence au processeur audio
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 