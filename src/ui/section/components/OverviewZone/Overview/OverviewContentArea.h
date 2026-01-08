#pragma once

#include <JuceHeader.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "controller/ContextIdentifiers.h"

// Forward declarations
class ButtonColoredPanel;
class AppController;
class AudioPluginAudioProcessorEditor;

//==============================================================================
/**
 * Zone de contenu d'aperçu gérant l'affichage de progressions avec gestion d'état vide
 * Architecture réactive : écoute les changements du modèle via ValueTree::Listener
 * Dessin custom (fond noir arrondi)
 */
class OverviewContentArea : public juce::Component, public juce::ValueTree::Listener
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
    std::unique_ptr<juce::Component> overlayContainer;  // Conteneur pour layout superposé
    juce::Label emptyLabel;
    
    // Stockage séparé des panels pour le layout superposé
    std::vector<std::unique_ptr<ButtonColoredPanel>> sectionPanels;
    std::vector<std::unique_ptr<ButtonColoredPanel>> modulationPanels;
    
    // === DÉCOUVERTE DE SERVICE ===
    AppController* appController = nullptr;  // Trouvé via parentHierarchyChanged()
    
    // === ARCHITECTURE RÉACTIVE ===
    juce::ValueTree modelState;     // ValueTree du modèle écouté via ValueTree::Listener
    juce::ValueTree selectionState; // ValueTree de l'état de sélection
    
    // Style constants
    static constexpr float cornerRadius = 8.0f;
    
    // Configuration - Dimensions pour le layout superposé
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 45;  // Plus haut pour les modulations
    static constexpr int CONTENT_MARGIN = 5;
    static constexpr int SECTION_WIDTH = 60;
    static constexpr int SECTION_HEIGHT = 25;
    static constexpr int MODULATION_WIDTH = 24;
    static constexpr int MODULATION_HEIGHT = 35;
    
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
    void layoutPanels();  // Positionne les panels avec modulations superposées aux jonctions
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewContentArea)
}; 