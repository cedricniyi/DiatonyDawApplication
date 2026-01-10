#include "SelectableStyledButton.h"

SelectableStyledButton::SelectableStyledButton(const juce::String& buttonText,
                                             juce::Colour normalColor,
                                             juce::Colour selectedColor,
                                             float fontSize,
                                             FontManager::FontWeight fontWeight)
    : StyledButton(buttonText, normalColor, selectedColor, fontSize, fontWeight),
      isSelected(false),
      normalColour(normalColor),
      selectedColour(selectedColor)
{
    updateButtonColours();
}

void SelectableStyledButton::setSelected(bool selected)
{
    if (isSelected != selected)
    {
        isSelected = selected;
        updateButtonColours();
        repaint();
    }
}

bool SelectableStyledButton::getSelected() const
{
    return isSelected;
}

void SelectableStyledButton::setNormalColour(const juce::Colour& colour)
{
    normalColour = colour;
    updateButtonColours();
}

void SelectableStyledButton::setSelectedColour(const juce::Colour& colour)
{
    selectedColour = colour;
    updateButtonColours();
}

juce::Colour SelectableStyledButton::getNormalColour() const
{
    return normalColour;
}

juce::Colour SelectableStyledButton::getSelectedColour() const
{
    return selectedColour;
}

void SelectableStyledButton::updateButtonColours()
{
    if (isSelected)
    {
        // État sélectionné : fond bleu, texte blanc
        setColour(juce::TextButton::buttonColourId, selectedColour);
        setColour(juce::TextButton::buttonOnColourId, selectedColour.brighter(0.15f));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }
    else
    {
        // État normal : fond gris, texte noir
        setColour(juce::TextButton::buttonColourId, normalColour);
        setColour(juce::TextButton::buttonOnColourId, normalColour.brighter(0.15f));
        setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    }
}