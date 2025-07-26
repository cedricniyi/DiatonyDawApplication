#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "../PlayBack/PlaybackActionArea.h"
#include "../GenerateAndMore/GenerationButtons.h"
#include "OverviewArea.h"

// Forward declarations  
class FontManager;

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Accès aux composants pour les callbacks
    GenerationButtons& getGenerationButtons();
    PlaybackActionArea& getPlaybackActionArea();
    OverviewArea& getOverviewArea();
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    PlaybackActionArea playbackActionArea;
    OverviewArea overviewArea;
    GenerationButtons generationButtons;
    juce::FlexBox flexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 