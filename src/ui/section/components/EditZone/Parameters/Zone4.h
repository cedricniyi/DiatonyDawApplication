#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Zone/BaseZone.h"
#include "ui/extra/Button/StyledButton.h"
#include "zone4/Zone4ContentArea.h"
#include "model/DiatonyTypes.h"

/** @brief Zone 4 - Éditeur de progression d'accords, hérite de BaseZone. */
class Zone4 : public BaseZone
{
public:
    Zone4();
    ~Zone4() override = default;
    
    /** @brief Callback appelé quand un accord est ajouté. */
    std::function<void(Diatony::ChordDegree, Diatony::ChordQuality, Diatony::ChordState)> onChordAdded;
    
    /** @brief Callback appelé quand un accord doit être supprimé (index de l'accord). */
    std::function<void(int)> onChordRemoved;
    
    /** @brief Synchronise l'affichage avec les accords du modèle. */
    void syncWithProgression(const std::vector<juce::ValueTree>& chords);

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    StyledButton addButton;
    Zone4ContentArea contentAreaComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4)
};
