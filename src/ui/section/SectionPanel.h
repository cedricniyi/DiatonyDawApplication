#pragma once

#include <JuceHeader.h>
#include "components/OverviewActionArea.h"
#include "components/ProgressionArea.h"

//==============================================================================
class SectionPanel : public juce::Component
{
public:
    SectionPanel() 
        : overviewActionArea(),
          progressionArea()
    {
        addAndMakeVisible (overviewActionArea);
        addAndMakeVisible (progressionArea);
    }
    
    void paint(juce::Graphics& g) override
    {

    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        // Diviser l'espace en deux zones verticales avec FlexBox
        juce::FlexBox fb;
        fb.flexDirection = juce::FlexBox::Direction::column;
        fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        fb.alignItems = juce::FlexBox::AlignItems::stretch;

        // Overview action area en haut (10% de l'espace)
        fb.items.add(juce::FlexItem(overviewActionArea).withFlex(0.15f).withMargin({0, 0, 4, 0}));
        
        // Progression area en bas (90% de l'espace restant)
        fb.items.add(juce::FlexItem(progressionArea).withFlex(0.85f).withMargin({4, 0, 0, 0}));

        fb.performLayout(area);
    }
    
private:
    OverviewActionArea  overviewActionArea;
    ProgressionArea     progressionArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionPanel)
}; 