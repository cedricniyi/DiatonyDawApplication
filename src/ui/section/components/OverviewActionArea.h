#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../../utils/FontManager.h"

//==============================================================================
class OverviewActionArea : public ColoredPanel
{
public:
    OverviewActionArea() 
        : ColoredPanel(juce::Colour::fromString("#fff0faff"))
    {
        // Définir l'alpha pour que le composant en dessous soit visible
        setAlpha(0.85f); 
        
        // Label "Overview & Actions"
        headerLabel.setText(juce::String::fromUTF8("Overview & Actions"), juce::dontSendNotification);
        headerLabel.setJustificationType (juce::Justification::centredLeft);
        headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(18.0f, FontManager::FontWeight::Bold);
        headerLabel.setFont(juce::Font(fontOptions));
        
        addAndMakeVisible (headerLabel);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Dessiner le fond coloré via ColoredPanel
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20, 10);
        headerLabel.setBounds(area);
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label headerLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverviewActionArea)
}; 