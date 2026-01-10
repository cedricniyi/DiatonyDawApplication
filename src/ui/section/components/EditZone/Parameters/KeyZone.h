#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "KeySelectorComponent.h"

/** @brief Zone de sélection de tonalité, contient KeySelectorComponent dans un cadre BaseZone. */
class KeyZone : public BaseZone
{
public:
    KeyZone();
    
    /** @brief Expose le callback du KeySelector. */
    std::function<void(int noteIndex, Diatony::BaseNote base, Diatony::Alteration alt)>& getOnKeyChanged();
    
    void setKey(int noteIndex);
    int getSelectedKey() const;

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    KeySelectorComponent keySelector;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyZone)
};
