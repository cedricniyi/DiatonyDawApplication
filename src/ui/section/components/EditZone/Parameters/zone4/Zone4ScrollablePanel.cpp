#include "Zone4ScrollablePanel.h"
#include "ui/extra/Component/Panel/InfoColoredPanel.h"

Zone4ScrollablePanel::Zone4ScrollablePanel() {}

void Zone4ScrollablePanel::paint(juce::Graphics&) {/* Contenu géré par les rect. enfants*/}

void Zone4ScrollablePanel::resized()
{
    // Recalculer la taille totale nécessaire car la hauteur peut avoir changé
    // (et la hauteur affecte la largeur des rectangles)
    if (!rectangles.empty())
        updateContentSize();
    else
        layoutRectangles();
}

void Zone4ScrollablePanel::addRectangle(std::unique_ptr<juce::Component> component)
{
    // Les dimensions sont ignorées car layoutRectangles() recalcule selon la hauteur disponible
    addRectangle(std::move(component), 0, 0);
}

void Zone4ScrollablePanel::addRectangle(std::unique_ptr<juce::Component> component, int width, int height)
{
    if (!component)
        return;
        
    addAndMakeVisible(*component);
    rectangles.emplace_back(std::move(component), width, height);
    updateContentSize();
}

void Zone4ScrollablePanel::clearAllRectangles()
{
    rectangles.clear();
    updateContentSize();
}

int Zone4ScrollablePanel::getNumRectangles() const
{
    return static_cast<int>(rectangles.size());
}

void Zone4ScrollablePanel::updateContentSize()
{
    auto currentHeight = getHeight();
    if (currentHeight <= 0) 
        currentHeight = 35;
    
    const int totalWidth = calculateRequiredWidth();
    setSize(totalWidth, currentHeight);
    layoutRectangles();
}

void Zone4ScrollablePanel::layoutRectangles()
{
    if (rectangles.empty())
        return;
        
    int x = 0;
    int totalHeight = getHeight();
    int rectangleHeight = totalHeight - SCROLLBAR_SPACE;
    int rectangleWidth = calculateChordWidth(rectangleHeight);
    
    for (auto& rectInfo : rectangles)
    {
        rectInfo.component->setBounds(x, 0, rectangleWidth, rectangleHeight);
        x += rectangleWidth + ChordPanelConfig::CHORD_SPACING;
    }
}

int Zone4ScrollablePanel::calculateChordWidth(int height) const
{
    int width;
    
    if (ChordPanelConfig::CHORD_WIDTH_FIXED > 0)
        width = ChordPanelConfig::CHORD_WIDTH_FIXED;
    else
        width = static_cast<int>(height * ChordPanelConfig::CHORD_WIDTH_RATIO);
    
    return juce::jmax(width, ChordPanelConfig::CHORD_WIDTH_MIN);
}

int Zone4ScrollablePanel::calculateRequiredWidth() const
{
    if (rectangles.empty())
        return MIN_CONTENT_WIDTH;
    
    int totalHeight = getHeight();
    int rectangleHeight = totalHeight - SCROLLBAR_SPACE;
    int rectangleWidth = calculateChordWidth(rectangleHeight);
    
    int totalWidth = static_cast<int>(rectangles.size()) * rectangleWidth 
                   + static_cast<int>(rectangles.size() - 1) * ChordPanelConfig::CHORD_SPACING
                   + ChordPanelConfig::CHORD_SPACING * 2;
    
    return juce::jmax(totalWidth, MIN_CONTENT_WIDTH);
}
