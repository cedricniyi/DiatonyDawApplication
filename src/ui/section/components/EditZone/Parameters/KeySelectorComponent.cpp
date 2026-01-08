#include "KeySelectorComponent.h"

KeySelectorComponent::KeySelectorComponent()
{
    setupKeyInfos();
    setupButtons();
}

KeySelectorComponent::~KeySelectorComponent()
{
}

void KeySelectorComponent::setupKeyInfos()
{
    // Mapping des 12 demi-tons avec BaseNote, Alteration et labels
    // noteIndex 0-11 correspond à C, C#, D, D#, E, F, F#, G, G#, A, A#, B
    
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
        
        // Callback de sélection
        keyButtons[i]->onClick = [this, i]() {
            selectKey(i);
        };
        
        addAndMakeVisible(*keyButtons[i]);
    }
    
    // Sélectionner C par défaut
    selectKey(0);
}

void KeySelectorComponent::paint(juce::Graphics& g)
{
    // Fond transparent - pas de dessin
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
    
    // Calculer la largeur de bouton pour les naturels (7 boutons)
    int availableWidth = bounds.getWidth() - (6 * BUTTON_SPACING);
    int btnWidth = availableWidth / 7;
    
    // Centrer verticalement les deux lignes
    int totalContentHeight = (BUTTON_HEIGHT * 2) + ROW_SPACING;
    int startY = (bounds.getHeight() - totalContentHeight) / 2;
    
    int topRowY = startY;
    int bottomRowY = startY + BUTTON_HEIGHT + ROW_SPACING;
    
    // === Ligne du bas : 7 touches naturelles (C D E F G A B) ===
    int x = bounds.getX();
    for (int i = 0; i < 7; ++i)
    {
        int noteIdx = naturalIndices[i];
        keyButtons[noteIdx]->setBounds(x, bottomRowY, btnWidth, BUTTON_HEIGHT);
        x += btnWidth + BUTTON_SPACING;
    }
    
    // === Ligne du haut : 5 touches altérées ===
    // Même espacement que les naturelles, avec un gap là où il n'y a pas de E#
    // Layout: [C#][D#]  gap  [F#][G#][A#]
    //         pos0 pos1      pos3 pos4 pos5 (aligné sur les naturelles)
    
    // Décalage horizontal pour centrer la ligne du haut (qui a moins de boutons)
    // On aligne C# au-dessus de C, D# au-dessus de D, etc.
    int sharpStartX = bounds.getX() + (btnWidth + BUTTON_SPACING) / 2;
    
    // Positions des altérées : 0=C#, 1=D#, (2=gap), 3=F#, 4=G#, 5=A#
    std::array<int, 5> sharpSlots = { 0, 1, 3, 4, 5 };
    
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
    
    // Mettre à jour l'état visuel de tous les boutons
    for (int i = 0; i < 12; ++i)
    {
        keyButtons[i]->setSelected(i == noteIndex);
    }
    
    // Notifier le callback
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
        return;  // Déjà sélectionné
    
    selectedNoteIndex = noteIndex;
    
    // Mettre à jour l'état visuel sans déclencher le callback
    for (int i = 0; i < 12; ++i)
    {
        keyButtons[i]->setSelected(i == noteIndex);
    }
}

