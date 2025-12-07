#include "SectionPanel.h"

//==============================================================================
SectionPanel::SectionPanel() 
    : overviewActionArea(),
      progressionArea()
{
    addAndMakeVisible (overviewActionArea);
    addAndMakeVisible (progressionArea);
}

void SectionPanel::paint(juce::Graphics& g)
{
    // Zone de délimitation - pas de fond
}

void SectionPanel::resized()
{
    auto area = getLocalBounds();
    
    // Diviser l'espace en deux zones verticales avec FlexBox
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    fb.alignItems = juce::FlexBox::AlignItems::stretch;

    fb.items.add(juce::FlexItem(overviewActionArea).withFlex(0.2).withMargin({8, 0, 4, 0}));
    
    fb.items.add(juce::FlexItem(progressionArea).withFlex(0.8).withMargin({4, 0, 8, 0}));

    fb.performLayout(area);
} 