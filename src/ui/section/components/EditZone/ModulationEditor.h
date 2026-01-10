#pragma once

#include <JuceHeader.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ModulationParameters/ModulationTypeZone.h"
#include "ModulationParameters/SectionChordsZone.h"
#include "utils/FontManager.h"
#include "model/Modulation.h"
#include "model/Section.h"
#include "model/Piece.h"
#include "model/ModelIdentifiers.h"
#include "controller/AppController.h"

class AudioPluginAudioProcessorEditor;

/** @brief Éditeur de modulation avec sélection du type et visualisation des accords adjacents. */
class ModulationEditor : public juce::Component, public juce::ValueTree::Listener
{
public:
    ModulationEditor();
    ~ModulationEditor() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Configure la modulation à éditer via son ID (ex: "Modulation_0"). */
    void setModulationToEdit(const juce::String& modulationId);
    /** @brief Configure le ValueTree de la modulation à éditer. */
    void setModulationState(juce::ValueTree modulationState);
    
    void parentHierarchyChanged() override;
    
    juce::String getCurrentModulationId() const { return currentModulationId; }
    bool isEditingModulation() const { return !currentModulationId.isEmpty(); }

private:
    juce::String currentModulationId;
    juce::ValueTree currentModulationState;
    
    // ValueTrees des sections adjacentes (pour écouter les changements de tonalité/mode)
    juce::ValueTree currentSection1;       // Section source
    juce::ValueTree currentSection2;       // Section destination
    
    // ValueTrees des progressions des sections adjacentes (pour écouter les changements d'accords)
    juce::ValueTree currentProgression1;  // Progression de la section source
    juce::ValueTree currentProgression2;  // Progression de la section destination
    
    AppController* appController = nullptr;
    
    juce::Label modulationNameLabel;
    ModulationTypeZone modulationTypeZone;
    juce::Label statusMessageLabel;
    
    // zones pour les accords des sections adjacentes
    SectionChordsZone fromSectionZone;      // 4 derniers accords de la section source
    SectionChordsZone toSectionZone;        // 4 premiers accords de la section destination
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Rectangle<int> notchArea;
    juce::Rectangle<int> contentArea;
    
    juce::Colour notchBackgroundColour = juce::Colour(0x66228B22);  // Forest green semi-transparent
    juce::Colour notchBorderColour = juce::Colours::green.withAlpha(0.4f);
    
    void setupModulationNameLabel();
    void setupModulationTypeZone();
    void setupStatusMessageLabel();
    void setupSectionChordsZones();
    void updateContent();
    void syncFromModel();
    void updateIntervalControlsVisibility();
    void drawNotch(juce::Graphics& g);
    
    void onModulationTypeChanged(Diatony::ModulationType newType);
    void onFromChordSelected(int chordIndex);
    void onToChordSelected(int chordIndex);
    
    void findAppController();
    void subscribeToAdjacentSectionsAndProgressions();
    void unsubscribeFromSectionsAndProgressions();
    
    // ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationEditor)
};
