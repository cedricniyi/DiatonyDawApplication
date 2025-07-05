#pragma once

#include <JuceHeader.h>
#include "../core/PluginProcessor.h"
#include "components/StatusPanel.h"
#include "components/SidebarPanel.h"
#include "components/ToastComponent.h"
#include "components/HeaderPanel.h"
#include "components/DiatonyContentPanel.h"
#include "components/HarmonizerContentPanel.h"
#include "LookAndFeel/DiatonyLookAndFeel.h"
#include "../model/DiatonyModel.h"

//==============================================================================
/**
*/
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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    
    // Look and feel personnalisé
    DiatonyLookAndFeel diatonyLookAndFeel;
    
    // Tooltip window
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;
    
    // Panels
    std::unique_ptr<HeaderPanel> headerPanel;
    std::unique_ptr<SidebarPanel> sidebarPanel;
    std::unique_ptr<StatusPanel> statusPanel;
    std::unique_ptr<DiatonyContentPanel> diatonyPanel;
    std::unique_ptr<HarmonizerContentPanel> harmonizerPanel;
    
    // Composant toast pour les notifications temporaires
    std::unique_ptr<ToastComponent> toastComponent;
    
    // Zone du titre
    juce::Rectangle<int> titleBounds;
    
    // Mode actif
    bool isDiatonyMode = true;
    
    bool isSidebarVisible = false;  
    
    // Méthodes de gestion des événements
    void setupPanels();
    void handleGenerateButtonClicked();
    void handlePlayButtonClicked();
    void handleSettingsClicked();
    void handleRefreshSolutions();
    void handleLoadSolution(const SolutionHistoryItem& solution);
    void handleSolutionSelected(const SolutionHistoryItem& solution);
    void handleDiatonyModeClicked();
    void handleHarmonizerModeClicked();
    void updateContentPanelVisibility();
    void toggleSidebar();
    void handleModelChanged(const DiatonyModel& model);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
}; 