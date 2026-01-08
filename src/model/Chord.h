#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * @brief Wrapper autour d'un ValueTree représentant un accord harmonique.
 * 
 * Propriétés : degree (I-VII), quality (maj/min/dim...), state (renversement).
 * Pattern "vue" : ne stocke aucune donnée, délègue tout au ValueTree.
 */
class Chord {
public:
    explicit Chord(juce::ValueTree state);
    
    void setDegree(Diatony::ChordDegree newDegree);
    void setQuality(Diatony::ChordQuality newQuality);
    void setChordState(Diatony::ChordState newState);
    
    int getId() const;
    Diatony::ChordDegree getDegree() const;
    Diatony::ChordQuality getQuality() const;
    Diatony::ChordState getChordState() const;
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::CHORD); }
    
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    static int degreeToInt(Diatony::ChordDegree degree);
    static Diatony::ChordDegree intToDegree(int value);
    static int qualityToInt(Diatony::ChordQuality quality);
    static Diatony::ChordQuality intToQuality(int value);
    static int stateToInt(Diatony::ChordState state);
    static Diatony::ChordState intToState(int value);
};
