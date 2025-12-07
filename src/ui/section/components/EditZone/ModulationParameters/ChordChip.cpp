#include "ChordChip.h"

ChordChip::ChordChip(int chordIndex, Diatony::ChordDegree degree, Diatony::ChordQuality quality)
    : chordIndex(chordIndex)
    , degree(degree)
    , quality(quality)
{
    setOpaque(false);
}

void ChordChip::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    
    // Couleur de fond basée sur la fonction tonale
    auto baseColor = getFunctionalColor();
    
    // Si désactivé, couleurs plus ternes
    if (!isEnabled())
    {
        g.setColour(baseColor.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds, 4.0f);
        
        g.setColour(baseColor.darker(0.4f).withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        
        // Texte grisé
        g.setColour(juce::Colours::white.withAlpha(0.5f));
    }
    // Si sélectionné et activé, fond plus vif + bordure
    else if (selected)
    {
        g.setColour(baseColor);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Bordure blanche pour la sélection
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
        
        // Texte blanc
        g.setColour(juce::Colours::white);
    }
    else
    {
        // Non sélectionné mais activé : couleur atténuée
        g.setColour(baseColor.withAlpha(0.6f));
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Bordure subtile
        g.setColour(baseColor.darker(0.2f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        
        // Texte blanc
        g.setColour(juce::Colours::white);
    }
    
    auto font = juce::Font(fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Bold));
    g.setFont(font);
    g.drawText(getDisplayText(), bounds, juce::Justification::centred, false);
}

void ChordChip::setSelected(bool newSelected)
{
    if (selected != newSelected)
    {
        selected = newSelected;
        repaint();
    }
}

void ChordChip::setChordData(int newIndex, Diatony::ChordDegree newDegree, Diatony::ChordQuality newQuality)
{
    chordIndex = newIndex;
    degree = newDegree;
    quality = newQuality;
    repaint();
}

juce::String ChordChip::getDisplayText() const
{
    // Degré en chiffres romains
    juce::String text = DiatonyText::getChordDegreeName(degree);
    
    // Ajouter le symbole de qualité si différent de Major
    if (quality != Diatony::ChordQuality::Major && quality != Diatony::ChordQuality::Auto)
    {
        text += DiatonyText::getChordQualitySymbol(quality);
    }
    
    return text;
}

juce::Colour ChordChip::getFunctionalColor() const
{
    // Même logique que InfoColoredPanel::getFunctionalStripColor()
    // Basé sur le degré pour déterminer la fonction tonale
    
    switch (degree)
    {
        case Diatony::ChordDegree::First:
        case Diatony::ChordDegree::Third:
        case Diatony::ChordDegree::Sixth:
            // Fonction Tonique : Bleu
            return juce::Colour(0xFF4A90A4);
            
        case Diatony::ChordDegree::Second:
        case Diatony::ChordDegree::Fourth:
            // Fonction Sous-Dominante : Or
            return juce::Colour(0xFFF1C40F);
            
        case Diatony::ChordDegree::Fifth:
        case Diatony::ChordDegree::Seventh:
            // Fonction Dominante : Rouge
            return juce::Colour(0xFFE74C3C);
            
        default:
            // Autres degrés (V/II, V/III, bII, etc.) : Gris
            return juce::Colours::grey;
    }
}

void ChordChip::mouseDown(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    
    // Ignorer si désactivé
    if (!isEnabled())
        return;
    
    // Feedback visuel immédiat
    repaint();
}

void ChordChip::mouseUp(const juce::MouseEvent& event)
{
    // Ignorer si désactivé
    if (!isEnabled())
        return;
    
    if (getLocalBounds().contains(event.getPosition()))
    {
        if (onClick)
            onClick(chordIndex);
    }
}

