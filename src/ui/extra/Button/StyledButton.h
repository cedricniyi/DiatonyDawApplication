#pragma once
#include <JuceHeader.h>
#include "utils/FontManager.h"

/** @brief TextButton avec couleurs et font personnalisées. */
class StyledButton : public juce::TextButton
{
public:
    StyledButton(const juce::String& buttonText,
                 juce::Colour normalColor,
                 juce::Colour highlightColor,
                 float fontSize = 14.0f,
                 FontManager::FontWeight fontWeight = FontManager::FontWeight::Regular);
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Font customFont;
};
