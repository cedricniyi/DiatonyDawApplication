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

/**
 * Composant d'édition de section - Transparent, remplit ProgressionArea
 * Reçoit l'ID de la section à éditer et affiche des informations/contrôles d'édition
 */
class SectionEditor : public juce::Component, public juce::ValueTree::Listener
{
public:
    SectionEditor();
    ~SectionEditor();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Configure ce composant pour éditer la section avec l'ID donné
     * @param sectionId ID de la section à éditer (ex: "Section_0")
     */
    void setSectionToEdit(const juce::String& sectionId);
    void setSectionState(juce::ValueTree sectionState);
    
    /**
     * Obtient l'ID de la section actuellement éditée
     * @return L'ID de la section ou une chaîne vide si aucune section n'est éditée
     */
    juce::String getCurrentSectionId() const { return currentSectionId; }
    
    /**
     * Vérifie si le composant édite actuellement une section
     * @return true si une section est en cours d'édition
     */
    bool isEditingSection() const { return !currentSectionId.isEmpty(); }
    
    /**
     * Force le rafraîchissement du titre (utile quand l'index change sans que l'ID change)
     */
    void refreshTitle();

    // Accès à la hiérarchie pour trouver AppController
    void parentHierarchyChanged() override;
    void findAppController();
    
private:
    juce::String currentSectionId;
    juce::ValueTree currentSectionState; // ValueTree de la section éditée
    juce::ValueTree currentProgressionState; // ValueTree de la progression (pour écouter les ajouts/suppressions d'accords)
    juce::Label sectionNameLabel;  // Label pour le nom de la section
    
    AppController* appController = nullptr; // Pointeur vers AppController (non-owning)
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Composants des zones de paramètres (style BaseZone)
    KeyZone keyZone;      // Tonalité (BaseZone + KeySelector)
    ModeZone modeZone;    // Mode Majeur/Mineur (BaseZone)
    Zone4 zone4Component; // Accords
    
    // Zones de layout
    juce::Rectangle<int> notchArea;    // Encoche centrée pour le titre
    juce::Rectangle<int> contentArea;
    
    // Zones de contenu (layout FlexBox pour ligne 1)
    juce::Rectangle<int> topRowArea;   // Première ligne (KeyZone + ModeZone)
    juce::Rectangle<int> zone4Area;    // Seconde ligne, toute la largeur
    
    // Proportions pour les lignes et colonnes (valeurs Fr pour Grid)
    int firstRowHeightFr = 40;   // Première ligne : 40 parts
    int secondRowHeightFr = 60;  // Seconde ligne : 60 parts
    int columnFr = 1;            // Chaque colonne : 1 part (égales)
    
    // Propriétés pour bordure sophistiquée (inspiré de OutlineTextButton)
    float borderThickness = 2.0f;
    float cornerRadius = 8.0f;
    juce::Colour borderColour = juce::Colours::darkblue;
    
    void setupSectionNameLabel();
    void updateContent();
    void drawBorder(juce::Graphics& g);
    void drawSeparatorLine(juce::Graphics& g);
    void drawNotch(juce::Graphics& g);  // Dessine l'encoche centrée
    void calculateContentZones();

    // Helpers binding
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