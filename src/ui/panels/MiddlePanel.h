#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"

//==============================================================================
class MiddlePanel : public ColoredPanel
{
public:
    MiddlePanel() : ColoredPanel(juce::Colours::dimgrey) {}
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
        // Ici vous pouvez ajouter des éléments spécifiques au MiddlePanel
    }
    
    void resized() override
    {
        // Ici vous pouvez gérer le layout spécifique au MiddlePanel
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiddlePanel)
}; 