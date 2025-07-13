#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"
#include "../extra/StyledButton.h"
#include "../../utils/FontManager.h"

class TopPanel : public ColoredPanel
{
public:
    TopPanel()
        : ColoredPanel (juce::Colours::white),
          settingsButton (juce::String::fromUTF8("⚙️ Settings"),
                          juce::Colour::fromString ("ff2980b9"),
                          juce::Colour::fromString ("ff3498db"),
                          14.0f, FontManager::FontWeight::Medium),
          historyButton  (juce::String::fromUTF8("📋 History"),
                          juce::Colour::fromString ("ff2980b9"),
                          juce::Colour::fromString ("ff3498db"),
                          14.0f, FontManager::FontWeight::Medium)
    {
        // Label gauche
        mainLabel.setText(juce::String::fromUTF8("DiatonyDAW – Prototype"),juce::dontSendNotification);
        mainLabel.setJustificationType (juce::Justification::centredLeft);
        mainLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Utiliser les fonts SF Pro via SharedResourcePointer
        mainLabel.setFont(fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold));
       
        addAndMakeVisible (mainLabel);

        // Boutons droite (fonts déjà configurées dans le constructeur)
        addAndMakeVisible (settingsButton);
        addAndMakeVisible (historyButton);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced (20, 10);

        // Positionner le label à gauche
        auto labelWidth = area.getWidth() / 2;
        mainLabel.setBounds (area.removeFromLeft (labelWidth));

        // FlexBox pour les deux boutons à droite
        juce::FlexBox buttonsFlex;
        buttonsFlex.flexDirection = juce::FlexBox::Direction::row;
        buttonsFlex.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
        buttonsFlex.alignItems    = juce::FlexBox::AlignItems::center;
        buttonsFlex.items = {
            juce::FlexItem (settingsButton).withMinWidth (100).withMinHeight (30)
                                            .withMargin ({ 0, 5, 0, 0 }),
            juce::FlexItem (historyButton).withMinWidth (100).withMinHeight (30)
        };

        buttonsFlex.performLayout (area);
    }

    void paint (juce::Graphics& g) override
    {
        // Fond blanc via ColoredPanel
        ColoredPanel::paint (g);
    }

private:
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Label      mainLabel;
    StyledButton     settingsButton;
    StyledButton     historyButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopPanel)
};
