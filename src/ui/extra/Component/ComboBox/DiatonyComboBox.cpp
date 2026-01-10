#include "DiatonyComboBox.h"

DiatonyComboBox::DiatonyComboBox()
{
    setLookAndFeel(&diatonyLookAndFeel);
    setTextWhenNothingSelected(juce::String::fromUTF8("Sélectionner..."));
    setTextWhenNoChoicesAvailable("Aucun choix");
    setColour(juce::ComboBox::textColourId, juce::Colour(60, 60, 60));
}

DiatonyComboBox::~DiatonyComboBox()
{
    setLookAndFeel(nullptr); // Détacher avant destruction
}

void DiatonyComboBox::adaptToBackgroundColour(juce::Colour backgroundColour)
{
    auto isLight = backgroundColour.getBrightness() > 0.5f;
    
    juce::Colour textColour;
    
    if (isLight)
    {
        textColour = juce::Colour(40, 40, 40);
        diatonyLookAndFeel.setBackgroundColour(backgroundColour.brighter(0.1f));
        diatonyLookAndFeel.setBorderColour(backgroundColour.darker(0.2f));
        diatonyLookAndFeel.setTextColour(textColour);
        diatonyLookAndFeel.setArrowColour(juce::Colour(80, 80, 80));
    }
    else
    {
        textColour = juce::Colour(220, 220, 220);
        diatonyLookAndFeel.setBackgroundColour(backgroundColour.brighter(0.15f));
        diatonyLookAndFeel.setBorderColour(backgroundColour.brighter(0.3f));
        diatonyLookAndFeel.setTextColour(textColour);
        diatonyLookAndFeel.setArrowColour(juce::Colour(180, 180, 180));
    }
    
    setColour(juce::ComboBox::textColourId, textColour);
    repaint();
}

void DiatonyComboBox::setShortTextForItem(int itemId, const juce::String& shortText)
{
    diatonyLookAndFeel.setShortTextForItem(itemId, shortText);
}

void DiatonyComboBox::enableShortDisplayMode(bool enabled)
{
    diatonyLookAndFeel.setShortDisplayMode(enabled);
    refreshDisplayedText();
}

void DiatonyComboBox::refreshDisplayedText()
{
    resized();
    repaint();
}
