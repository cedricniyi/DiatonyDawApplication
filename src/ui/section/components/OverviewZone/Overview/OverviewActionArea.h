#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/OutlineTextButton.h"
#include "OverviewContentArea.h"

//==============================================================================
/**
 * Zone d'action de l'Overview - Contient directement le ContentArea et le bouton
 * (Structure aplatie, sans le niveau intermédiaire OverviewArea)
 */
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Accès au composant OverviewContentArea
    OverviewContentArea& getContentArea();
    
private:
    OverviewContentArea contentArea;
    OutlineTextButton actionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
};
