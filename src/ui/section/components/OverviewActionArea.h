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
        
        // Ajouter les éléments au FlexBox - taille adaptée au contenu
        auto preferredSize = playbackActionArea.getPreferredSize();
        flexBox.items.add(juce::FlexItem(playbackActionArea)
            .withWidth(preferredSize.getWidth())
            .withHeight(preferredSize.getHeight())
            .withMargin(juce::FlexItem::Margin(0, 20, 0, 0)));
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
        int horizontalMargin = juce::jmin(20, bounds.getWidth() / 25);  // Max 20px, min selon largeur
        int verticalMargin = juce::jmin(10, bounds.getHeight() / 15);   // Max 10px, min selon hauteur
        
        auto area = bounds.reduced(horizontalMargin, verticalMargin);
        flexBox.performLayout(area.toFloat());
    }
    
    // Accès au composant playback pour les callbacks
    PlaybackActionArea& getPlaybackActionArea() { return playbackActionArea; }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    PlaybackActionArea playbackActionArea;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 