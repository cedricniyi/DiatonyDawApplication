#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "ui/extra/StyledButton.h"
#include "utils/FontManager.h"
#include "WelcomeView.h"

//==============================================================================
class ProgressionArea : public ColoredPanel
{
public:
    ProgressionArea() 
        : ColoredPanel(juce::Colours::white)
    {
        // Ajouter la vue de bienvenue
        addAndMakeVisible(welcomeView);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        // Appliquer le padding à toute la zone
        auto area = getLocalBounds().reduced(20, 10);
        
        // Positionner la vue de bienvenue dans toute la zone disponible
        welcomeView.setBounds(area);
    }
    
private:
    WelcomeView welcomeView;
    juce::SharedResourcePointer<FontManager> fontManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionArea)
}; 