#include "SectionChordsZone.h"

SectionChordsZone::SectionChordsZone(DisplayMode mode)
    : BaseZone(mode == DisplayMode::LastFour ? "Source" : "Dest.")
    , displayMode(mode)
{
}

void SectionChordsZone::enablementChanged()
{
    for (auto& chip : chordChips)
        chip->setEnabled(isEnabled());
}

void SectionChordsZone::setSection(const Section& section, int sectionIndex)
{
    if (!section.isValid())
    {
        clear();
        return;
    }
    
    // Header : "P1 (C Maj, 4 acc.)"
    juce::String headerText = "P" + juce::String(sectionIndex + 1) + " (";
    headerText += DiatonyText::getNoteName(section.getNote(), section.getAlteration());
    headerText += " " + juce::String(section.getIsMajor() ? "Maj" : "Min");
    headerText += ", " + juce::String(section.getProgression().size()) + " acc.)";
    setTitle(headerText);
    
    auto progression = section.getProgression();
    totalChordsInSection = progression.size();
    
    for (auto& chip : chordChips)
        removeChildComponent(chip.get());
    chordChips.clear();
    
    if (totalChordsInSection == 0)
    {
        chordIndexOffset = 0;
        resized();
        return;
    }
    
    // Déterminer quels accords afficher (max 4)
    size_t startIndex = 0;
    size_t endIndex = totalChordsInSection;
    
    if (displayMode == DisplayMode::LastFour)
    {
        if (totalChordsInSection > 4)
            startIndex = totalChordsInSection - 4;
        chordIndexOffset = static_cast<int>(startIndex);
    }
    else
    {
        if (totalChordsInSection > 4)
            endIndex = 4;
        chordIndexOffset = 0;
    }
    
    for (size_t i = startIndex; i < endIndex; ++i)
    {
        auto chord = progression.getChord(i);
        auto chip = std::make_unique<ChordChip>(
            static_cast<int>(i),
            chord.getDegree(),
            chord.getQuality()
        );
        
        chip->onClick = [this](int chordIndex) {
            setSelectedChordIndex(chordIndex);
            if (onChordSelected)
                onChordSelected(chordIndex);
        };
        
        addAndMakeVisible(*chip);
        chordChips.push_back(std::move(chip));
    }
    
    selectedChordIndex = -1;
    resized();
}

void SectionChordsZone::setSelectedChordIndex(int index)
{
    if (selectedChordIndex == index)
        return;
    
    selectedChordIndex = index;
    
    for (auto& chip : chordChips)
        chip->setSelected(chip->getChordIndex() == index);
}

void SectionChordsZone::clear()
{
    for (auto& chip : chordChips)
        removeChildComponent(chip.get());
    chordChips.clear();
    selectedChordIndex = -1;
    chordIndexOffset = 0;
    totalChordsInSection = 0;
    setTitle(displayMode == DisplayMode::LastFour ? "Source" : "Dest.");
    resized();
}

void SectionChordsZone::resizeContent(const juce::Rectangle<int>& contentBounds)
{
    layoutChips(contentBounds);
}

void SectionChordsZone::layoutChips(const juce::Rectangle<int>& contentBounds)
{
    if (chordChips.empty() || contentBounds.isEmpty())
        return;
    
    constexpr int CHIP_SPACING = 4;
    constexpr int CHIP_HEIGHT = 28;
    
    int numChips = static_cast<int>(chordChips.size());
    int availableWidth = contentBounds.getWidth() - ((numChips - 1) * CHIP_SPACING);
    int chipWidth = availableWidth / numChips;
    int startY = contentBounds.getY() + (contentBounds.getHeight() - CHIP_HEIGHT) / 2;
    
    int x = contentBounds.getX();
    for (auto& chip : chordChips)
    {
        chip->setBounds(x, startY, chipWidth, CHIP_HEIGHT);
        x += chipWidth + CHIP_SPACING;
    }
}
