#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Zone/BaseZone.h"
#include "ui/extra/Button/StyledButton.h"
#include "zone4/Zone4ContentArea.h"
#include "model/DiatonyTypes.h"

/**
 * Zone 4 - Éditeur de progression d'accords
 * Hérite de BaseZone pour avoir le même style que KeyZone et ModeZone
 */
class Zone4 : public BaseZone
{
public:
    Zone4();
    ~Zone4() override = default;
    
    // Binding depuis SectionEditor (même pattern que KeyZone/ModeZone)
    std::function<void(Diatony::ChordDegree, Diatony::ChordQuality, Diatony::ChordState)> onChordAdded;
    
    // Synchronisation Modèle -> UI (afficher les accords existants)
    void syncWithProgression(const std::vector<juce::ValueTree>& chords);

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    // Composants UI
    StyledButton addButton;
    Zone4ContentArea contentAreaComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Zone4)
};