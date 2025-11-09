#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/OutlineTextButton.h"
#include "utils/FontManager.h"
#include "OverviewContentArea.h"

//==============================================================================
class OverviewArea : public ColoredPanel
{
public:
    OverviewArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Méthode pour obtenir la taille préférée
    juce::Rectangle<int> getPreferredSize() const;
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label      overviewLabel;
    OverviewContentArea contentArea;
    OutlineTextButton actionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewArea)
}; 