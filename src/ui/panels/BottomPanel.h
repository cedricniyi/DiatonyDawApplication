#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"

//==============================================================================
class BottomPanel : public ColoredPanel
{
public:
    BottomPanel() : ColoredPanel(juce::Colours::lightslategrey) {}
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
        // Ici vous pouvez ajouter des éléments spécifiques au BottomPanel
    }
    
    void resized() override
    {
        // Ici vous pouvez gérer le layout spécifique au BottomPanel
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BottomPanel)
}; 