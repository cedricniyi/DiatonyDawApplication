#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "ScrollableContentPanel.h"

// Forward declarations
class ButtonColoredPanel;

//==============================================================================
/**
 * Zone de contenu d'aperçu gérant l'affichage de progressions avec gestion d'état vide
 */
class OverviewContentArea : public ColoredPanel
{
public:
    OverviewContentArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Gestion du contenu
    void addSmallPanel();
    void clearAllPanels();
    bool hasContent() const;
    
    // Configuration
    juce::Rectangle<int> getPreferredSize() const;
    
    // Gestion de la sélection
    ButtonColoredPanel* getSelectedPanel() const;
    bool hasSelectedPanel() const;

private:
    // Composants UI
    juce::Viewport viewport;
    std::unique_ptr<ScrollableContentPanel> scrollableContent;
    juce::Label emptyLabel;
    
    // Configuration
    static constexpr int PREFERRED_WIDTH = 300;
    static constexpr int PREFERRED_HEIGHT = 35;
    static constexpr int CONTENT_MARGIN = 5;
    
    // Gestion des IDs de panels
    int nextPanelId = 1;
    
    // Gestion de la sélection
    ButtonColoredPanel* selectedPanel = nullptr;
    
    // Méthodes privées
    void setupViewport();
    void setupEmptyLabel();
    void updateVisibility();
    void onPanelClicked(ButtonColoredPanel* clickedPanel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewContentArea)
}; 