#pragma once

#include <JuceHeader.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "controller/ContextIdentifiers.h"

class ButtonColoredPanel;
class AppController;
class AudioPluginAudioProcessorEditor;

/**
 * @brief Zone de contenu d'aperçu gérant l'affichage des progressions.
 *
 * Architecture réactive : écoute les changements du modèle via ValueTree::Listener.
 */
class OverviewContentArea : public juce::Component, public juce::ValueTree::Listener
{
public:
    OverviewContentArea();
    ~OverviewContentArea() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Découvre AppController via la hiérarchie des composants. */
    void parentHierarchyChanged() override;
    
    /** @brief Connecte ce composant au ValueTree du modèle. */
    void setModelState(juce::ValueTree& modelState);
    
    void addSmallPanel();
    void clearAllPanels();
    bool hasContent() const;
    juce::Rectangle<int> getPreferredSize() const;
    
    // ValueTree::Listener
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
    
    AppController* appController = nullptr;
    juce::ValueTree modelState;             // ValueTree du modèle écouté via ValueTree::Listener
    juce::ValueTree selectionState;         // ValueTree de l'état de sélection
    
    static constexpr float cornerRadius = 8.0f;
    
    // Configuration - Dimensions pour le layout superposé
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 45;  // Plus haut pour les modulations
    static constexpr int CONTENT_MARGIN = 5;
    static constexpr int SECTION_WIDTH = 60;
    static constexpr int SECTION_HEIGHT = 25;
    static constexpr int MODULATION_WIDTH = 24;
    static constexpr int MODULATION_HEIGHT = 35;
    
    int nextPanelId = 1;
    
    void setupViewport();
    void setupEmptyLabel();
    void updateVisibility();
    void onPanelClicked(ButtonColoredPanel* clickedPanel);
    void findAppController();
    
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
