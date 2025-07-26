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

void ScrollableContentPanel::addSmallPanel(std::unique_ptr<juce::Component> component)
{
    if (!component)
        return;
        
    addAndMakeVisible(*component);
    smallPanels.push_back(std::move(component));
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
        currentHeight = PANEL_HEIGHT;
    
    setSize(juce::jmax(totalWidth, MIN_CONTENT_WIDTH), currentHeight);
    
    // Relayouter après changement de taille
    resized();
}

void ScrollableContentPanel::layoutSmallPanels()
{
    if (smallPanels.empty())
        return;
        
    int x = 0;
    int y = (getHeight() - PANEL_HEIGHT) / 2; // Centrer verticalement
    
    for (auto& panel : smallPanels)
    {
        panel->setBounds(x, y, PANEL_WIDTH, PANEL_HEIGHT);
        x += PANEL_WIDTH + PANEL_SPACING;
    }
}

int ScrollableContentPanel::calculateRequiredWidth() const
{
    if (smallPanels.empty())
        return MIN_CONTENT_WIDTH;
        
    return static_cast<int>(smallPanels.size()) * (PANEL_WIDTH + PANEL_SPACING);
} 