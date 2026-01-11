#pragma once

#include <JuceHeader.h>
#include "model/DiatonyTypes.h"
#include "ui/DiatonyText.h"
#include "utils/FontManager.h"
#include <functional>

/**
 * @brief Composant cliquable représentant un accord (I, IVm, V7, IIdim...).
 *
 * Couleur basée sur la fonction tonale : Tonique=Bleu, Sous-Dom=Or, Dom=Rouge.
 */
class ChordChip : public juce::Component
{
public:
    ChordChip(int chordIndex, Diatony::ChordDegree degree, Diatony::ChordQuality quality);
    ~ChordChip() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override {}
    
    std::function<void(int chordIndex)> onClick;
    
    void setSelected(bool selected);
    bool isSelected() const { return selected; }
    
    int getChordIndex() const { return chordIndex; }
    Diatony::ChordDegree getDegree() const { return degree; }
    Diatony::ChordQuality getQuality() const { return quality; }
    
    void setChordData(int newIndex, Diatony::ChordDegree newDegree, Diatony::ChordQuality newQuality);

private:
    int chordIndex;
    Diatony::ChordDegree degree;
    Diatony::ChordQuality quality;
    bool selected = false;
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Affiche "I", "IVm", "V7", "IIdim"...
    juce::String getDisplayText() const;
    
    // Couleur basée sur la fonction tonale
    juce::Colour getFunctionalColor() const;
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordChip)
};
