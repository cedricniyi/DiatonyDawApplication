#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "utils/FontManager.h"
#include "PlaybackActionArea.h"

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea() 
        : ColoredPanel(juce::Colour::fromString("#fff0faff"))
    {
        // Définir l'alpha pour que le composant en dessous soit visible
        setAlpha(0.85f); 
        
        // Ajouter les boutons de contrôle média
        addAndMakeVisible(playbackActionArea);
        
        // Configuration du FlexBox
        flexBox.flexDirection = juce::FlexBox::Direction::row;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        flexBox.alignItems = juce::FlexBox::AlignItems::center;
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        // Ajouter les éléments au FlexBox
        flexBox.items.add(juce::FlexItem(playbackActionArea).withMinWidth(130.0f).withMinHeight(40.0f).withMargin(juce::FlexItem::Margin(0, 20, 0, 0)));
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20, 10);
        flexBox.performLayout(area.toFloat());
    }
    
    // Accès aux boutons pour les callbacks
    PlaybackActionArea& getPlaybackActionArea() { return playbackActionArea; }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    PlaybackActionArea playbackActionArea;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 