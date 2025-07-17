#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"

//==============================================================================
class OverviewContentArea : public ColoredPanel
{
public:
    OverviewContentArea() 
        : ColoredPanel(juce::Colour::fromString("#ffe3e3ff"))
    {
        // Configuration de base
        setAlpha(1.0f);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
        
        // TODO: Ici on ajoutera plus tard les rectangles cliquables
    }
    
    void resized() override
    {
        ColoredPanel::resized();
        
        // TODO: Ici on gérera le layout des rectangles cliquables
    }
    
    // Méthode pour obtenir la taille préférée
    juce::Rectangle<int> getPreferredSize() const
    {
        return juce::Rectangle<int>(0, 0, 300, 35); // Assez long, hauteur raisonnable
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewContentArea)
}; 