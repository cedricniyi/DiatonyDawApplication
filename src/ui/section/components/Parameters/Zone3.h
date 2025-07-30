#pragma once

#include <JuceHeader.h>

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
    juce::Colour backgroundColour = juce::Colours::lightyellow.withAlpha(0.3f);
    juce::Colour borderColour = juce::Colours::black;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone3)
};