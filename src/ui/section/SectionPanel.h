#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"
#include "../extra/StyledButton.h"
#include "../../utils/FontManager.h"
#include "components/ContentAreaPanel.h"

//==============================================================================
class SectionPanel : public ColoredPanel
{
public:
    SectionPanel() 
        : ColoredPanel(juce::Colours::white),
          addSectionButton  (juce::String::fromUTF8("+ New Section"),
                             juce::Colour::fromString ("#ff27ae60"),
                             juce::Colour::fromString ("ff2ecc71"),
                             14.0f, FontManager::FontWeight::Medium),
          contentArea()
    {
        // Label gauche
        headerLabel.setText(juce::String::fromUTF8("Piece sections"), juce::dontSendNotification);
        headerLabel.setJustificationType (juce::Justification::centredLeft);
        headerLabel.setColour (juce::Label::textColourId, juce::Colours::black);
        
        // Correction : Convertir FontOptions en Font
        auto fontOptions = fontManager->getSFProDisplay(24.0f, FontManager::FontWeight::Bold);
        headerLabel.setFont(juce::Font(fontOptions));
        
        // Boutons droite (fonts déjà configurées dans le constructeur)
        addAndMakeVisible (headerLabel);    
        addAndMakeVisible (addSectionButton);
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
        
        // Layout avec FlexBox pour le header
        juce::FlexBox fb;
        fb.flexDirection = juce::FlexBox::Direction::row;
        fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        fb.alignItems = juce::FlexBox::AlignItems::center;
        fb.items.add (juce::FlexItem (headerLabel).withFlex (1.0f).withMinHeight(30));
        fb.items.add (juce::FlexItem (addSectionButton).withFlex (0.0f).withMinWidth(120).withMinHeight(30));
        fb.performLayout (headerArea);

        int headerBottomMargin = 8;
        area.removeFromTop (headerBottomMargin);
        
        // Le reste de l'espace va au contentArea
        contentArea.setBounds(area);
    }
    
private:

    juce::SharedResourcePointer<FontManager> fontManager;

    juce::Label      headerLabel;
    StyledButton     addSectionButton;
    ContentAreaPanel contentArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionPanel)
}; 