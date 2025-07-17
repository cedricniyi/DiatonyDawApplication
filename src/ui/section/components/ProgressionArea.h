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
        : ColoredPanel(juce::Colours::white),
          addSectionButton  (juce::String::fromUTF8("+ New Section"),
                             juce::Colour::fromString ("#ff27ae60"),
                             juce::Colour::fromString ("ff2ecc71"),
                             14.0f, FontManager::FontWeight::Medium)
    {
        // Label "Piece sections"
        headerLabel.setText(juce::String::fromUTF8("Piece sections"), juce::dontSendNotification);
        headerLabel.setJustificationType (juce::Justification::centredLeft);
        headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
        headerLabel.setFont(juce::Font(fontOptions));
        
        addAndMakeVisible (headerLabel);
        addAndMakeVisible (addSectionButton);
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

        // Calculer la hauteur nécessaire pour le header
        auto headerHeight = juce::jmax(30, static_cast<int>(headerLabel.getFont().getHeight() + 10)); // hauteur du texte + marge
        auto headerArea = area.removeFromTop(headerHeight);
        
        // Layout avec FlexBox pour le header
        juce::FlexBox headerFB;
        headerFB.flexDirection = juce::FlexBox::Direction::row;
        headerFB.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        headerFB.alignItems = juce::FlexBox::AlignItems::center;
        headerFB.items.add (juce::FlexItem (headerLabel).withFlex (1.0f).withMinHeight(30));
        headerFB.items.add (juce::FlexItem (addSectionButton).withFlex (0.0f).withMinWidth(120).withMinHeight(30));
        headerFB.performLayout (headerArea);

        int headerBottomMargin = 8;
        area.removeFromTop (headerBottomMargin);
        
        // Le reste de l'espace est disponible pour le contenu futur
        // contentArea.setBounds(area); // À implémenter plus tard
    }
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label headerLabel;
    StyledButton addSectionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressionArea)
}; 