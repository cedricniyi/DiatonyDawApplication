#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "utils/FontManager.h"
#include "PlaybackActionArea.h"
#include "GenerationButtons.h"

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea() 
        : ColoredPanel(juce::Colour::fromString("#fff0faff"))
    {
        // Définir l'alpha pour que le composant en dessous soit visible
        setAlpha(0.85f); 
        
        // Ajouter les composants
        addAndMakeVisible(generationButtons);
        addAndMakeVisible(playbackActionArea);
        
        // Configuration du FlexBox
        flexBox.flexDirection = juce::FlexBox::Direction::row;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        flexBox.alignItems = juce::FlexBox::AlignItems::center;
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        // Ajouter les éléments au FlexBox - PlaybackActionArea à gauche
        auto preferredSize = playbackActionArea.getPreferredSize();
        flexBox.items.add(juce::FlexItem(playbackActionArea)
            .withWidth(preferredSize.getWidth())
            .withHeight(preferredSize.getHeight())
            .withMargin(juce::FlexItem::Margin(0, 0, 0, 0)));
        
        // Puis GenerationButtons à droite
        auto generationSize = generationButtons.getPreferredSize();
        flexBox.items.add(juce::FlexItem(generationButtons)
            .withWidth(generationSize.getWidth())
            .withHeight(generationSize.getHeight())
            .withMargin(juce::FlexItem::Margin(0, 0, 0, 0)));
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Marges adaptatives basées sur la taille disponible
        int horizontalMargin = juce::jmin(10, bounds.getWidth() / 15);
        int verticalMargin = juce::jmin(10, bounds.getHeight() / 15);
        
        auto area = bounds.reduced(horizontalMargin, verticalMargin);
        flexBox.performLayout(area.toFloat());
    }
    
    // Accès aux composants pour les callbacks
    GenerationButtons& getGenerationButtons() { return generationButtons; }
    PlaybackActionArea& getPlaybackActionArea() { return playbackActionArea; }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    GenerationButtons generationButtons;
    PlaybackActionArea playbackActionArea;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 