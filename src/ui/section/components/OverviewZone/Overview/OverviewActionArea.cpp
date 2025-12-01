#include "OverviewActionArea.h"
#include "utils/FontManager.h"

//==============================================================================
OverviewActionArea::OverviewActionArea() 
    : ColoredPanel(juce::Colour::fromString("#fffcfcff"))
{
    // Définir l'alpha pour que le composant en dessous soit visible
    setAlpha(0.85f); 
    
    // Ajouter uniquement l'OverviewArea
    addAndMakeVisible(overviewArea);
    
    // Configuration du FlexBox (simplifié, un seul élément centré)
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexBox.alignItems = juce::FlexBox::AlignItems::center;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    
    // OverviewArea occupe tout l'espace disponible
    auto overviewSize = overviewArea.getPreferredSize();
    flexBox.items.add(juce::FlexItem(overviewArea)
        .withMinWidth(150.0f)
        .withHeight(overviewSize.getHeight())
        .withFlex(1, 1, 200.0f));
}

void OverviewActionArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void OverviewActionArea::resized()
{
    auto bounds = getLocalBounds();
    
    // Marges adaptatives basées sur la taille disponible
    int horizontalMargin = juce::jmin(8, bounds.getWidth() / 18);
    int verticalMargin = juce::jmin(4, bounds.getHeight() / 25);
    
    auto area = bounds.reduced(horizontalMargin, verticalMargin);
    flexBox.performLayout(area.toFloat());
}

OverviewArea& OverviewActionArea::getOverviewArea()
{
    return overviewArea;
}
