#pragma once

#include <JuceHeader.h>

//==============================================================================
// Classe simple pour les panneaux colorés avec shadow performant
class ColoredPanel : public juce::Component
{
public:
    ColoredPanel(juce::Colour color);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setAlpha(float alpha);
    virtual void setColor(juce::Colour color);
    juce::Colour getColor() const;
    
private:
    juce::Colour panelColor;
}; 