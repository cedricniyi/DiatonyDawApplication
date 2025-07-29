#pragma once

#include <JuceHeader.h>

/**
 * Composant d'édition de modulation - Placeholder pour l'édition détaillée d'une modulation
 * Reçoit l'ID de la modulation à éditer et affiche des informations/contrôles d'édition
 */
class ModulationEditor : public juce::Component
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
    juce::Label titleLabel;
    juce::Label contentLabel;
    
    void setupLabels();
    void updateContent();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationEditor)
}; 