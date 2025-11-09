#pragma once

#include "StyledButton.h"

/**
 * Extension de StyledButton avec gestion de sélection similaire à CircularButton
 * Permet d'avoir des couleurs distinctes pour les états normal/sélectionné
 * sans affecter les autres usages de StyledButton dans l'application
 */
class SelectableStyledButton : public StyledButton
{
public:
    SelectableStyledButton(const juce::String& buttonText,
                          juce::Colour normalColor = juce::Colours::lightgrey,
                          juce::Colour selectedColor = juce::Colours::blue,
                          float fontSize = 14.0f,
                          FontManager::FontWeight fontWeight = FontManager::FontWeight::Regular);
    
    ~SelectableStyledButton() override = default;
    
    // Gestion de la sélection (comme CircularButton)
    void setSelected(bool selected);
    bool getSelected() const;
    
    // Configuration des couleurs pour les états normal/sélectionné
    void setNormalColour(const juce::Colour& colour);
    void setSelectedColour(const juce::Colour& colour);
    
    juce::Colour getNormalColour() const;
    juce::Colour getSelectedColour() const;

private:
    bool isSelected;
    juce::Colour normalColour;
    juce::Colour selectedColour;
    
    void updateButtonColours();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectableStyledButton)
};