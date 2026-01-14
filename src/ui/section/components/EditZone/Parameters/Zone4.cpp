#include "Zone4.h"

Zone4::Zone4()
    : BaseZone("Chords"),
      addButton("+", 
                juce::Colours::blue,
                juce::Colours::darkblue, 
                18.0f, 
                FontManager::FontWeight::Bold)
{
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
    
    // Propager le callback de suppression depuis Zone4ContentArea
    contentAreaComponent.onChordRemoved = [this](int chordIndex) {
        if (onChordRemoved)
            onChordRemoved(chordIndex);
    };
    
    addAndMakeVisible(addButton);
    addAndMakeVisible(contentAreaComponent);
}

void Zone4::resizeContent(const juce::Rectangle<int>& contentBounds)
{
    if (contentBounds.isEmpty())
        return;
    
    auto area = contentBounds;
    
    constexpr int BUTTON_WIDTH = 50;
    constexpr int SPACING = 8;
    
    auto buttonArea = area.removeFromRight(BUTTON_WIDTH);
    area.removeFromRight(SPACING);
    
    contentAreaComponent.setBounds(area);
    addButton.setBounds(buttonArea);
}

void Zone4::syncWithProgression(const std::vector<juce::ValueTree>& chords)
{
    contentAreaComponent.clearAllRectangles();
    
    // Ajouter autant de rectangles qu'il y a d'accords avec leurs valeurs connectées au ValueTree
    for (const auto& chordState : chords)
        contentAreaComponent.addRectangle(chordState);
}

