#pragma once

#include <JuceHeader.h>

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
    juce::Colour backgroundColour = juce::Colours::lightsteelblue.withAlpha(0.3f);
    juce::Colour borderColour = juce::Colours::black;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4)
};