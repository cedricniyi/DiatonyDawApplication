#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "utils/FontManager.h"
#include "PlaybackActionArea.h"
#include "GenerationButtons.h"
#include "OverviewArea.h"

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea() 
        : ColoredPanel(juce::Colours::white)
    {
        // Définir l'alpha pour que le composant en dessous soit visible
        setAlpha(0.85f); 
        
        // Ajouter les composants
        addAndMakeVisible(playbackActionArea);
        addAndMakeVisible(overviewArea);
        addAndMakeVisible(generationButtons);
        
        // Configuration du FlexBox
        flexBox.flexDirection = juce::FlexBox::Direction::row;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        flexBox.alignItems = juce::FlexBox::AlignItems::center;
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        // Ajouter les éléments au FlexBox
        // PlaybackActionArea à gauche (taille fixe)
        auto playbackSize = playbackActionArea.getPreferredSize();
        flexBox.items.add(juce::FlexItem(playbackActionArea)
            .withWidth(playbackSize.getWidth())
            .withHeight(playbackSize.getHeight())
            .withFlex(0, 0, playbackSize.getWidth())
            .withMargin(juce::FlexItem::Margin(0, 10, 0, 0)));
        
        // OverviewArea au centre (responsive en largeur)
        auto overviewSize = overviewArea.getPreferredSize();
        flexBox.items.add(juce::FlexItem(overviewArea)
            .withMinWidth(150.0f)
            .withHeight(overviewSize.getHeight())
            .withFlex(1, 1, 200.0f) // Flexible en largeur
            .withMargin(juce::FlexItem::Margin(0, 10, 0, 10)));
        
        // GenerationButtons à droite (taille fixe)
        auto generationSize = generationButtons.getPreferredSize();
        flexBox.items.add(juce::FlexItem(generationButtons)
            .withWidth(generationSize.getWidth())
            .withHeight(generationSize.getHeight())
            .withFlex(0, 0, generationSize.getWidth())
            .withMargin(juce::FlexItem::Margin(0, 0, 0, 10)));
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Marges adaptatives basées sur la taille disponible (réduites pour plus d'espace)
        int horizontalMargin = juce::jmin(8, bounds.getWidth() / 18);
        int verticalMargin = juce::jmin(4, bounds.getHeight() / 25);
        
        auto area = bounds.reduced(horizontalMargin, verticalMargin);
        flexBox.performLayout(area.toFloat());
    }
    
    // Accès aux composants pour les callbacks
    GenerationButtons& getGenerationButtons() { return generationButtons; }
    PlaybackActionArea& getPlaybackActionArea() { return playbackActionArea; }
    OverviewArea& getOverviewArea() { return overviewArea; }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    PlaybackActionArea playbackActionArea;
    OverviewArea overviewArea;
    GenerationButtons generationButtons;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 