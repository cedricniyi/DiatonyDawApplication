#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../../utils/FontManager.h"
#include "InteractivePianoContentArea.h"

//==============================================================================
// Conteneur principal avec padding
class InteractivePiano : public ColoredPanel
{
public:
    InteractivePiano(juce::Colour color) 
        : ColoredPanel(color),
          fontManager(),
          contentArea()
    {
        addAndMakeVisible(contentArea);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Appeler le paint de ColoredPanel pour le fond
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        // Appliquer un padding de 10px tout autour
        auto bounds = getLocalBounds();
        auto contentBounds = bounds.reduced(10);
        
        contentArea.setBounds(contentBounds);
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    InteractivePianoContentArea contentArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractivePiano)
}; 