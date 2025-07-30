#pragma once

#include <JuceHeader.h>

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

private:
    // Couleur de fond pour cette zone
    juce::Colour backgroundColour = juce::Colours::lightgreen.withAlpha(0.3f);
    juce::Colour borderColour = juce::Colours::black;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone2)
};