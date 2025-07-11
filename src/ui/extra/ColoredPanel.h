#pragma once

#include <JuceHeader.h>

//==============================================================================
// Classe simple pour les panneaux colorés
class ColoredPanel : public juce::Component
{
public:
    ColoredPanel(juce::Colour color) : panelColor(color) 
    {
        setOpaque(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(panelColor);
    }
    
private:
    juce::Colour panelColor;
}; 