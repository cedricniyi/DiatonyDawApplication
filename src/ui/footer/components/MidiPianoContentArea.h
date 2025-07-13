#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"

//==============================================================================
class MidiPianoContentArea : public ColoredPanel
{
public:
    MidiPianoContentArea() : ColoredPanel(juce::Colours::lightgreen) {}
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
        
        // Bordure pour délimiter clairement
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::green);
        g.drawRoundedRectangle(bounds.reduced(1.0f), 10.0f, 2.0f);
        
        // Texte explicatif au centre de la zone
        g.setColour(juce::Colours::darkgreen);
        g.setFont(16.0f);
        g.drawText("Zone disponible pour le midi piano", getLocalBounds(), juce::Justification::centred);
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPianoContentArea)
}; 