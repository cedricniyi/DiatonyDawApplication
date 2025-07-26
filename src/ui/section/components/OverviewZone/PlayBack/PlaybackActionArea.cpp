#include "PlaybackActionArea.h"
#include "utils/FontManager.h"

//==============================================================================
PlaybackActionArea::PlaybackActionArea() 
    : ColoredPanel(juce::Colour::fromString("#ffededfc")),
      playButton(juce::String::fromUTF8("▶"), juce::Colours::black, juce::Colour::fromString("#ff666666"), 16.0f, FontManager::FontWeight::Bold),
      pauseButton(juce::String::fromUTF8("⏸"), juce::Colours::black, juce::Colour::fromString("#ff666666"), 16.0f, FontManager::FontWeight::Bold),
      stopButton(juce::String::fromUTF8("⏹"), juce::Colours::black, juce::Colour::fromString("#ff666666"), 16.0f, FontManager::FontWeight::Bold)
{
    // Ajouter les boutons
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    
    // Configuration du FlexBox
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    
    // Ajouter les boutons au FlexBox avec espacement
    flexBox.items.add(juce::FlexItem(playButton).withWidth(32.0f).withHeight(32.0f).withMargin(juce::FlexItem::Margin(0, 4, 0, 0)));
    flexBox.items.add(juce::FlexItem(pauseButton).withWidth(32.0f).withHeight(32.0f).withMargin(juce::FlexItem::Margin(0, 4, 0, 0)));
    flexBox.items.add(juce::FlexItem(stopButton).withWidth(32.0f).withHeight(32.0f).withMargin(juce::FlexItem::Margin(0, 0, 0, 0)));
}

void PlaybackActionArea::paint(juce::Graphics& g)
{
    ColoredPanel::paint(g);
}

void PlaybackActionArea::resized()
{
    auto bounds = getLocalBounds();
    
    // Marges adaptatives pour éviter les problèmes aux petites tailles
    int horizontalMargin = juce::jmin(4, bounds.getWidth() / 15);
    int verticalMargin = juce::jmin(6, bounds.getHeight() / 8);
    
    auto area = bounds.reduced(horizontalMargin, verticalMargin);
    flexBox.performLayout(area.toFloat());
}

juce::Rectangle<int> PlaybackActionArea::getPreferredSize() const
{
    // Calcul basé sur le nombre de boutons et leurs dimensions
    int buttonWidth = 32;
    int buttonSpacing = 4;
    int padding = 16; // 8px de chaque côté
    
    int totalWidth = (3 * buttonWidth) + (2 * buttonSpacing) + padding; // 3 boutons, 2 espaces
    int totalHeight = 32 + 12; // hauteur bouton + padding vertical
    
    return juce::Rectangle<int>(0, 0, totalWidth, totalHeight);
}

StyledButton& PlaybackActionArea::getPlayButton()
{
    return playButton;
}

StyledButton& PlaybackActionArea::getPauseButton()
{
    return pauseButton;
}

StyledButton& PlaybackActionArea::getStopButton()
{
    return stopButton;
} 