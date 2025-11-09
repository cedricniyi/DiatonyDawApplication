#include "DiatonyComboBox.h"

//==============================================================================
DiatonyComboBox::DiatonyComboBox()
{
    // Appliquer le LookAndFeel personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Configuration par défaut
    setTextWhenNothingSelected(juce::String::fromUTF8("Sélectionner..."));
    setTextWhenNoChoicesAvailable("Aucun choix");
    
    // Définir la couleur du texte par défaut (gris foncé)
    setColour(juce::ComboBox::textColourId, juce::Colour(60, 60, 60));
}

DiatonyComboBox::~DiatonyComboBox()
{
    // Important : détacher le LookAndFeel avant destruction
    setLookAndFeel(nullptr);
}

void DiatonyComboBox::adaptToBackgroundColour(juce::Colour backgroundColour)
{
    // Adapter les couleurs du LookAndFeel selon la couleur de fond
    auto isLight = backgroundColour.getBrightness() > 0.5f;
    
    juce::Colour textColour;
    
    if (isLight)
    {
        // Fond clair : couleurs sombres pour le contraste
        textColour = juce::Colour(40, 40, 40);
        diatonyLookAndFeel.setBackgroundColour(backgroundColour.brighter(0.1f));
        diatonyLookAndFeel.setBorderColour(backgroundColour.darker(0.2f));
        diatonyLookAndFeel.setTextColour(textColour);
        diatonyLookAndFeel.setArrowColour(juce::Colour(80, 80, 80));
    }
    else
    {
        // Fond sombre : couleurs claires pour le contraste
        textColour = juce::Colour(220, 220, 220);
        diatonyLookAndFeel.setBackgroundColour(backgroundColour.brighter(0.15f));
        diatonyLookAndFeel.setBorderColour(backgroundColour.brighter(0.3f));
        diatonyLookAndFeel.setTextColour(textColour);
        diatonyLookAndFeel.setArrowColour(juce::Colour(180, 180, 180));
    }
    
    // Mettre à jour la couleur du texte du ComboBox lui-même
    setColour(juce::ComboBox::textColourId, textColour);
    
    // Forcer le repaint
    repaint();
}