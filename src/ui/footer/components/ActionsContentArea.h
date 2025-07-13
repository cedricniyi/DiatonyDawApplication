#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"

//==============================================================================
class ActionsContentArea : public ColoredPanel
{
public:
    ActionsContentArea() : ColoredPanel(juce::Colours::lightcoral) {}
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
        
        // Bordure pour délimiter clairement
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::red);
        g.drawRoundedRectangle(bounds.reduced(1.0f), 10.0f, 2.0f);
        
        // Texte explicatif au centre de la zone
        g.setColour(juce::Colours::darkred);
        g.setFont(16.0f);
        g.drawText("Zone disponible pour les actions", getLocalBounds(), juce::Justification::centred);
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActionsContentArea)
}; 