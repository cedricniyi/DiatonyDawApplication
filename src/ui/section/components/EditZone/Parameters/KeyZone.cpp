#include "KeyZone.h"

KeyZone::KeyZone()
    : BaseZone("Tonality")
{
    addAndMakeVisible(keySelector);
}

std::function<void(int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)>& KeyZone::getOnKeyChanged()
{
    return keySelector.onKeyChanged;
}

void KeyZone::setKey(int noteIndex)
{
    keySelector.setKey(noteIndex);
}

int KeyZone::getSelectedKey() const
{
    return keySelector.getSelectedKey();
}

void KeyZone::resizeContent(const juce::Rectangle<int>& contentBounds)
{
    // BaseZone gère déjà le padding - on utilise tout l'espace fourni
    keySelector.setBounds(contentBounds);
}

