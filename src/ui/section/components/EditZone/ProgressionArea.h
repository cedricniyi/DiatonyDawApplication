#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "WelcomeView.h"

//==============================================================================
class ProgressionArea : public ColoredPanel
{
public:
    ProgressionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    WelcomeView welcomeView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionArea)
}; 