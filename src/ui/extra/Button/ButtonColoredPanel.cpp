#include "ButtonColoredPanel.h"

//==============================================================================
ButtonColoredPanel::ButtonColoredPanel(juce::Colour color) 
    : juce::Button(""),  // Bouton sans texte
      baseColor(color),
      isSelected(false),
      contentType(PanelContentType::Section),  // Par défaut Section
      displayText(""),  // Pas de texte par défaut
      showText(true)  // Affichage du texte activé par défaut (rétrocompatibilité)
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
    
    // Afficher le texte s'il y en a un ET si l'affichage est activé
    if (showText && displayText.isNotEmpty())
    {
        // Couleur du texte : blanc pour contraster avec les couleurs de fond
        g.setColour(juce::Colours::white);
        
        // Police adaptée à la taille du panel
        g.setFont(juce::FontOptions(12.0f));
        
        // Dessiner le texte centré
        g.drawText(displayText, bounds.toNearestInt(), juce::Justification::centred, true);
    }
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

void ButtonColoredPanel::setDisplayText(const juce::String& text)
{
    if (displayText != text)
    {
        displayText = text;
        repaint();  // Redessiner pour afficher le nouveau texte
    }
}

juce::String ButtonColoredPanel::getDisplayText() const
{
    return displayText;
}

void ButtonColoredPanel::setShowText(bool shouldShowText)
{
    if (showText != shouldShowText)
    {
        showText = shouldShowText;
        repaint();  // Redessiner pour mettre à jour l'affichage
    }
}

bool ButtonColoredPanel::getShowText() const
{
    return showText;
} 