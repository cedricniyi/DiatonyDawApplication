#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "Zone4ScrollablePanel.h"
#include "utils/FontManager.h"

//==============================================================================
/**
 * Zone de contenu pour Zone4 - version simplifiée d'OverviewContentArea
 * Gère l'affichage de rectangles avec gestion d'état vide/plein et viewport scrollable
 */
class Zone4ContentArea : public ColoredPanel
{
public:
    Zone4ContentArea();
    ~Zone4ContentArea() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Gestion du contenu
    void addRectangle();
    void clearAllRectangles();
    bool hasContent() const;
    
    // Configuration
    juce::Rectangle<int> getPreferredSize() const;
    
private:
    // Composants UI
    juce::Viewport viewport;
    std::unique_ptr<Zone4ScrollablePanel> scrollableContent;
    juce::Label emptyLabel;
    
    // Configuration
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 45;  // Plus de hauteur pour les panels
    static constexpr int HORIZONTAL_PADDING = 8;
    static constexpr int TOP_PADDING = 6;
    static constexpr int BOTTOM_PADDING = 0;     // Pas de padding en bas pour la scrollbar
    
    // Gestion des couleurs pour les rectangles
    int nextRectangleId = 1;
    juce::Array<juce::Colour> availableColours = {
        juce::Colours::lightblue,
        juce::Colours::lightgreen,
        juce::Colours::lightyellow,
        juce::Colours::lightpink,
        juce::Colours::lightcyan,
        juce::Colours::lightgrey
    };
    
    // Méthodes privées
    void setupViewport();
    void setupEmptyLabel();
    void updateVisibility();
    juce::Colour getNextColour();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4ContentArea)
};