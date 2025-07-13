#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"

//==============================================================================
class FooterPanel : public ColoredPanel
{
public:
    FooterPanel() : ColoredPanel(juce::Colours::white) {}
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
        // Ici vous pouvez ajouter des éléments spécifiques au FooterPanel
    }
    
    void resized() override
    {
        // Ici vous pouvez gérer le layout spécifique au FooterPanel
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 