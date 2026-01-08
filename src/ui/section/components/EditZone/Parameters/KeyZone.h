#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "KeySelectorComponent.h"

//==============================================================================
/**
 * Zone de sélection de tonalité (Key)
 * Contient le KeySelectorComponent dans un cadre BaseZone
 */
class KeyZone : public BaseZone
{
public:
    KeyZone();
    
    // Exposer le callback du KeySelector
    std::function<void(int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)>& getOnKeyChanged();
    
    // Setter pour synchronisation modèle → UI
    void setKey(int noteIndex);
    
    int getSelectedKey() const;

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    KeySelectorComponent keySelector;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyZone)
};

