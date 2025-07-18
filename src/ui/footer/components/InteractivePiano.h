#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../../utils/FontManager.h"

//==============================================================================
class InteractivePiano : public ColoredPanel
{
public:
    InteractivePiano(juce::Colour color) 
        : ColoredPanel(color),
          fontManager()
    {
        // Texte de démonstration
        label.setText(juce::String::fromUTF8("🎹 Interactive Piano 🎹"), juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
        
        // Configurer la police
        auto fontOptions = fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Medium);
        label.setFont(juce::Font(fontOptions));
        
        addAndMakeVisible(label);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Appeler le paint de ColoredPanel pour le fond
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        label.setBounds(getLocalBounds());
    }
    
private:
    juce::Label label;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractivePiano)
}; 