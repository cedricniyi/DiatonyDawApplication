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
    // Fond gris foncé #333333
    g.fillAll(juce::Colour::fromString("#FF333333"));
}

void SectionPanel::resized()
{
    auto area = getLocalBounds();
    
    // Diviser l'espace en deux zones verticales avec FlexBox
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    fb.alignItems = juce::FlexBox::AlignItems::stretch;

    // Overview action area en haut (10% de l'espace)
    fb.items.add(juce::FlexItem(overviewActionArea).withFlex(0.20f).withMargin({0, 0, 4, 0}));
    
    // Progression area en bas (90% de l'espace restant)
    fb.items.add(juce::FlexItem(progressionArea).withFlex(0.80f).withMargin({4, 0, 0, 0}));

    fb.performLayout(area);
} 