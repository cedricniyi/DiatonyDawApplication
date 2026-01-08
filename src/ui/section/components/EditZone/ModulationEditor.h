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

// Forward declaration
class AudioPluginAudioProcessorEditor;

/**
 * Composant d'édition de modulation - Transparent avec encoche verdâtre
 */
class ModulationEditor : public juce::Component, public juce::ValueTree::Listener
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
    juce::Label modulationNameLabel;        // Label pour le nom de la modulation (dans l'encoche)
    ModulationTypeZone modulationTypeZone;  // Zone avec 4 boutons pour le type
    
    // Label pour indiquer quand l'intervalle est géré automatiquement
    // Message dynamique centré (automatique vs manuel)
    juce::Label statusMessageLabel;
    
    // Nouvelles zones pour les accords des sections adjacentes
    SectionChordsZone fromSectionZone;      // 4 derniers accords de la section source
    SectionChordsZone toSectionZone;        // 4 premiers accords de la section destination
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Zones de layout
    juce::Rectangle<int> notchArea;     // Encoche centrée pour le titre
    juce::Rectangle<int> contentArea;   // Zone de contenu sous l'encoche
    
    // Couleur de l'encoche (légèrement verdâtre)
    juce::Colour notchBackgroundColour = juce::Colour(0x66228B22);  // Forest green semi-transparent
    juce::Colour notchBorderColour = juce::Colours::green.withAlpha(0.4f);
    
    void setupModulationNameLabel();
    void setupModulationTypeZone();
    void setupStatusMessageLabel();
    void setupSectionChordsZones();
    void updateContent();
    void syncFromModel();
    void updateIntervalControlsVisibility();  // Affiche/cache les contrôles selon le type
    void drawNotch(juce::Graphics& g);        // Dessine l'encoche centrée
    
    // Callbacks pour les changements
    void onModulationTypeChanged(Diatony::ModulationType newType);
    void onFromChordSelected(int chordIndex);
    void onToChordSelected(int chordIndex);
    
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