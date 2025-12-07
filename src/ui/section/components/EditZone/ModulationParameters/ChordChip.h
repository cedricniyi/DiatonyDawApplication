#pragma once

#include <JuceHeader.h>
#include "model/DiatonyTypes.h"
#include "ui/DiatonyText.h"
#include "utils/FontManager.h"
#include <functional>

//==============================================================================
/**
 * ChordChip - Petit composant cliquable représentant un accord
 * Affiche le degré + symbole qualité (I, IVm, V7, IIdim...)
 * Couleur basée sur la fonction tonale (même logique que InfoColoredPanel)
 */
class ChordChip : public juce::Component
{
public:
    ChordChip(int chordIndex, Diatony::ChordDegree degree, Diatony::ChordQuality quality);
    ~ChordChip() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override {}
    
    // Callback quand le chip est cliqué
    std::function<void(int chordIndex)> onClick;
    
    // Sélection visuelle
    void setSelected(bool selected);
    bool isSelected() const { return selected; }
    
    // Getters
    int getChordIndex() const { return chordIndex; }
    Diatony::ChordDegree getDegree() const { return degree; }
    Diatony::ChordQuality getQuality() const { return quality; }
    
    // Mettre à jour les données de l'accord
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
    // I/III/VI = Bleu (#4A90A4), II/IV = Or (#F1C40F), V/VII = Rouge (#E74C3C), autres = Gris
    juce::Colour getFunctionalColor() const;
    
    // Gestion du clic
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordChip)
};

