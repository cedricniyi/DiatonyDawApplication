#include "ButtonColoredPanel.h"

//==============================================================================
ButtonColoredPanel::ButtonColoredPanel(juce::Colour color) 
    : juce::Button(""),  // Bouton sans texte
      baseColor(color),
      isSelected(false),
      contentType(PanelContentType::Section)  // Par défaut Section
{
    // Désactiver le look and feel par défaut du bouton
    setButtonText("");
}

void ButtonColoredPanel::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Créer le path pour le rectangle arrondi (identique à ColoredPanel)
    juce::Path panelPath;
    panelPath.addRoundedRectangle(bounds, 10.0f);

    // Couleur avec effets hover et sélection
    juce::Colour currentColor;
    
    if (isSelected)
    {
        // Panel sélectionné : rouge
        currentColor = juce::Colours::red;
    }
    else
    {
        // Panel normal : couleur de base
        currentColor = baseColor;
    }
    
    if (shouldDrawButtonAsHighlighted)
    {
        // Panel survolé : légèrement plus lumineux (équivalent au hover)
        currentColor = currentColor.brighter(0.15f);
    }
    
    // Rendre le panel (sans bordure, identique à ColoredPanel)
    g.setColour(currentColor);
    g.fillPath(panelPath);
}

void ButtonColoredPanel::setSelected(bool selected)
{
    if (isSelected != selected)
    {
        isSelected = selected;
        repaint();
    }
}

bool ButtonColoredPanel::getSelected() const
{
    return isSelected;
}

void ButtonColoredPanel::setColor(juce::Colour color)
{
    baseColor = color;
    repaint();
}

juce::Colour ButtonColoredPanel::getColor() const
{
    return baseColor;
}

void ButtonColoredPanel::setUserData(const juce::var& data)
{
    userData = data;
}

juce::var ButtonColoredPanel::getUserData() const
{
    return userData;
}

void ButtonColoredPanel::setContentType(PanelContentType type)
{
    contentType = type;
}

PanelContentType ButtonColoredPanel::getContentType() const
{
    return contentType;
} 