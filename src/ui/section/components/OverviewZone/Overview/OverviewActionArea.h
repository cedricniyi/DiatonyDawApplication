#pragma once

#include <JuceHeader.h>
#include "ui/extra/Button/StyledButton.h"
#include "OverviewContentArea.h"

/** @brief Zone d'action de l'Overview avec ContentArea et bouton d'ajout. */
class OverviewActionArea : public juce::Component
{
public:
    OverviewActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Retourne le composant OverviewContentArea. */
    OverviewContentArea& getContentArea();
    
private:
    OverviewContentArea contentArea;
    StyledButton actionButton;
    
    // Style constants
    static constexpr float cornerRadius = 8.0f;
    static constexpr int borderThickness = 1;
    static constexpr int contentPadding = 10;  // Espacement interne pour les enfants

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
};
