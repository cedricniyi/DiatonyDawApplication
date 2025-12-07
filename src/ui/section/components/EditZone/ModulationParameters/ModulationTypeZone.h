#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"
#include <array>
#include <memory>
#include <functional>

//==============================================================================
/**
 * Zone de sélection du type de modulation
 * 4 boutons sur une ligne : Cadence Parfaite, Accord Pivot, Altération, Chromatique
 * Bouton sélectionné en vert (au lieu de bleu comme KeyZone)
 */
class ModulationTypeZone : public BaseZone
{
public:
    ModulationTypeZone();
    ~ModulationTypeZone() override = default;
    
    // Callback appelé quand le type change
    std::function<void(Diatony::ModulationType)> onTypeChanged;
    
    // Setter pour synchronisation modèle → UI
    void setSelectedType(Diatony::ModulationType type);
    
    // Getter
    Diatony::ModulationType getSelectedType() const { return selectedType; }

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    // Les 4 types de modulation
    static constexpr std::array<Diatony::ModulationType, 4> modulationTypes = {
        Diatony::ModulationType::PerfectCadence,
        Diatony::ModulationType::PivotChord,
        Diatony::ModulationType::Alteration,
        Diatony::ModulationType::Chromatic
    };
    
    // Labels courts pour les boutons
    static constexpr std::array<const char*, 4> typeLabels = {
        "Perfect Cadence",
        "Pivot Chord",
        "Alteration",
        "Chromatic"
    };
    
    // Couleurs (vert au lieu de bleu)
    juce::Colour normalColour = juce::Colours::lightgrey;
    juce::Colour selectedColour = juce::Colour(0xFF2E8B57);  // SeaGreen
    
    // Boutons
    std::array<std::unique_ptr<SelectableStyledButton>, 4> typeButtons;
    
    Diatony::ModulationType selectedType = Diatony::ModulationType::PerfectCadence;
    
    void setupButtons();
    void selectType(int index);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationTypeZone)
};

