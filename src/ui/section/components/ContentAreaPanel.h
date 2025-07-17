#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"

//==============================================================================
class ContentAreaPanel : public ColoredPanel
{
public:
    ContentAreaPanel() : ColoredPanel(juce::Colours::lightblue) {}
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
        
        // Bordure pour délimiter clairement
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::blue);
        g.drawRoundedRectangle(bounds.reduced(1.0f), 10.0f, 2.0f);
        
        // Texte explicatif au centre de la zone
        g.setColour(juce::Colours::darkblue);
        g.setFont(16.0f);
        g.drawText("Zone disponible pour le contenu", getLocalBounds(), juce::Justification::centred);
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContentAreaPanel)
}; 