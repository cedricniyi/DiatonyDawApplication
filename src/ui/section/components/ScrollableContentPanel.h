#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"

//==============================================================================
/**
 * Panneau de contenu scrollable gérant une collection horizontale de petits panels colorés
 */
class ScrollableContentPanel : public juce::Component
{
public:
    ScrollableContentPanel()
    {
    }
    
    void paint(juce::Graphics& g) override
    {
        // Le contenu est géré par les panels enfants
        // Pas de rendu direct nécessaire ici
    }
    
    void resized() override
    {
        layoutSmallPanels();
    }
    
    // Gestion des panels
    void addSmallPanel(std::unique_ptr<ColoredPanel> panel)
    {
        if (!panel)
            return;
            
        addAndMakeVisible(*panel);
        smallPanels.push_back(std::move(panel));
        updateContentSize();
    }
    
    void clearAllPanels()
    {
        smallPanels.clear();
        updateContentSize();
    }
    
    int getNumPanels() const
    {
        return static_cast<int>(smallPanels.size());
    }
    
    // Gestion de la taille du contenu
    void updateContentSize()
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
    
private:
    std::vector<std::unique_ptr<ColoredPanel>> smallPanels;
    
    // Configuration des dimensions
    static constexpr int PANEL_WIDTH = 40;
    static constexpr int PANEL_HEIGHT = 25;
    static constexpr int PANEL_SPACING = 5;
    static constexpr int MIN_CONTENT_WIDTH = 50;
    
    void layoutSmallPanels()
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
    
    int calculateRequiredWidth() const
    {
        if (smallPanels.empty())
            return MIN_CONTENT_WIDTH;
            
        return static_cast<int>(smallPanels.size()) * (PANEL_WIDTH + PANEL_SPACING);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollableContentPanel)
}; 