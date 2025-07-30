#pragma once

#include <JuceHeader.h>

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
    juce::Colour backgroundColour = juce::Colours::lightcoral.withAlpha(0.3f);
    juce::Colour borderColour = juce::Colours::black;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone1)
};