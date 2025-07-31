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
    StyledButton(const juce::String& buttonText,
                 juce::Colour normalColor,
                 juce::Colour highlightColor,
                 float fontSize = 14.0f,
                 FontManager::FontWeight fontWeight = FontManager::FontWeight::Regular);
    
    // Override simple pour dessiner avec notre font
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Font customFont;
};
