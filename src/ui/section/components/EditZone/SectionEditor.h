#pragma once

#include <JuceHeader.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "utils/FontManager.h"
#include "Parameters/KeyZone.h"
#include "Parameters/ModeZone.h"
#include "Parameters/Zone4.h"
#include "model/Section.h"
#include "model/NoteConverter.h"
#include "controller/AppController.h"

/** @brief Éditeur de section affichant KeyZone, ModeZone et Zone4 (accords). */
class SectionEditor : public juce::Component, public juce::ValueTree::Listener
{
public:
    SectionEditor();
    ~SectionEditor();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Configure la section à éditer via son ID (ex: "Section_0"). */
    void setSectionToEdit(const juce::String& sectionId);
    /** @brief Configure le ValueTree de la section à éditer. */
    void setSectionState(juce::ValueTree sectionState);
    
    juce::String getCurrentSectionId() const { return currentSectionId; }
    bool isEditingSection() const { return !currentSectionId.isEmpty(); }
    
    /** @brief Force le rafraîchissement du titre (utile quand l'index change). */
    void refreshTitle();

    void parentHierarchyChanged() override;
    void findAppController();
    
private:
    juce::String currentSectionId;
    juce::ValueTree currentSectionState;        // ValueTree de la section éditée
    juce::ValueTree currentProgressionState;    // ValueTree de la progression (pour écouter les ajouts/suppressions d'accords)
    juce::Label sectionNameLabel;               // Label pour le titre de la progression
    
    AppController* appController = nullptr;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Composants des zones de paramètres (style BaseZone)
    KeyZone keyZone;      // Tonalité (BaseZone + KeySelector)
    ModeZone modeZone;    // Mode Majeur/Mineur (BaseZone)
    Zone4 zone4Component; // Accords
    
    // Zones de layout
    juce::Rectangle<int> notchArea;     // Encoche centrée pour le titre
    juce::Rectangle<int> contentArea;
    juce::Rectangle<int> topRowArea;    // Première ligne (KeyZone + ModeZone)
    juce::Rectangle<int> zone4Area;     // Seconde ligne, toute la largeur
    
    int firstRowHeightFr = 40;
    int secondRowHeightFr = 60;
    int columnFr = 1;
    
    float borderThickness = 2.0f;
    float cornerRadius = 8.0f;
    juce::Colour borderColour = juce::Colours::darkblue;
    
    void setupSectionNameLabel();
    void updateContent();
    void drawBorder(juce::Graphics& g);
    void drawSeparatorLine(juce::Graphics& g);
    void drawNotch(juce::Graphics& g);  // Dessine l'encoche centrée
    void calculateContentZones();

    void bindZonesToModel();
    void syncZonesFromModel();

    // ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionEditor)
};
