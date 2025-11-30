#pragma once

#include <JuceHeader.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ScrollableContentPanel.h"
#include "controller/ContextIdentifiers.h"

// Forward declarations
class ButtonColoredPanel;
class AppController;
class AudioPluginAudioProcessorEditor;

//==============================================================================
/**
 * Zone de contenu d'aperçu gérant l'affichage de progressions avec gestion d'état vide
 * Architecture réactive : écoute les changements du modèle via ValueTree::Listener
 */
class OverviewContentArea : public ColoredPanel, public juce::ValueTree::Listener
{
public:
    OverviewContentArea();
    ~OverviewContentArea() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === DÉCOUVERTE DE SERVICE ===
    /** 
     * Appelé automatiquement par JUCE quand la hiérarchie des composants change
     * Utilisé pour découvrir AppController via findParentComponentOfClass
     */
    void parentHierarchyChanged() override;
    
    // === ARCHITECTURE RÉACTIVE ===
    /** 
     * Connecte ce composant au ValueTree du modèle pour écouter les changements
     * @param modelState Le ValueTree racine du modèle (Piece.getState())
     */
    void setModelState(juce::ValueTree& modelState);
    
    // Gestion du contenu
    void addSmallPanel();
    void clearAllPanels();
    bool hasContent() const;
    
    // Configuration
    juce::Rectangle<int> getPreferredSize() const;
    
    // Gestion de la sélection (maintenant pilotée par l'état central)
    
    // =================================================================================
    // ValueTree::Listener interface - Réactivité aux changements du modèle
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, 
                               int indexFromWhichChildWasRemoved) override;
    void valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, 
                                    int oldIndex, int newIndex) override;
    void valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged) override;

private:
    // Composants UI
    juce::Viewport viewport;
    std::unique_ptr<ScrollableContentPanel> scrollableContent;
    juce::Label emptyLabel;
    
    // === DÉCOUVERTE DE SERVICE ===
    AppController* appController = nullptr;  // Trouvé via parentHierarchyChanged()
    
    // === ARCHITECTURE RÉACTIVE ===
    juce::ValueTree modelState;     // ValueTree du modèle écouté via ValueTree::Listener
    juce::ValueTree selectionState; // ValueTree de l'état de sélection
    
    // Configuration
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 35;
    static constexpr int CONTENT_MARGIN = 5;
    
    // Gestion des IDs de panels
    int nextPanelId = 1;
    
    // Gestion de la sélection supprimée - maintenant via selectionState
    
    // Méthodes privées
    void setupViewport();
    void setupEmptyLabel();
    void updateVisibility();
    void onPanelClicked(ButtonColoredPanel* clickedPanel);
    void findAppController();  // Méthode pour découvrir AppController
    
    // Méthodes pour l'architecture réactive
    void refreshFromModel();   // Met à jour l'UI depuis le modèle
    void handleSectionAdded(const juce::ValueTree& sectionNode);
    void handleSectionRemoved();
    void createPanelForSection(const juce::ValueTree& sectionNode, int sectionIndex, bool autoSelect);  // Crée un panel visuel pour une section
    void createPanelForModulation(const juce::ValueTree& modulationNode);  // Crée un panel visuel pour une modulation
    void updateSelectionHighlight(); // Met à jour l'aspect visuel des panels selon la sélection centrale
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewContentArea)
}; 