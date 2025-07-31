#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Information d'un panel avec ses dimensions
 */
struct PanelInfo
{
    std::unique_ptr<juce::Component> component;
    int width;
    int height;
    
    PanelInfo(std::unique_ptr<juce::Component> comp, int w, int h)
        : component(std::move(comp)), width(w), height(h) {}
};

/**
 * Panneau de contenu scrollable gérant une collection horizontale de petits composants
 * Version découplée : les dimensions sont fournies explicitement
 */
class ScrollableContentPanel : public juce::Component
{
public:
    ScrollableContentPanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Gestion des composants - accepte n'importe quel juce::Component
    void addSmallPanel(std::unique_ptr<juce::Component> component);  // Rétrocompatibilité
    void addSmallPanel(std::unique_ptr<juce::Component> component, int width, int height);  // Version découplée
    void clearAllPanels();
    int getNumPanels() const;
    
    // Gestion de la taille du contenu
    void updateContentSize();
    
private:
    std::vector<PanelInfo> smallPanels;  // Plus de std::vector<unique_ptr<Component>>
    
    // Configuration des dimensions par défaut (pour rétrocompatibilité)
    static constexpr int DEFAULT_PANEL_WIDTH = 40;
    static constexpr int DEFAULT_PANEL_HEIGHT = 25;
    static constexpr int PANEL_SPACING = 5;
    static constexpr int MIN_CONTENT_WIDTH = 50;
    
    void layoutSmallPanels();
    int calculateRequiredWidth() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollableContentPanel)
}; 