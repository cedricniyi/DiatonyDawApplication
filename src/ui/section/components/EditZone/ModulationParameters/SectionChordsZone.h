#pragma once

#include "ui/extra/Component/Zone/BaseZone.h"
#include "ChordChip.h"
#include "model/Section.h"
#include "model/Progression.h"
#include "ui/DiatonyText.h"
#include <vector>
#include <memory>
#include <functional>

/**
 * @brief Zone affichant les accords d'une section adjacente (max 4 chips).
 *
 * Mode LastFour : derniers 4 accords (section source).
 * Mode FirstFour : premiers 4 accords (section destination).
 */
class SectionChordsZone : public BaseZone
{
public:
    enum class DisplayMode { LastFour, FirstFour };
    
    SectionChordsZone(DisplayMode mode);
    ~SectionChordsZone() override = default;
    
    void setSection(const Section& section, int sectionIndex);
    
    std::function<void(int chordIndex)> onChordSelected;
    
    void setSelectedChordIndex(int index);
    int getSelectedChordIndex() const { return selectedChordIndex; }
    
    void clear();

protected:
    void resizeContent(const juce::Rectangle<int>& contentBounds) override;
    void enablementChanged() override;

private:
    DisplayMode displayMode;
    std::vector<std::unique_ptr<ChordChip>> chordChips;
    int selectedChordIndex = -1;
    int chordIndexOffset = 0;
    size_t totalChordsInSection = 0;
    
    void layoutChips(const juce::Rectangle<int>& contentBounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SectionChordsZone)
};
