#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"

//==============================================================================
class TopPanel : public ColoredPanel
{
public:
    TopPanel() : ColoredPanel(juce::Colours::black) {}
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
        // Ici vous pouvez ajouter des éléments spécifiques au TopPanel
    }
    
    void resized() override
    {
        // Ici vous pouvez gérer le layout spécifique au TopPanel
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopPanel)
};
