#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "utils/FontManager.h"

//==============================================================================
class OverviewArea : public ColoredPanel
{
public:
    OverviewArea() 
        : ColoredPanel(juce::Colour::fromString("#f5f5f5ff"))
    {
        setAlpha(0.9f);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
        
        // Optionnel: ajouter du contenu visuel avec padding interne
        g.setColour(juce::Colours::darkblue);
        g.setFont(fontManager->getSFProText(16.0f, FontManager::FontWeight::Bold));
        
        // Appliquer un padding interne pour le texte
        auto textArea = getLocalBounds().reduced(12, 8);
        g.drawText("OVERVIEW", textArea, juce::Justification::left);
    }
    
    void resized() override
    {
        ColoredPanel::resized();
    }
    
    // Méthode pour obtenir la taille préférée
    juce::Rectangle<int> getPreferredSize() const
    {
        return juce::Rectangle<int>(0, 0, 200, 55); // Largeur flexible, hauteur fixe
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewArea)
}; 