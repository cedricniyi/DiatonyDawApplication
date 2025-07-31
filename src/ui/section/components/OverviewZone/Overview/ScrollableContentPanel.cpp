#include "ScrollableContentPanel.h"

//==============================================================================
ScrollableContentPanel::ScrollableContentPanel()
{
}

void ScrollableContentPanel::paint(juce::Graphics& g)
{
    // Le contenu est géré par les panels enfants
    // Pas de rendu direct nécessaire ici
}

void ScrollableContentPanel::resized()
{
    layoutSmallPanels();
}

// Version rétrocompatible (utilise dimensions par défaut)
void ScrollableContentPanel::addSmallPanel(std::unique_ptr<juce::Component> component)
{
    addSmallPanel(std::move(component), DEFAULT_PANEL_WIDTH, DEFAULT_PANEL_HEIGHT);
}

// Version découplée (dimensions explicites)
void ScrollableContentPanel::addSmallPanel(std::unique_ptr<juce::Component> component, int width, int height)
{
    if (!component)
        return;
        
    addAndMakeVisible(*component);
    smallPanels.emplace_back(std::move(component), width, height);
    updateContentSize();
}

void ScrollableContentPanel::clearAllPanels()
{
    smallPanels.clear();
    updateContentSize();
}

int ScrollableContentPanel::getNumPanels() const
{
    return static_cast<int>(smallPanels.size());
}

void ScrollableContentPanel::updateContentSize()
{
    const int totalWidth = calculateRequiredWidth();
    
    // S'assurer qu'on a une hauteur minimale
    auto currentHeight = getHeight();
    if (currentHeight <= 0) 
        currentHeight = DEFAULT_PANEL_HEIGHT;
    
    setSize(juce::jmax(totalWidth, MIN_CONTENT_WIDTH), currentHeight);
    
    // Relayouter après changement de taille
    resized();
}

void ScrollableContentPanel::layoutSmallPanels()
{
    if (smallPanels.empty())
        return;
        
    int x = 0;
    int y = (getHeight() - DEFAULT_PANEL_HEIGHT) / 2; // Centrer verticalement
    
    // ✅ SIMPLE : Pas de dynamic_cast, pas de logique métier !
    for (auto& panelInfo : smallPanels)
    {
        panelInfo.component->setBounds(x, y, panelInfo.width, panelInfo.height);
        x += panelInfo.width + PANEL_SPACING;
    }
}

int ScrollableContentPanel::calculateRequiredWidth() const
{
    if (smallPanels.empty())
        return MIN_CONTENT_WIDTH;
    
    int totalWidth = 0;
    
    // ✅ SIMPLE : Pas de dynamic_cast, pas de logique métier !
    for (const auto& panelInfo : smallPanels)
    {
        totalWidth += panelInfo.width + PANEL_SPACING;
    }
    
    return totalWidth;
} 