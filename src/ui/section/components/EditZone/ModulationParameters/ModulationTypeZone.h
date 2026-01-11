#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"
#include <array>
#include <memory>
#include <functional>

/** @brief Zone de sélection du type de modulation (4 boutons : Cadence, Pivot, Altération, Chromatique). */
class ModulationTypeZone : public BaseZone
{
public:
    ModulationTypeZone();
    ~ModulationTypeZone() override = default;
    
    std::function<void(Diatony::ModulationType)> onTypeChanged;
    
    void setSelectedType(Diatony::ModulationType type);
    Diatony::ModulationType getSelectedType() const { return selectedType; }

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    static constexpr std::array<Diatony::ModulationType, 4> modulationTypes = {
        Diatony::ModulationType::PerfectCadence,
        Diatony::ModulationType::PivotChord,
        Diatony::ModulationType::Alteration,
        Diatony::ModulationType::Chromatic
    };
    
    static constexpr std::array<const char*, 4> typeLabels = {
        "Perfect Cadence", "Pivot Chord", "Alteration", "Chromatic"
    };
    
    juce::Colour normalColour = juce::Colours::lightgrey;
    juce::Colour selectedColour = juce::Colour(0xFF2E8B57);  // SeaGreen
    
    std::array<std::unique_ptr<SelectableStyledButton>, 4> typeButtons;
    Diatony::ModulationType selectedType = Diatony::ModulationType::PerfectCadence;
    
    void setupButtons();
    void selectType(int index);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationTypeZone)
};
