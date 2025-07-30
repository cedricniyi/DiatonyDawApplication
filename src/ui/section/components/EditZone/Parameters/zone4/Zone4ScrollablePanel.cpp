#include "Zone4ScrollablePanel.h"

//==============================================================================
Zone4ScrollablePanel::Zone4ScrollablePanel()
{
}

void Zone4ScrollablePanel::paint(juce::Graphics& g)
{
    // Le contenu est géré par les rectangles enfants
    // Pas de rendu direct nécessaire ici
}

void Zone4ScrollablePanel::resized()
{
    layoutRectangles();
}

// Version rétrocompatible (utilise dimensions par défaut)
void Zone4ScrollablePanel::addRectangle(std::unique_ptr<juce::Component> component)
{
    addRectangle(std::move(component), DEFAULT_RECTANGLE_WIDTH, DEFAULT_RECTANGLE_HEIGHT);
}

// Version avec dimensions explicites
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
    const int totalWidth = calculateRequiredWidth();
    
    // Garder la hauteur actuelle (sera définie par le parent - Zone4ContentArea)
    auto currentHeight = getHeight();
    if (currentHeight <= 0) 
        currentHeight = 35; // Hauteur minimale de fallback
    
    setSize(juce::jmax(totalWidth, MIN_CONTENT_WIDTH), currentHeight);
    
    // Relayouter après changement de taille
    resized();
}

void Zone4ScrollablePanel::layoutRectangles()
{
    if (rectangles.empty())
        return;
        
    int x = 0;
    int totalHeight = getHeight();
    int rectangleHeight = totalHeight - SCROLLBAR_SPACE;
    
    for (auto& rectInfo : rectangles)
    {
        // Largeur = hauteur × 2, hauteur = hauteur disponible moins l'espace scrollbar
        int rectangleWidth = rectangleHeight * 2;
        rectInfo.component->setBounds(x, 0, rectangleWidth, rectangleHeight);
        x += rectangleWidth + RECTANGLE_SPACING;
    }
}

int Zone4ScrollablePanel::calculateRequiredWidth() const
{
    if (rectangles.empty())
        return MIN_CONTENT_WIDTH;
    
    int totalWidth = 0;
    int totalHeight = getHeight();
    int rectangleHeight = totalHeight - SCROLLBAR_SPACE;
    int rectangleWidth = rectangleHeight * 2;
    
    for (const auto& rectInfo : rectangles)
    {
        totalWidth += rectangleWidth + RECTANGLE_SPACING;
    }
    
    return totalWidth;
}