#pragma once

#include <JuceHeader.h>

//==============================================================================
// Classe simple pour les panneaux colorés avec shadow performant
class ColoredPanel : public juce::Component
{
public:
    ColoredPanel(juce::Colour color) 
        : panelColor(color)    {
        setOpaque(false);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Créer le path pour le rectangle arrondi
        juce::Path panelPath;
        panelPath.addRoundedRectangle(bounds, 10.0f);

        // Rendre le panel
        g.setColour(panelColor);
        g.fillPath(panelPath);
    }
    
    void resized() override
    {
    }
    
    void setAlpha(float alpha)
    {
        panelColor = panelColor.withAlpha(alpha);
        repaint();
    }
    
    void setColor(juce::Colour color)
    {
        panelColor = color;
        repaint();
    }
    
    juce::Colour getColor() const
    {
        return panelColor;
    }
    
private:
    juce::Colour panelColor;

}; 