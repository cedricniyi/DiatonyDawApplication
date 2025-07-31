#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Information d'un rectangle avec ses dimensions
 */
struct RectangleInfo
{
    std::unique_ptr<juce::Component> component;
    int width;
    int height;
    
    RectangleInfo(std::unique_ptr<juce::Component> comp, int w, int h)
        : component(std::move(comp)), width(w), height(h) {}
};

/**
 * Panneau scrollable horizontal pour Zone4 - version simplifiée de ScrollableContentPanel
 * Gère une collection horizontale de rectangles colorés
 */
class Zone4ScrollablePanel : public juce::Component
{
public:
    Zone4ScrollablePanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Gestion des rectangles
    void addRectangle(std::unique_ptr<juce::Component> component);
    void addRectangle(std::unique_ptr<juce::Component> component, int width, int height);
    void clearAllRectangles();
    int getNumRectangles() const;
    
    // Gestion de la taille du contenu
    void updateContentSize();
    
private:
    std::vector<RectangleInfo> rectangles;
    
    // Configuration des dimensions par défaut
    static constexpr int DEFAULT_RECTANGLE_WIDTH = 60;  // Non utilisé - largeur = hauteur × 2
    static constexpr int DEFAULT_RECTANGLE_HEIGHT = 35; // Non utilisé - les rectangles s'adaptent à la hauteur
    static constexpr int RECTANGLE_SPACING = 8;
    static constexpr int MIN_CONTENT_WIDTH = 50;
    static constexpr int SCROLLBAR_SPACE = 10; // Espace réservé pour la scrollbar
    
    void layoutRectangles();
    int calculateRequiredWidth() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4ScrollablePanel)
};