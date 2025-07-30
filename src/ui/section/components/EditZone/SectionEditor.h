#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "utils/FontManager.h"

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
    
    // Zones de layout
    juce::Rectangle<int> headerArea;
    juce::Rectangle<int> contentArea;
    
    // Propriétés pour bordure sophistiquée (inspiré de OutlineTextButton)
    float borderThickness = 2.0f;
    float cornerRadius = 8.0f;
    juce::Colour borderColour = juce::Colours::darkblue;
    
    void setupSectionNameLabel();
    void updateContent();
    void drawBorder(juce::Graphics& g);
    void drawSeparatorLine(juce::Graphics& g);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionEditor)
}; 