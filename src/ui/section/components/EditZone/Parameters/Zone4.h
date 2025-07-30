#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"

/**
 * Zone 4 - Quatrième paramètre/contrôle de section
 * Seconde ligne, toute la largeur
 */
class Zone4 : public juce::Component
{
public:
    Zone4();
    ~Zone4() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Couleur de fond pour cette zone
    juce::Colour titleBackgroundColour = juce::Colours::lightsteelblue.withAlpha(0.3f);
    juce::Colour contentBackgroundColour = juce::Colours::lightsteelblue.withAlpha(0.5f); // Plus foncé
    juce::Colour borderColour = juce::Colours::black;
    
    // Layout des zones
    juce::Rectangle<int> titleArea;
    juce::Rectangle<int> contentArea;
    
    // FontManager pour le titre
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4)
};