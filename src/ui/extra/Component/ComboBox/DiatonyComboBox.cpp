#include "DiatonyComboBox.h"

//==============================================================================
DiatonyComboBox::DiatonyComboBox()
{
    // Appliquer le LookAndFeel personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Configuration par défaut
    setTextWhenNothingSelected("Sélectionner...");
    setTextWhenNoChoicesAvailable("Aucun choix");
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
    
    if (isLight)
    {
        // Fond clair : couleurs sombres pour le contraste
        diatonyLookAndFeel.setBackgroundColour(backgroundColour.brighter(0.1f));
        diatonyLookAndFeel.setBorderColour(backgroundColour.darker(0.2f));
        diatonyLookAndFeel.setTextColour(juce::Colour(40, 40, 40));
        diatonyLookAndFeel.setArrowColour(juce::Colour(80, 80, 80));
    }
    else
    {
        // Fond sombre : couleurs claires pour le contraste
        diatonyLookAndFeel.setBackgroundColour(backgroundColour.brighter(0.15f));
        diatonyLookAndFeel.setBorderColour(backgroundColour.brighter(0.3f));
        diatonyLookAndFeel.setTextColour(juce::Colour(220, 220, 220));
        diatonyLookAndFeel.setArrowColour(juce::Colour(180, 180, 180));
    }
    
    // Forcer le repaint
    repaint();
}