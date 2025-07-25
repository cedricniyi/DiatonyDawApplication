#pragma once

#include <juce_core/juce_core.h>
#include <functional>
#include "DiatonyTypes.h"

/**
 * Représente un accord avec son degré, sa qualité et son état (renversement)
 * Classe simple qui encapsule les trois composants essentiels d'un accord harmonique
 */
class Chord {
public:
    // Constructeurs
    Chord();
    Chord(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState state);
    
    // Setters
    void setDegree(Diatony::ChordDegree newDegree);
    void setQuality(Diatony::ChordQuality newQuality);
    void setState(Diatony::ChordState newState);
    
    // Getters
    Diatony::ChordDegree getDegree() const { return degree; }
    Diatony::ChordQuality getQuality() const { return quality; }
    Diatony::ChordState getState() const { return state; }
    
    // Affichage simple
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onChordChanged;
    
private:
    Diatony::ChordDegree degree;
    Diatony::ChordQuality quality;
    Diatony::ChordState state;
    
    void notifyChange();
}; 