// StyledButton.h
#pragma once
#include <JuceHeader.h>

/// Un TextButton réutilisable, où l’on ne précise que le texte et 2 couleurs
class StyledButton : public juce::TextButton
{
public:
    /** 
     * @param buttonText   Le libellé du bouton
     * @param normalColor  Couleur de fond au repos
     * @param highlightColor Couleur de fond au survol/clic
     */
    StyledButton (const juce::String& buttonText,
                  juce::Colour normalColor,
                  juce::Colour highlightColor)
      : juce::TextButton (buttonText)
    {
        // Couleurs du bouton
        setColour (juce::TextButton::buttonColourId,      normalColor);
        setColour (juce::TextButton::buttonOnColourId,    highlightColor);
        // Couleur du texte, toujours blanc ici
        setColour (juce::TextButton::textColourOffId,     juce::Colours::white);
        setColour (juce::TextButton::textColourOnId,      juce::Colours::white);
        // Suppression du contour si jamais utilisé dans un ComboBox-like
        setColour (juce::ComboBox::outlineColourId,       juce::Colours::transparentBlack);

        // // Optionnel : coins arrondis, padding, etc.
        // setColour (juce::TextButton::outlineColourId,     juce::Colours::transparentBlack);
        // setButtonText (buttonText);
    }
};
