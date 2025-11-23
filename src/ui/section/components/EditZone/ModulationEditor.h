#pragma once

#include <JuceHeader.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Component/ComboBox/DiatonyComboBox.h"
#include "utils/FontManager.h"
#include "model/Modulation.h"
#include "model/Section.h"
#include "model/Piece.h"
#include "model/ModelIdentifiers.h"
#include "controller/AppController.h"

// Forward declaration
class AudioPluginAudioProcessorEditor;

/**
 * Composant d'édition de modulation - Affiche les informations d'une modulation
 * et de ses sections adjacentes depuis le ValueTree
 */
class ModulationEditor : public ColoredPanel, public juce::ValueTree::Listener
{
public:
    ModulationEditor();
    ~ModulationEditor() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Configure ce composant pour éditer la modulation avec l'ID donné
     * @param modulationId ID de la modulation à éditer (ex: "Modulation_0")
     */
    void setModulationToEdit(const juce::String& modulationId);
    
    /**
     * Configure le ValueTree de la modulation à éditer
     * @param modulationState ValueTree de la modulation
     */
    void setModulationState(juce::ValueTree modulationState);
    
    /**
     * Découverte de service - appelé automatiquement par JUCE
     */
    void parentHierarchyChanged() override;
    
    /**
     * Obtient l'ID de la modulation actuellement éditée
     * @return L'ID de la modulation ou une chaîne vide si aucune modulation n'est éditée
     */
    juce::String getCurrentModulationId() const { return currentModulationId; }
    
    /**
     * Vérifie si le composant édite actuellement une modulation
     * @return true si une modulation est en cours d'édition
     */
    bool isEditingModulation() const { return !currentModulationId.isEmpty(); }

private:
    juce::String currentModulationId;
    juce::ValueTree currentModulationState;  // ValueTree de la modulation éditée
    
    // ValueTrees des sections adjacentes (pour écouter les changements de tonalité/mode)
    juce::ValueTree currentSection1;       // Section source
    juce::ValueTree currentSection2;       // Section destination
    
    // ValueTrees des progressions des sections adjacentes (pour écouter les changements d'accords)
    juce::ValueTree currentProgression1;  // Progression de la section source
    juce::ValueTree currentProgression2;  // Progression de la section destination
    
    // Référence à AppController et Piece pour accéder aux sections adjacentes
    AppController* appController = nullptr;
    
    // Labels pour afficher les informations
    juce::Label modulationNameLabel;        // Label pour le nom de la modulation
    juce::Label modulationTypeLabel;        // Label "Type :"
    DiatonyComboBox modulationTypeComboBox; // ComboBox pour sélectionner le type
    juce::Label fromSectionLabel;           // Section source
    juce::Label toSectionLabel;             // Section destination
    juce::Label chordIndicesLabel;          // Indices d'accords
    
    // ComboBoxes listant les accords des sections adjacentes
    DiatonyComboBox fromChordComboBox;      // ComboBox avec accords de la section source
    DiatonyComboBox toChordComboBox;        // ComboBox avec accords de la section destination
    
    // Label pour indiquer quand l'intervalle est géré automatiquement
    juce::Label autoIntervalLabel;          // "Transition gérée automatiquement par le solveur"
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Zones de layout
    juce::Rectangle<int> headerArea;
    juce::Rectangle<int> contentArea;
    
    // Propriétés pour bordure sophistiquée (inspiré de OutlineTextButton)
    float borderThickness = 2.0f;
    float cornerRadius = 8.0f;
    juce::Colour borderColour = juce::Colours::darkgreen;
    
    // Valeurs validées depuis DiatonyTypes.h (comme Zone4ContentArea)
    // Les 4 types de modulation disponibles
    static constexpr std::array<Diatony::ModulationType, 4> modulationTypes = {
        Diatony::ModulationType::PerfectCadence,
        Diatony::ModulationType::PivotChord,
        Diatony::ModulationType::Alteration,
        Diatony::ModulationType::Chromatic
    };
    
    void setupModulationNameLabel();
    void setupInfoLabels();
    void setupModulationTypeComboBox();
    void setupChordSelectionComboBoxes();
    void setupAutoIntervalLabel();
    void updateContent();
    void syncFromModel();
    void updateIntervalControlsVisibility();  // Affiche/cache les contrôles selon le type
    void drawBorder(juce::Graphics& g);
    void drawSeparatorLine(juce::Graphics& g);
    
    // Callbacks pour les changements
    void onModulationTypeChanged();
    void onFromChordChanged();
    void onToChordChanged();
    
    // Helper pour peupler les ComboBoxes d'accords
    void populateChordComboBoxes();
    
    // Helper pour formater les accords d'une section
    juce::String formatSectionChords(const Section& section) const;
    
    // Helpers
    void findAppController();
    void subscribeToAdjacentSectionsAndProgressions();  // S'abonner aux sections ET progressions adjacentes
    void unsubscribeFromSectionsAndProgressions();      // Désabonner des sections et progressions
    
    // ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int index) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationEditor)
}; 