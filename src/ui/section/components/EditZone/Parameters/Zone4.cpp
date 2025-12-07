#include "Zone4.h"

Zone4::Zone4()
    : BaseZone("Chords"),
      addButton("+", 
                juce::Colours::blue,
                juce::Colours::darkblue, 
                18.0f, 
                FontManager::FontWeight::Bold)
{
    // Callback du bouton d'ajout
    addButton.onClick = [this]() {
        if (onChordAdded)
        {
            // Ajouter un accord avec des valeurs par défaut
            onChordAdded(
                Diatony::ChordDegree::First,
                Diatony::ChordQuality::Auto,
                Diatony::ChordState::Fundamental
            );
        }
    };
    addAndMakeVisible(addButton);
    
    // Zone de contenu scrollable
    addAndMakeVisible(contentAreaComponent);
}

void Zone4::resizeContent(const juce::Rectangle<int>& contentBounds)
{
    if (contentBounds.isEmpty())
        return;
    
    // Layout horizontal : zone de contenu à gauche, bouton à droite
    // (même disposition que OverviewActionArea)
    auto area = contentBounds;
    
    constexpr int BUTTON_WIDTH = 50;
    constexpr int SPACING = 8;
    
    // Bouton à droite (prend toute la hauteur)
    auto buttonArea = area.removeFromRight(BUTTON_WIDTH);
    area.removeFromRight(SPACING);
    
    // Zone de contenu scrollable à gauche
    contentAreaComponent.setBounds(area);
    addButton.setBounds(buttonArea);
}

void Zone4::syncWithProgression(const std::vector<juce::ValueTree>& chords)
{
    // Effacer tous les rectangles actuels
    contentAreaComponent.clearAllRectangles();
    
    // Ajouter autant de rectangles qu'il y a d'accords avec leurs valeurs connectées au ValueTree
    for (const auto& chordState : chords)
    {
        // Passer le ValueTree pour initialiser et connecter les comboboxes au modèle
        contentAreaComponent.addRectangle(chordState);
    }
}