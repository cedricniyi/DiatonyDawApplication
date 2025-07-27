#include "ColoredPanel.h"

//==============================================================================
ColoredPanel::ColoredPanel(juce::Colour color) 
    : panelColor(color)
{
    setOpaque(false);
}

void ColoredPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Créer le path pour le rectangle arrondi
    juce::Path panelPath;
    panelPath.addRoundedRectangle(bounds, 10.0f);

    // Rendre le panel
    g.setColour(panelColor);
    g.fillPath(panelPath);
}

void ColoredPanel::resized()
{
}

void ColoredPanel::setAlpha(float alpha)
{
    panelColor = panelColor.withAlpha(alpha);
    repaint();
}

void ColoredPanel::setColor(juce::Colour color)
{
    panelColor = color;
    repaint();
}

juce::Colour ColoredPanel::getColor() const
{
    return panelColor;
} 