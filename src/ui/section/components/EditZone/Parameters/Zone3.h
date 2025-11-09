#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"
#include "ui/DiatonyText.h"

/**
 * Zone 3 - Troisième paramètre/contrôle de section
 * Première ligne, troisième colonne
 */
class Zone3 : public juce::Component
{
public:
    Zone3();
    ~Zone3() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Binding depuis SectionEditor
    std::function<void(Diatony::Mode)> onModeChanged; // UI -> Modèle
    void setSelectedMode(Diatony::Mode mode);         // Modèle -> UI

private:
    // Couleur de fond pour cette zone
    juce::Colour titleBackgroundColour = juce::Colours::lightyellow.withAlpha(0.3f);
    juce::Colour contentBackgroundColour = juce::Colours::lightyellow.withAlpha(0.5f); // Plus foncé
    juce::Colour borderColour = juce::Colours::black;
    
    // Layout des zones
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> contentArea;
    
    // FontManager pour le titre
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Les 2 boutons SelectableStyledButton rectangulaires correspondant aux 2 modes
    std::array<std::unique_ptr<SelectableStyledButton>, 2> styledButtons;
    
    // Les 2 modes dans l'ordre logique
    static constexpr std::array<Diatony::Mode, 2> modes = {
        Diatony::Mode::Major, Diatony::Mode::Minor
    };
    
    // Mode actuellement sélectionné
    Diatony::Mode selectedMode = Diatony::Mode::Major;
    
    void setupStyledButtons();
    void layoutStyledButtons();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone3)
};