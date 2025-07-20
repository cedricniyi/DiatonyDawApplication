#pragma once

#include <JuceHeader.h>

// Forward declarations
class HeaderPanel;
class SectionPanel;  
class FooterPanel;
class AudioPluginAudioProcessor;

// =================================================================================
// Identifiants pour les propriétés ValueTree (seulement ce qui est utilisé)

namespace UIStateIdentifiers
{
    // Nom de l'arbre principal
    const juce::Identifier APP_STATE { "APP_STATE" };
    
    // NETTOYÉ: Seulement les propriétés vraiment utilisées
    const juce::Identifier footerExpanded { "footerExpanded" };
}

/**
 * @brief Contrôleur simple qui gère l'état UI via ValueTree
 * 
 * NOUVEAU: Ne crée plus le ValueTree - reçoit une référence depuis le PluginEditor.
 * Gère seulement l'orchestration des propriétés réellement utilisées :
 * - footerExpanded : état d'expansion du footer
 */
class RootUIController : public juce::ValueTree::Listener
{
public:
    // NOUVEAU: Constructeur prend une référence au ValueTree (créé par PluginEditor)
    explicit RootUIController(AudioPluginAudioProcessor& processor, juce::ValueTree& appStateRef);
    ~RootUIController() override;
    
    void initializePanels(HeaderPanel& header, SectionPanel& section, FooterPanel& footer);
    
    juce::ValueTree& getAppState() { return appState; }
    const juce::ValueTree& getAppState() const { return appState; }
    
    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    
    void valueTreeChildAdded(juce::ValueTree& parentTree,
                             juce::ValueTree& childWhichHasBeenAdded) override;
    
    void valueTreeChildRemoved(juce::ValueTree& parentTree,
                               juce::ValueTree& childWhichHasBeenRemoved,
                               int indexFromWhichChildWasRemoved) override;
    
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                    int oldIndex, int newIndex) override;
    
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;

    // =================================================================================
    // Actions principales (NETTOYÉ: seulement ce qui est utilisé)
    
    void setFooterExpanded(bool expanded);

    // =================================================================================
    // Accesseurs (NETTOYÉ: seulement ce qui est utilisé)
    
    bool isFooterExpanded() const;

private:
    AudioPluginAudioProcessor& audioProcessor;
    juce::ValueTree& appState;  // NOUVEAU: Référence (pas ownership)
    
    HeaderPanel* headerPanel = nullptr;
    SectionPanel* sectionPanel = nullptr;
    FooterPanel* footerPanel = nullptr;
    
    void setupStateListeners();
    void notifyPanelsOfStateChange(const juce::Identifier& property);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootUIController)
}; 