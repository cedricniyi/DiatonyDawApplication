#pragma once

#include <JuceHeader.h>
#include "ColoredPanel.h"

//==============================================================================
/**
 * Panel coloré cliquable avec callback et gestion hover
 */
class ClickableColoredPanel : public ColoredPanel
{
public:
    // Type de callback pour les clics
    using ClickCallback = std::function<void(ClickableColoredPanel*)>;
    
    ClickableColoredPanel(juce::Colour color, ClickCallback clickCallback = nullptr) 
        : ColoredPanel(color),
          originalColor(color),
          onClickCallback(clickCallback),
          isHovered(false),
          isSelected(false)
    {
        // Activer les événements souris
        setInterceptsMouseClicks(true, false);
    }
    
    void paint(juce::Graphics& g) override
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
    
    void mouseEnter(const juce::MouseEvent& event) override
    {
        isHovered = true;
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent& event) override
    {
        isHovered = false;
        setMouseCursor(juce::MouseCursor::NormalCursor);
        repaint();
    }
    
    void mouseDown(const juce::MouseEvent& event) override
    {
        // Pas d'effet visuel spécial sur mouseDown - la sélection gère l'affichage
    }
    
    void mouseUp(const juce::MouseEvent& event) override
    {
        // Si le clic s'est terminé sur le composant, déclencher le callback
        if (event.mods.isLeftButtonDown() && getLocalBounds().contains(event.getPosition()))
        {
            if (onClickCallback)
                onClickCallback(this);
        }
    }
    
    // Setters pour les callbacks et données
    void setClickCallback(ClickCallback callback)
    {
        onClickCallback = callback;
    }
    
    void setUserData(const juce::var& data)
    {
        userData = data;
    }
    
    juce::var getUserData() const
    {
        return userData;
    }
    
    void setColor(juce::Colour color) override
    {
        originalColor = color;
        ColoredPanel::setColor(color);
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

private:
    juce::Colour originalColor;
    ClickCallback onClickCallback;
    bool isHovered;
    bool isSelected;
    juce::var userData; // Pour stocker des données associées (ex: ID de progression)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClickableColoredPanel)
}; 