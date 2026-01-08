#pragma once

#include <JuceHeader.h>
#include "ui/extra/Button/StyledButton.h"
#include "OverviewContentArea.h"

//==============================================================================
/**
 * Zone d'action de l'Overview - Contient directement le ContentArea et le bouton
 * Dessin custom style BaseZone (fond semi-transparent + contour)
 */
class OverviewActionArea : public juce::Component
{
public:
    OverviewActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Accès au composant OverviewContentArea
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
