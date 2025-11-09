#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"
#include "ui/DiatonyText.h"

/**
 * Zone 2 - Deuxième paramètre/contrôle de section
 * Première ligne, deuxième colonne
 */
class Zone2 : public juce::Component
{
public:
    Zone2();
    ~Zone2() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Binding depuis SectionEditor
    std::function<void(Diatony::Alteration)> onAlterationChanged; // UI -> Modèle
    void setSelectedAlteration(Diatony::Alteration alt);          // Modèle -> UI

private:
    // Couleur de fond pour cette zone
    juce::Colour titleBackgroundColour = juce::Colours::lightgreen.withAlpha(0.3f);
    juce::Colour contentBackgroundColour = juce::Colours::lightgreen.withAlpha(0.5f); // Plus foncé
    juce::Colour borderColour = juce::Colours::black;
    
    // Layout des zones
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> contentArea;
    
    // FontManager pour le titre
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Les 3 boutons SelectableStyledButton correspondant aux 3 altérations
    std::array<std::unique_ptr<SelectableStyledButton>, 3> styledButtons;
    
    // Les 3 altérations dans l'ordre logique
    static constexpr std::array<Diatony::Alteration, 3> alterations = {
        Diatony::Alteration::Flat, Diatony::Alteration::Natural, Diatony::Alteration::Sharp
    };
    
    // Altération actuellement sélectionnée
    Diatony::Alteration selectedAlteration = Diatony::Alteration::Natural;
    
    void setupStyledButtons();
    void layoutStyledButtons();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone2)
};