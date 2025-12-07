#include "Zone4ScrollablePanel.h"
#include "ui/extra/Component/Panel/InfoColoredPanel.h"

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
    // Recalculer la taille totale nécessaire car la hauteur peut avoir changé
    // (et la hauteur affecte la largeur des rectangles)
    if (!rectangles.empty())
    {
        updateContentSize();
    }
    else
    {
        layoutRectangles();
    }
}

// Version rétrocompatible (utilise dimensions par défaut)
void Zone4ScrollablePanel::addRectangle(std::unique_ptr<juce::Component> component)
{
    // Les dimensions sont ignorées car layoutRectangles() recalcule selon la hauteur disponible
    addRectangle(std::move(component), 0, 0);
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
    // Garder la hauteur actuelle (sera définie par le parent - Zone4ContentArea)
    auto currentHeight = getHeight();
    if (currentHeight <= 0) 
        currentHeight = 35; // Hauteur minimale de fallback
    
    // Calculer la largeur totale nécessaire avec les nouvelles dimensions
    const int totalWidth = calculateRequiredWidth();
    
    // Mise à jour de la taille avec la nouvelle largeur calculée
    setSize(totalWidth, currentHeight);
    
    // Relayouter après changement de taille pour appliquer les nouvelles positions
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
    {
        // Utiliser la largeur fixe
        width = ChordPanelConfig::CHORD_WIDTH_FIXED;
    }
    else
    {
        // Calculer selon le ratio
        width = static_cast<int>(height * ChordPanelConfig::CHORD_WIDTH_RATIO);
    }
    
    // Appliquer la largeur minimale
    return juce::jmax(width, ChordPanelConfig::CHORD_WIDTH_MIN);
}

int Zone4ScrollablePanel::calculateRequiredWidth() const
{
    if (rectangles.empty())
        return MIN_CONTENT_WIDTH;
    
    int totalHeight = getHeight();
    int rectangleHeight = totalHeight - SCROLLBAR_SPACE;
    int rectangleWidth = calculateChordWidth(rectangleHeight);
    
    // Calculer la largeur totale
    int totalWidth = static_cast<int>(rectangles.size()) * rectangleWidth 
                   + static_cast<int>(rectangles.size() - 1) * ChordPanelConfig::CHORD_SPACING
                   + ChordPanelConfig::CHORD_SPACING * 2;  // Marge droite
    
    return juce::jmax(totalWidth, MIN_CONTENT_WIDTH);
}