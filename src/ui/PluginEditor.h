#pragma once

#include "../core/PluginProcessor.h"
#include "components/TonalityPanel.h"
#include "components/ProgressionPanel.h"
#include "components/GenerationPanel.h"
#include "components/StatusPanel.h"
#include "components/SidebarPanel.h"
#include "components/ToastComponent.h"
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
    std::unique_ptr<SidebarPanel> sidebarPanel;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<TonalityPanel> tonalityPanel;
    std::unique_ptr<ProgressionPanel> progressionPanel;
    std::unique_ptr<GenerationPanel> generationPanel;
    
    // Composant toast pour les notifications temporaires
    std::unique_ptr<ToastComponent> toastComponent;
    
    // Zone du titre
    juce::Rectangle<int> titleBounds;
    
    // Progression courante
    Progression currentProgression;
    
    // Méthodes de gestion des événements
    void setupPanels();
    void handleGenerateButtonClicked();
    void handlePlayButtonClicked();
    
    // Méthodes pour gérer l'historique des solutions
    void handleRefreshSolutions();
    void handleLoadSolution(const SolutionHistoryItem& solution);
    void handleSolutionSelected(const SolutionHistoryItem& solution);
    
    // Méthodes de la sidebar d'origine (pour compatibilité)
    void handleAboutClicked();
    void handleSettingsClicked();
    void handleHelpClicked();
    
    // Référence au processeur audio
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 