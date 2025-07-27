#include "ClickableColoredPanel.h"

//==============================================================================
ClickableColoredPanel::ClickableColoredPanel(juce::Colour color, ClickCallback clickCallback) 
    : ColoredPanel(color),
      originalColor(color),
      onClickCallback(clickCallback),
      isHovered(false),
      isSelected(false)
{
    // Activer les événements souris
    setInterceptsMouseClicks(true, false);
}

void ClickableColoredPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Créer le path pour le rectangle arrondi
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
        // Panel normal : couleur de base (bleu)
        currentColor = getColor();
    }
    
    if (isHovered)
    {
        // Panel survolé : légèrement plus lumineux
        currentColor = currentColor.brighter(0.15f);
    }
    
    // Rendre le panel (sans bordure)
    g.setColour(currentColor);
    g.fillPath(panelPath);
}

void ClickableColoredPanel::mouseEnter(const juce::MouseEvent& event)
{
    isHovered = true;
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    repaint();
}

void ClickableColoredPanel::mouseExit(const juce::MouseEvent& event)
{
    isHovered = false;
    setMouseCursor(juce::MouseCursor::NormalCursor);
    repaint();
}

void ClickableColoredPanel::mouseDown(const juce::MouseEvent& event)
{
    // Pas d'effet visuel spécial sur mouseDown - la sélection gère l'affichage
}

void ClickableColoredPanel::mouseUp(const juce::MouseEvent& event)
{
    // Si le clic s'est terminé sur le composant, déclencher le callback
    if (event.mods.isLeftButtonDown() && getLocalBounds().contains(event.getPosition()))
    {
        if (onClickCallback)
            onClickCallback(this);
    }
}

void ClickableColoredPanel::setClickCallback(ClickCallback callback)
{
    onClickCallback = callback;
}

void ClickableColoredPanel::setUserData(const juce::var& data)
{
    userData = data;
}

juce::var ClickableColoredPanel::getUserData() const
{
    return userData;
}

void ClickableColoredPanel::setColor(juce::Colour color)
{
    originalColor = color;
    ColoredPanel::setColor(color);
}

void ClickableColoredPanel::setSelected(bool selected)
{
    if (isSelected != selected)
    {
        isSelected = selected;
        repaint();
    }
}

bool ClickableColoredPanel::getSelected() const
{
    return isSelected;
} 