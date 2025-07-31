#include "StyledButton.h"
#include "utils/FontManager.h"

//==============================================================================
StyledButton::StyledButton(const juce::String& buttonText,
                          juce::Colour normalColor,
                          juce::Colour highlightColor,
                          float fontSize,
                          FontManager::FontWeight fontWeight)
    : juce::TextButton(buttonText),
      customFont(juce::Font(fontManager->getSFProDisplay(fontSize, fontWeight)))
{
    // Couleurs du bouton
    setColour(juce::TextButton::buttonColourId,      normalColor);
    setColour(juce::TextButton::buttonOnColourId,    highlightColor);
    setColour(juce::TextButton::textColourOffId,     juce::Colours::white);
    setColour(juce::TextButton::textColourOnId,      juce::Colours::white);
    setColour(juce::ComboBox::outlineColourId,       juce::Colours::transparentBlack);
}

void StyledButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
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