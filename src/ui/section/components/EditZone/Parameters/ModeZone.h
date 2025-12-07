#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "ui/extra/Button/SelectableStyledButton.h"
#include "model/DiatonyTypes.h"
#include "ui/DiatonyText.h"
#include <array>

//==============================================================================
/**
 * Zone de sélection du mode (Majeur/Mineur)
 * Contient 2 boutons dans un cadre BaseZone
 */
class ModeZone : public BaseZone
{
public:
    ModeZone();
    
    // Callback quand le mode change
    std::function<void(Diatony::Mode)> onModeChanged;
    
    // Setter pour synchronisation modèle → UI
    void setSelectedMode(Diatony::Mode mode);
    
    Diatony::Mode getSelectedMode() const;

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;

private:
    // Les 2 boutons pour Major/Minor
    std::array<std::unique_ptr<SelectableStyledButton>, 2> modeButtons;
    
    // Les 2 modes dans l'ordre
    static constexpr std::array<Diatony::Mode, 2> modes = {
        Diatony::Mode::Major, Diatony::Mode::Minor
    };
    
    // Mode actuellement sélectionné
    Diatony::Mode selectedMode = Diatony::Mode::Major;
    
    void setupButtons();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeZone)
};

