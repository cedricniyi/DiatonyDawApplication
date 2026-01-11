#pragma once

#include <JuceHeader.h>

/** @brief Configuration des dimensions des panneaux d'accords. */
namespace ChordPanelConfig
{
    constexpr int CHORD_WIDTH_FIXED = 90;     // Largeur fixe (0 = utiliser ratio)
    constexpr float CHORD_WIDTH_RATIO = 0.5f; // Ratio hauteur→largeur (si FIXED == 0)
    constexpr int CHORD_WIDTH_MIN = 50;       // Largeur minimale
    constexpr int CHORD_SPACING = 8;          // Espace entre les accords
}

/** @brief Informations d'un rectangle avec ses dimensions. */
struct RectangleInfo
{
    std::unique_ptr<juce::Component> component;
    int width;
    int height;
    
    RectangleInfo(std::unique_ptr<juce::Component> comp, int w, int h)
        : component(std::move(comp)), width(w), height(h) {}
};

/** @brief Panneau scrollable horizontal pour Zone4, gère une collection de panneaux d'accords. */
class Zone4ScrollablePanel : public juce::Component
{
public:
    Zone4ScrollablePanel();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void addRectangle(std::unique_ptr<juce::Component> component);
    void addRectangle(std::unique_ptr<juce::Component> component, int width, int height);
    void clearAllRectangles();
    int getNumRectangles() const;
    
    void updateContentSize();
    
private:
    std::vector<RectangleInfo> rectangles;
    
    static constexpr int MIN_CONTENT_WIDTH = 50;
    static constexpr int SCROLLBAR_SPACE = 10;
    
    void layoutRectangles();
    int calculateRequiredWidth() const;
    int calculateChordWidth(int height) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4ScrollablePanel)
};
