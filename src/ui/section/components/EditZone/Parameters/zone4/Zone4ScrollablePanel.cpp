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
    
    for (auto& rectInfo : rectangles)
    {
        // Contrôle automatique de la partie droite des InfoColoredPanel selon la hauteur
        bool rightSideVisible = rectangleHeight > 50;
        
        if (auto* infoPanel = dynamic_cast<InfoColoredPanel*>(rectInfo.component.get()))
        {
            if (rectangleHeight <= 50)
            {
                // Hauteur trop petite : masquer la partie droite
                infoPanel->hideRightSide();
            }
            else
            {
                // Hauteur suffisante : afficher la partie droite
                infoPanel->showRightSidePanel();
            }
        }
        
        // Largeur adaptative selon la hauteur ET la visibilité de la partie droite :
        // - Si hauteur <= 50px (mode réduit, zone gauche seule) : largeur = hauteur × 5.5 (plus large pour numéro + combobox)
        // - Si hauteur <= 80px (mode complet, petit) : largeur = hauteur × 4
        // - Sinon (mode complet, normal) : largeur = hauteur × 3
        int rectangleWidth;
        if (rectangleHeight <= 50)
        {
            rectangleWidth = static_cast<int>(rectangleHeight * 5.5f); // Mode réduit : plus large pour numérotation + combobox
        }
        else if (rectangleHeight <= 80)
        {
            rectangleWidth = rectangleHeight * 4;
        }
        else
        {
            rectangleWidth = rectangleHeight * 3;
        }
        
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
    
    // Largeur adaptative selon la hauteur (même logique que dans layoutRectangles())
    // - Si hauteur <= 50px (mode réduit, zone gauche seule) : largeur = hauteur × 5.5 (plus large pour numéro + combobox)
    // - Si hauteur <= 80px (mode complet, petit) : largeur = hauteur × 4
    // - Sinon (mode complet, normal) : largeur = hauteur × 3
    int rectangleWidth;
    if (rectangleHeight <= 50)
    {
        rectangleWidth = static_cast<int>(rectangleHeight * 5.5f); // Mode réduit : plus large pour numérotation + combobox
    }
    else if (rectangleHeight <= 80)
    {
        rectangleWidth = rectangleHeight * 4;
    }
    else
    {
        rectangleWidth = rectangleHeight * 3;
    }
    
    // Calculer la largeur totale nécessaire
    for (size_t i = 0; i < rectangles.size(); ++i)
    {
        totalWidth += rectangleWidth;
        if (i < rectangles.size() - 1) // Espacement entre les rectangles, pas après le dernier
        {
            totalWidth += RECTANGLE_SPACING;
        }
    }
    
    // Ajouter une marge de sécurité pour s'assurer que le dernier panneau soit entièrement visible
    totalWidth += RECTANGLE_SPACING * 2; // Marge droite pour révélation complète
    
    return juce::jmax(totalWidth, MIN_CONTENT_WIDTH);
}