#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Panel coloré cliquable basé sur juce::Button
 * Ressemble visuellement à ColoredPanel mais avec onClick natif
 */
class ButtonColoredPanel : public juce::Button
{
public:
    ButtonColoredPanel(juce::Colour color) 
        : juce::Button(""),  // Bouton sans texte
          baseColor(color),
          isSelected(false)
    {
        // Désactiver le look and feel par défaut du bouton
        setButtonText("");
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
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
    
    // Gestion de la sélection
    void setSelected(bool selected)
    {
        if (isSelected != selected)
        {
            isSelected = selected;
            repaint();
        }
    }
    
    bool getSelected() const
    {
        return isSelected;
    }
    
    void setColor(juce::Colour color)
    {
        baseColor = color;
        repaint();
    }
    
    juce::Colour getColor() const
    {
        return baseColor;
    }
    
    void setUserData(const juce::var& data)
    {
        userData = data;
    }
    
    juce::var getUserData() const
    {
        return userData;
    }

private:
    juce::Colour baseColor;
    bool isSelected;
    juce::var userData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonColoredPanel)
}; 