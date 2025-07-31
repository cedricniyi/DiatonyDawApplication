#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/CircularButton.h"

/**
 * Zone 1 - Premier paramètre/contrôle de section
 * Première ligne, première colonne
 */
class Zone1 : public juce::Component
{
public:
    Zone1();
    ~Zone1() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Couleur de fond pour cette zone
    juce::Colour titleBackgroundColour = juce::Colours::lightcoral.withAlpha(0.3f);
    juce::Colour contentBackgroundColour = juce::Colours::lightcoral.withAlpha(0.5f); // Plus foncé
    juce::Colour borderColour = juce::Colours::black;
    
    // Layout des zones
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> contentArea;
    
    // FontManager pour le titre
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Les 7 boutons circulaires
    std::array<std::unique_ptr<CircularButton>, 7> circularButtons;
    
    void setupCircularButtons();
    void layoutCircularButtons();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone1)
};