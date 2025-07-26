#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Panneau de contenu scrollable gérant une collection horizontale de petits composants
 */
class ScrollableContentPanel : public juce::Component
{
public:
    ScrollableContentPanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Gestion des composants - accepte n'importe quel juce::Component
    void addSmallPanel(std::unique_ptr<juce::Component> component);
    void clearAllPanels();
    int getNumPanels() const;
    
    // Gestion de la taille du contenu
    void updateContentSize();
    
private:
    std::vector<std::unique_ptr<juce::Component>> smallPanels;
    
    // Configuration des dimensions
    static constexpr int PANEL_WIDTH = 40;
    static constexpr int PANEL_HEIGHT = 25;
    static constexpr int PANEL_SPACING = 5;
    static constexpr int MIN_CONTENT_WIDTH = 50;
    
    void layoutSmallPanels();
    int calculateRequiredWidth() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollableContentPanel)
}; 