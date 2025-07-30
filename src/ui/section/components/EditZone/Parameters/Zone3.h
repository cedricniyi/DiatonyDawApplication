#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"

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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone3)
};