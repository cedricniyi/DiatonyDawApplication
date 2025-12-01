#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "OverviewArea.h"

// Forward declarations  
class FontManager;

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Accès au composant OverviewArea
    OverviewArea& getOverviewArea();
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    OverviewArea overviewArea;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
};
