// StyledButton.h
#pragma once
#include <JuceHeader.h>
#include "utils/FontManager.h"

/// Un TextButton réutilisable, où l'on ne précise que le texte et 2 couleurs
class StyledButton : public juce::TextButton
{
public:
    /** 
     * Constructeur simple avec possibilité de personnaliser la font
     */
    StyledButton (const juce::String& buttonText,
                  juce::Colour normalColor,
                  juce::Colour highlightColor,
                  float fontSize = 14.0f,
                  FontManager::FontWeight fontWeight = FontManager::FontWeight::Regular)
      : juce::TextButton (buttonText),
        customFont(juce::Font(fontManager->getSFProDisplay(fontSize, fontWeight)))
    {
        // Couleurs du bouton
        setColour (juce::TextButton::buttonColourId,      normalColor);
        setColour (juce::TextButton::buttonOnColourId,    highlightColor);
        setColour (juce::TextButton::textColourOffId,     juce::Colours::white);
        setColour (juce::TextButton::textColourOnId,      juce::Colours::white);
        setColour (juce::ComboBox::outlineColourId,       juce::Colours::transparentBlack);
    }
    
    // Override simple pour dessiner avec notre font
    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        // Dessiner le fond du bouton (sans texte)
        auto bounds = getLocalBounds().toFloat();
        
        // Couleur du fond selon l'état
        juce::Colour backgroundColour = findColour(juce::TextButton::buttonColourId);
        if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
            backgroundColour = findColour(juce::TextButton::buttonOnColourId);
            
        g.setColour(backgroundColour);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Dessiner uniquement notre texte avec la font personnalisée
        g.setColour(findColour(shouldDrawButtonAsDown ? juce::TextButton::textColourOnId 
                                                      : juce::TextButton::textColourOffId));
        g.setFont(customFont);
        g.drawText(getButtonText(), bounds.toNearestInt(), juce::Justification::centred, true);
    }

private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Font customFont;
};
