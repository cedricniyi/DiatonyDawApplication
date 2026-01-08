#pragma once

#include <JuceHeader.h>

// ═══════════════════════════════════════════════════════════════════════════════
// 🎛️ PARAMÈTRES DES PANNEAUX D'ACCORDS - Modifie ces valeurs !
// ═══════════════════════════════════════════════════════════════════════════════

namespace ChordPanelConfig
{
    // Largeur des panneaux d'accords
    // - CHORD_WIDTH_FIXED : largeur fixe en pixels (si > 0, ignore le ratio)
    // - CHORD_WIDTH_RATIO : largeur = hauteur * ratio (si WIDTH_FIXED == 0)
    constexpr int CHORD_WIDTH_FIXED = 90;     // ← Largeur fixe (0 = utiliser ratio)
    constexpr float CHORD_WIDTH_RATIO = 0.5f; // ← Ratio hauteur→largeur (si FIXED == 0)
    
    // Largeur minimale (toujours appliquée)
    constexpr int CHORD_WIDTH_MIN = 50;       // ← Largeur minimale
    
    // Espacement entre les panneaux
    constexpr int CHORD_SPACING = 8;          // ← Espace entre les accords
}

// ═══════════════════════════════════════════════════════════════════════════════

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
 * Panneau scrollable horizontal pour Zone4
 * Gère une collection horizontale de panneaux d'accords
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
    
    static constexpr int MIN_CONTENT_WIDTH = 50;
    static constexpr int SCROLLBAR_SPACE = 10;
    
    void layoutRectangles();
    int calculateRequiredWidth() const;
    int calculateChordWidth(int height) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4ScrollablePanel)
};