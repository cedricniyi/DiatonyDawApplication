#include "KeySelectorComponent.h"

KeySelectorComponent::KeySelectorComponent()
{
    setupKeyInfos();
    setupButtons();
}

KeySelectorComponent::~KeySelectorComponent() {}

void KeySelectorComponent::setupKeyInfos()
{
    // Mapping des 12 demi-tons : C=0, C#=1, D=2, D#=3, E=4, F=5, F#=6, G=7, G#=8, A=9, A#=10, B=11
    keyInfos[0]  = { 0,  Diatony::BaseNote::C, Diatony::Alteration::Natural, "C",       true  };
    keyInfos[1]  = { 1,  Diatony::BaseNote::C, Diatony::Alteration::Sharp,   "C#/Db",   false };
    keyInfos[2]  = { 2,  Diatony::BaseNote::D, Diatony::Alteration::Natural, "D",       true  };
    keyInfos[3]  = { 3,  Diatony::BaseNote::D, Diatony::Alteration::Sharp,   "D#/Eb",   false };
    keyInfos[4]  = { 4,  Diatony::BaseNote::E, Diatony::Alteration::Natural, "E",       true  };
    keyInfos[5]  = { 5,  Diatony::BaseNote::F, Diatony::Alteration::Natural, "F",       true  };
    keyInfos[6]  = { 6,  Diatony::BaseNote::F, Diatony::Alteration::Sharp,   "F#/Gb",   false };
    keyInfos[7]  = { 7,  Diatony::BaseNote::G, Diatony::Alteration::Natural, "G",       true  };
    keyInfos[8]  = { 8,  Diatony::BaseNote::G, Diatony::Alteration::Sharp,   "G#/Ab",   false };
    keyInfos[9]  = { 9,  Diatony::BaseNote::A, Diatony::Alteration::Natural, "A",       true  };
    keyInfos[10] = { 10, Diatony::BaseNote::A, Diatony::Alteration::Sharp,   "A#/Bb",   false };
    keyInfos[11] = { 11, Diatony::BaseNote::B, Diatony::Alteration::Natural, "B",       true  };
}

void KeySelectorComponent::setupButtons()
{
    for (int i = 0; i < 12; ++i)
    {
        const auto& info = keyInfos[i];
        
        keyButtons[i] = std::make_unique<SelectableStyledButton>(
            info.label,
            normalColour,
            selectedColour,
            16.0f,
            FontManager::FontWeight::Medium
        );
        
        keyButtons[i]->onClick = [this, i]() {
            selectKey(i);
        };
        
        addAndMakeVisible(*keyButtons[i]);
    }
    
    selectKey(0);
}

void KeySelectorComponent::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void KeySelectorComponent::resized()
{
    layoutButtons();
}

void KeySelectorComponent::layoutButtons()
{
    auto bounds = getLocalBounds();
    if (bounds.isEmpty())
        return;
    
    constexpr int BUTTON_HEIGHT = 20;
    constexpr int BUTTON_SPACING = 4;
    constexpr int ROW_SPACING = 6;
    
    int availableWidth = bounds.getWidth() - (6 * BUTTON_SPACING);
    int btnWidth = availableWidth / 7;
    
    int totalContentHeight = (BUTTON_HEIGHT * 2) + ROW_SPACING;
    int startY = (bounds.getHeight() - totalContentHeight) / 2;
    
    int topRowY = startY;
    int bottomRowY = startY + BUTTON_HEIGHT + ROW_SPACING;
    
    // Ligne du bas : 7 touches naturelles (C D E F G A B)
    int x = bounds.getX();
    for (int i = 0; i < 7; ++i)
    {
        int noteIdx = naturalIndices[i];
        keyButtons[noteIdx]->setBounds(x, bottomRowY, btnWidth, BUTTON_HEIGHT);
        x += btnWidth + BUTTON_SPACING;
    }
    
    // Ligne du haut : 5 touches altérées, alignées en quinconce
    int sharpStartX = bounds.getX() + (btnWidth + BUTTON_SPACING) / 2;
    std::array<int, 5> sharpSlots = { 0, 1, 3, 4, 5 };  // Gap à la position 2 (pas de E#)
    
    for (int i = 0; i < 5; ++i)
    {
        int noteIdx = sharpIndices[i];
        int slot = sharpSlots[i];
        int sharpX = sharpStartX + slot * (btnWidth + BUTTON_SPACING);
        keyButtons[noteIdx]->setBounds(sharpX, topRowY, btnWidth, BUTTON_HEIGHT);
    }
}

void KeySelectorComponent::selectKey(int noteIndex)
{
    if (noteIndex < 0 || noteIndex > 11)
        return;
    
    selectedNoteIndex = noteIndex;
    
    for (int i = 0; i < 12; ++i)
        keyButtons[i]->setSelected(i == noteIndex);
    
    if (onKeyChanged)
    {
        const auto& info = keyInfos[noteIndex];
        onKeyChanged(noteIndex, info.base, info.alt);
    }
}

void KeySelectorComponent::setKey(int noteIndex)
{
    if (noteIndex < 0 || noteIndex > 11)
        return;
    
    if (selectedNoteIndex == noteIndex)
        return;
    
    selectedNoteIndex = noteIndex;
    
    for (int i = 0; i < 12; ++i)
        keyButtons[i]->setSelected(i == noteIndex);
}
