#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"

/**
 * KeySelectorComponent - Sélecteur de tonalité unifié
 * Remplace Zone1 (BaseNote) et Zone2 (Alteration)
 * 
 * Disposition en quinconce style piano :
 *      [C#] [D#]      [F#] [G#] [A#]   ← 5 boutons altérés
 *    [C] [D] [E] [F] [G] [A] [B]       ← 7 boutons naturels
 */
class KeySelectorComponent : public juce::Component
{
public:
    KeySelectorComponent();
    ~KeySelectorComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Callback quand une touche est sélectionnée
    // noteIndex: 0-11 (C=0, C#=1, D=2, etc.)
    std::function<void(int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)> onKeyChanged;
    
    // Setter pour synchronisation modèle → UI
    void setKey(int noteIndex);
    
    // Getter pour l'index actuellement sélectionné
    int getSelectedKey() const { return selectedNoteIndex; }

private:
    // Structure pour stocker les infos d'une touche
    struct KeyInfo {
        int noteIndex;              // 0-11
        Diatony::BaseNote base;
        Diatony::Alteration alt;
        juce::String label;
        bool isNatural;             // true pour les touches "blanches"
    };
    
    // Les 12 boutons (7 naturels + 5 altérés)
    std::array<std::unique_ptr<SelectableStyledButton>, 12> keyButtons;
    
    // Mapping noteIndex → KeyInfo
    std::array<KeyInfo, 12> keyInfos;
    
    // Index actuellement sélectionné (0-11)
    int selectedNoteIndex = 0;
    
    // Indices des touches naturelles (blanches) dans l'ordre d'affichage
    static constexpr std::array<int, 7> naturalIndices = { 0, 2, 4, 5, 7, 9, 11 };  // C, D, E, F, G, A, B
    
    // Indices des touches altérées (noires) dans l'ordre d'affichage
    static constexpr std::array<int, 5> sharpIndices = { 1, 3, 6, 8, 10 };  // C#, D#, F#, G#, A#
    
    // Méthodes privées
    void setupKeyInfos();
    void setupButtons();
    void layoutButtons();
    void selectKey(int noteIndex);
    
    // FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Couleurs - style discret comme Zone2
    juce::Colour normalColour = juce::Colours::lightgrey;
    juce::Colour selectedColour = juce::Colours::blue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySelectorComponent)
};

