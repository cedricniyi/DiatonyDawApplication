#include "ColoredPanel.h"

//==============================================================================
ColoredPanel::ColoredPanel(juce::Colour color) 
    : panelColor(color)
{
    setOpaque(false);
}

void ColoredPanel::paint(juce::Graphics& g)
{
    auto bounds = getDrawingBounds().toFloat();
    
    if (bounds.isEmpty())
        return;
    
    // Créer le path pour le rectangle (arrondi ou non selon cornerRadius)
    juce::Path panelPath;
    
    if (cornerRadius > 0.0f)
        panelPath.addRoundedRectangle(bounds, cornerRadius);
    else
        panelPath.addRectangle(bounds);

    // Dessiner le fond
    g.setColour(panelColor);
    g.fillPath(panelPath);
    
    // Dessiner le contour si activé (style BaseZone)
    if (showBorder && borderThickness > 0.0f)
    {
        g.setColour(borderColor);
        
        if (cornerRadius > 0.0f)
            g.drawRoundedRectangle(bounds, cornerRadius, borderThickness);
        else
            g.drawRect(bounds, borderThickness);
    }
}

void ColoredPanel::resized()
{
}

// === Configuration de base ===

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

// === Configuration des coins arrondis ===

void ColoredPanel::setCornerRadius(float radius)
{
    cornerRadius = radius;
    repaint();
}

float ColoredPanel::getCornerRadius() const
{
    return cornerRadius;
}

// === Configuration du contour ===

void ColoredPanel::setBorderColor(juce::Colour color)
{
    borderColor = color;
    repaint();
}

void ColoredPanel::setBorderThickness(float thickness)
{
    borderThickness = thickness;
    repaint();
}

void ColoredPanel::enableBorder(bool shouldShowBorder)
{
    showBorder = shouldShowBorder;
    repaint();
}

// === Configuration du padding ===

void ColoredPanel::setInternalPadding(int padding)
{
    internalPadding = padding;
    repaint();
}

int ColoredPanel::getInternalPadding() const
{
    return internalPadding;
}

juce::Rectangle<int> ColoredPanel::getDrawingBounds() const
{
    return getLocalBounds().reduced(internalPadding);
}
