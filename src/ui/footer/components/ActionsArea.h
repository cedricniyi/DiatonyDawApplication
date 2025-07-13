#pragma once

#include <JuceHeader.h>
#include "../../extra/ColoredPanel.h"
#include "../../../utils/FontManager.h"
#include "ActionsContentArea.h"

//==============================================================================
class ActionsArea : public ColoredPanel
{
public:
    ActionsArea() 
        : ColoredPanel(juce::Colours::white),
          contentArea()
    {
        // Label "Actions"
        headerLabel.setText(juce::String::fromUTF8("Actions"), juce::dontSendNotification);
        headerLabel.setJustificationType (juce::Justification::centredLeft);
        headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
        headerLabel.setFont(juce::Font(fontOptions));
        
        addAndMakeVisible (headerLabel);
        addAndMakeVisible (contentArea);
    }
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
    }
    
    void resized() override
    {
        // Appliquer le padding à toute la zone
        auto area = getLocalBounds().reduced(20, 10);

        // Calculer la hauteur nécessaire pour le header
        auto headerHeight = juce::jmax(30, static_cast<int>(headerLabel.getFont().getHeight() + 10)); // hauteur du texte + marge
        auto headerArea = area.removeFromTop(headerHeight);
        
        // Positionner le label "Actions"
        headerLabel.setBounds(headerArea);

        int headerBottomMargin = 8;
        area.removeFromTop (headerBottomMargin);
        
        // Le reste de l'espace va au contentArea
        contentArea.setBounds(area);
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label headerLabel;
    ActionsContentArea contentArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActionsArea)
}; 