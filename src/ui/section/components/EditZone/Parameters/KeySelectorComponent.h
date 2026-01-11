#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"

/**
 * @brief Sélecteur de tonalité unifié avec disposition en quinconce style piano.
 *
 * Ligne du haut : 5 altérées (C#, D#, F#, G#, A#)
 * Ligne du bas : 7 naturelles (C, D, E, F, G, A, B)
 */
class KeySelectorComponent : public juce::Component
{
public:
    KeySelectorComponent();
    ~KeySelectorComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Callback quand une touche est sélectionnée (noteIndex: 0-11 [C=0, C#=1, D=2, etc.]). */
    std::function<void(int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)> onKeyChanged;
    
    void setKey(int noteIndex);
    int getSelectedKey() const { return selectedNoteIndex; }

private:
    // Structure pour stocker les infos d'une touche
    struct KeyInfo {
        int noteIndex;
        Diatony::BaseNote base;
        Diatony::Alteration alt;
        juce::String label;
        bool isNatural;
    };
    
    std::array<std::unique_ptr<SelectableStyledButton>, 12> keyButtons;
    std::array<KeyInfo, 12> keyInfos;
    
    int selectedNoteIndex = 0;
    
    // C, D, E, F, G, A, B
    static constexpr std::array<int, 7> naturalIndices = { 0, 2, 4, 5, 7, 9, 11 };
    // C#, D#, F#, G#, A#
    static constexpr std::array<int, 5> sharpIndices = { 1, 3, 6, 8, 10 };
    
    void setupKeyInfos();
    void setupButtons();
    void layoutButtons();
    void selectKey(int noteIndex);
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Colour normalColour = juce::Colours::lightgrey;
    juce::Colour selectedColour = juce::Colours::blue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySelectorComponent)
};
