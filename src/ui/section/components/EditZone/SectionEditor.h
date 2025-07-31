#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "utils/FontManager.h"
#include "Parameters/Zone1.h"
#include "Parameters/Zone2.h"
#include "Parameters/Zone3.h"
#include "Parameters/Zone4.h"

/**
 * Composant d'édition de section - Placeholder pour l'édition détaillée d'une section
 * Reçoit l'ID de la section à éditer et affiche des informations/contrôles d'édition
 */
class SectionEditor : public ColoredPanel
{
public:
    SectionEditor();
    ~SectionEditor() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Configure ce composant pour éditer la section avec l'ID donné
     * @param sectionId ID de la section à éditer (ex: "Section_0")
     */
    void setSectionToEdit(const juce::String& sectionId);
    
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

private:
    juce::String currentSectionId;
    juce::Label sectionNameLabel;  // Label pour le nom de la section
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Composants des 4 zones de paramètres
    Zone1 zone1Component;
    Zone2 zone2Component;
    Zone3 zone3Component;
    Zone4 zone4Component;
    
    // Zones de layout
    juce::Rectangle<int> headerArea;
    juce::Rectangle<int> contentArea;
    
    // Zones de contenu (4 zones au total)
    juce::Rectangle<int> zone1Area;  // Première ligne, première colonne
    juce::Rectangle<int> zone2Area;  // Première ligne, deuxième colonne  
    juce::Rectangle<int> zone3Area;  // Première ligne, troisième colonne
    juce::Rectangle<int> zone4Area;  // Seconde ligne, toute la largeur
    
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
    void calculateContentZones();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionEditor)
}; 