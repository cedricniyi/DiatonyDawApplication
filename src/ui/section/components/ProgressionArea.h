#pragma once

#include <JuceHeader.h>
#include "ui/extra/ColoredPanel.h"
#include "ui/extra/StyledButton.h"
#include "utils/FontManager.h"

//==============================================================================
class ProgressionArea : public ColoredPanel
{
public:
    ProgressionArea() 
        : ColoredPanel(juce::Colours::white)
    {
       
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
        
        // Le reste de l'espace est disponible pour le contenu futur
        // contentArea.setBounds(area); // À implémenter plus tard
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionArea)
}; 