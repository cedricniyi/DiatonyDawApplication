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
    auto baseColor = getFunctionalColor();
    
    if (!isEnabled())
    {
        // Désactivé : couleurs ternes
        g.setColour(baseColor.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(baseColor.darker(0.4f).withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        g.setColour(juce::Colours::white.withAlpha(0.5f));
    }
    else if (selected)
    {
        // Sélectionné : fond vif + bordure blanche
        g.setColour(baseColor);
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colours::white);
        g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
        g.setColour(juce::Colours::white);
    }
    else
    {
        // Non sélectionné : couleur atténuée
        g.setColour(baseColor.withAlpha(0.6f));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(baseColor.darker(0.2f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
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
    juce::String text = DiatonyText::getChordDegreeName(degree);
    
    if (quality != Diatony::ChordQuality::Major && quality != Diatony::ChordQuality::Auto)
        text += DiatonyText::getChordQualitySymbol(quality);
    
    return text;
}

juce::Colour ChordChip::getFunctionalColor() const
{
    switch (degree)
    {
        case Diatony::ChordDegree::First:
        case Diatony::ChordDegree::Third:
        case Diatony::ChordDegree::Sixth:
            return juce::Colour(0xFF4A90A4);  // Tonique : Bleu
            
        case Diatony::ChordDegree::Second:
        case Diatony::ChordDegree::Fourth:
            return juce::Colour(0xFFF1C40F);  // Sous-Dominante : Or
            
        case Diatony::ChordDegree::Fifth:
        case Diatony::ChordDegree::Seventh:
            return juce::Colour(0xFFE74C3C);  // Dominante : Rouge
            
        default:
            return juce::Colours::grey;       // Autres degrés
    }
}

void ChordChip::mouseDown(const juce::MouseEvent&)
{
    if (!isEnabled())
        return;
    repaint();
}

void ChordChip::mouseUp(const juce::MouseEvent& event)
{
    if (!isEnabled())
        return;
    
    if (getLocalBounds().contains(event.getPosition()))
    {
        if (onClick)
            onClick(chordIndex);
    }
}
