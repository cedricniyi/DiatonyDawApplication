#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"
#include "../extra/StyledButton.h"
#include "../../utils/FontManager.h"

class HeaderPanel : public ColoredPanel
{
public:
    HeaderPanel()
        : ColoredPanel (juce::Colours::white),
          dButton (juce::String::fromUTF8("D"),
                   juce::Colour::fromString ("ff808080"), // Gris
                   juce::Colour::fromString ("ff606060"), // Gris plus foncé au survol
                   14.0f, FontManager::FontWeight::Black)
    {
        // Label gauche
        mainLabel.setText(juce::String::fromUTF8("DiatonyDAWPlugin"),juce::dontSendNotification);
        mainLabel.setJustificationType (juce::Justification::centredLeft);
        mainLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
        mainLabel.setFont(juce::Font(fontOptions));
       
        addAndMakeVisible (mainLabel);

        // Bouton D à droite
        addAndMakeVisible (dButton);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced (20, 10);

        // 1) Mesurer la largeur exacte du texte avec GlyphArrangement
        juce::GlyphArrangement ga;
        ga.addLineOfText (mainLabel.getFont(),
                          mainLabel.getText(),
                          0, 0);
        auto labelWidth = static_cast<int>(std::ceil(ga.getBoundingBox(0, -1, false).getWidth()+10));

        // 2) Positionner le label à gauche avec sa largeur exacte
        mainLabel.setBounds(area.removeFromLeft(labelWidth));

        // 3) Positionner le bouton D à droite avec FlexBox (bouton carré)
        auto buttonSize = area.getHeight(); // Bouton carré basé sur la hauteur
        juce::FlexBox buttonFlex;
        buttonFlex.flexDirection = juce::FlexBox::Direction::row;
        buttonFlex.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
        buttonFlex.alignItems = juce::FlexBox::AlignItems::center;
        buttonFlex.items.add(juce::FlexItem(dButton).withMinWidth(buttonSize).withMinHeight(buttonSize));
        
        buttonFlex.performLayout(area);
    }

    void paint (juce::Graphics& g) override
    {
        // Fond blanc via ColoredPanel
        ColoredPanel::paint (g);
    }

private:
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Label      mainLabel;
    StyledButton     dButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
}; 