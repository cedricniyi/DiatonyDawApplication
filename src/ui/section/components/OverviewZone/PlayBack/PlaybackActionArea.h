#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"

//==============================================================================
class PlaybackActionArea : public ColoredPanel
{
public:
    PlaybackActionArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Méthode pour calculer la taille préférée du composant
    juce::Rectangle<int> getPreferredSize() const;
    
    // Accès aux boutons
    StyledButton& getPlayButton();
    StyledButton& getPauseButton();
    StyledButton& getStopButton();
    
private:
    StyledButton playButton;
    StyledButton pauseButton;
    StyledButton stopButton;
    juce::FlexBox flexBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaybackActionArea)
}; 