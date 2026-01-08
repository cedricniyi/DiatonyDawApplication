#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "ChordChip.h"
#include "model/Section.h"
#include "model/Progression.h"
#include "ui/DiatonyText.h"
#include <vector>
#include <memory>
#include <functional>

//==============================================================================
/**
 * SectionChordsZone - BaseZone affichant les accords d'une section adjacente
 * Affiche max 4 accords : derniers 4 (mode LastFour) ou premiers 4 (mode FirstFour)
 * Header dynamique : "Section 1 (C Maj, 4 acc.)"
 * 
 * Quand désactivé (mode automatique), les chips sont non-cliquables
 * Seul le mode Pivot permet la sélection
 */
class SectionChordsZone : public BaseZone
{
public:
    // Mode d'affichage : derniers 4 accords (section source) ou premiers 4 (section dest)
    enum class DisplayMode { LastFour, FirstFour };
    
    SectionChordsZone(DisplayMode mode);
    ~SectionChordsZone() override = default;
    
    // Met à jour depuis une Section
    void setSection(const Section& section, int sectionIndex);
    
    // Callback quand un accord est cliqué (retourne l'index réel dans la progression)
    std::function<void(int chordIndex)> onChordSelected;
    
    // Sélectionner visuellement un accord
    void setSelectedChordIndex(int index);
    int getSelectedChordIndex() const { return selectedChordIndex; }
    
    // Vider la zone
    void clear();

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;
    void enablementChanged() override;  // Appelé quand setEnabled() change

private:
    DisplayMode displayMode;
    std::vector<std::unique_ptr<ChordChip>> chordChips;  // Max 4
    int selectedChordIndex = -1;
    
    // Offset pour convertir l'index local (0-3) en index réel dans la progression
    int chordIndexOffset = 0;
    
    // Nombre total d'accords dans la section (pour l'affichage dans le header)
    size_t totalChordsInSection = 0;
    
    void layoutChips(const juce::Rectangle<int>& contentBounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionChordsZone)
};

