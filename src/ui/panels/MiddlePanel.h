#pragma once

#include <JuceHeader.h>
#include "../extra/ColoredPanel.h"
#include "../extra/StyledButton.h"
#include "../../utils/FontManager.h"

//==============================================================================
class MiddlePanel : public ColoredPanel
{
public:
    MiddlePanel() 
        : ColoredPanel(juce::Colours::white),
          addSectionButton  (juce::String::fromUTF8("+ New Section"),
                             juce::Colour::fromString ("#ff27ae60"),
                             juce::Colour::fromString ("ff2ecc71"),
                             14.0f, FontManager::FontWeight::Medium)
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
    }
    
    void paint(juce::Graphics& g) override
    {
        ColoredPanel::paint(g);
        
        // Dessiner un rectangle bleu pour montrer l'espace restant sous le header
        auto area = getLocalBounds();
        auto headerArea = area.removeFromTop(50); // Même hauteur que dans resized()
        
        // Rectangle bleu pour l'espace restant
        g.setColour(juce::Colours::lightblue);
        g.fillRect(area);
        
        // Bordure pour délimiter clairement
        g.setColour(juce::Colours::blue);
        g.drawRect(area, 2);
        
        // Texte explicatif au centre de la zone restante
        g.setColour(juce::Colours::darkblue);
        g.setFont(16.0f);
        g.drawText("Zone disponible pour le contenu", area, juce::Justification::centred);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        // Header en haut : 50px de haut, SANS marges (toute la largeur)
        auto headerArea = area.removeFromTop(50);

        // Layout avec FlexBox pour le header
        juce::FlexBox fb;
        fb.flexDirection = juce::FlexBox::Direction::row;
        fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        fb.alignItems = juce::FlexBox::AlignItems::center;

        // Ajouter un peu de padding interne au header seulement
        auto paddedHeaderArea = headerArea.reduced(20, 10);

        // éléments flexibles
        fb.items.add (juce::FlexItem (headerLabel).withFlex (1.0f).withMinHeight(30));
        fb.items.add (juce::FlexItem (addSectionButton).withFlex (0.0f).withMinWidth(120).withMinHeight(30));

        fb.performLayout (paddedHeaderArea);
        
        // Le reste de l'espace (area) est automatiquement disponible pour d'autres composants
    }
    
private:

    juce::SharedResourcePointer<FontManager> fontManager;

    juce::Label      headerLabel;
    StyledButton     addSectionButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiddlePanel)
}; 