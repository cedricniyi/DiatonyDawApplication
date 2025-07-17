#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../extra/StyledButton.h"

//==============================================================================
class PlaybackActionArea : public ColoredPanel
{
public:
    PlaybackActionArea() 
        : ColoredPanel(juce::Colours::transparentBlack),
          playButton(juce::String::fromUTF8("▶"), juce::Colour::fromString("#ff4CAF50"), juce::Colour::fromString("#ff45A049"), 20.0f, FontManager::FontWeight::Bold),
          pauseButton(juce::String::fromUTF8("⏸"), juce::Colour::fromString("#ffFFC107"), juce::Colour::fromString("#ffFFB300"), 20.0f, FontManager::FontWeight::Bold),
          stopButton(juce::String::fromUTF8("⏹"), juce::Colour::fromString("#ff9C27B0"), juce::Colour::fromString("#ff8E24AA"), 20.0f, FontManager::FontWeight::Bold)
    {
        // Configuration des boutons
        setupButton(playButton);
        setupButton(pauseButton);
        setupButton(stopButton);
        
        // Ajouter les boutons
        addAndMakeVisible(playButton);
        addAndMakeVisible(pauseButton);
        addAndMakeVisible(stopButton);
        
        // Configuration du FlexBox
        flexBox.flexDirection = juce::FlexBox::Direction::row;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        flexBox.alignItems = juce::FlexBox::AlignItems::center;
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        // Ajouter les éléments au FlexBox avec un espacement
        flexBox.items.add(juce::FlexItem(playButton).withMinWidth(40.0f).withMinHeight(40.0f).withMargin(juce::FlexItem::Margin(0, 5, 0, 0)));
        flexBox.items.add(juce::FlexItem(pauseButton).withMinWidth(40.0f).withMinHeight(40.0f).withMargin(juce::FlexItem::Margin(0, 5, 0, 0)));
        flexBox.items.add(juce::FlexItem(stopButton).withMinWidth(40.0f).withMinHeight(40.0f).withMargin(juce::FlexItem::Margin(0, 0, 0, 0)));
    }
    
    void paint(juce::Graphics& g) override
    {
        // Pas de fond visible pour ce conteneur
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        flexBox.performLayout(bounds.toFloat());
    }
    
    // Méthodes pour accéder aux boutons (si nécessaire pour les callbacks)
    StyledButton& getPlayButton() { return playButton; }
    StyledButton& getPauseButton() { return pauseButton; }
    StyledButton& getStopButton() { return stopButton; }
    
private:
    StyledButton playButton;
    StyledButton pauseButton;
    StyledButton stopButton;
    juce::FlexBox flexBox;
    
    void setupButton(StyledButton& button)
    {
        // Rendre les boutons carrés
        button.setSize(40, 40);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackActionArea)
}; 