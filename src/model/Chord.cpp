#include "Chord.h"

// Constructeurs
Chord::Chord() : degree(Diatony::ChordDegree::First), quality(Diatony::ChordQuality::Major), state(Diatony::ChordState::Fundamental) {}

Chord::Chord(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState state) 
    : degree(degree), quality(quality), state(state) {}

// Setters
void Chord::setDegree(Diatony::ChordDegree newDegree) {
    if (degree != newDegree) {
        degree = newDegree;
        notifyChange();
    }
}

void Chord::setQuality(Diatony::ChordQuality newQuality) {
    if (quality != newQuality) {
        quality = newQuality;
        notifyChange();
    }
}

void Chord::setState(Diatony::ChordState newState) {
    if (state != newState) {
        state = newState;
        notifyChange();
    }
}

// Affichage simple
juce::String Chord::toString() const {
    return "Chord(degree:" + juce::String(static_cast<int>(degree)) + 
           ", quality:" + juce::String(static_cast<int>(quality)) + 
           ", state:" + juce::String(static_cast<int>(state)) + ")";
}

// Méthodes privées
void Chord::notifyChange() {
    if (onChordChanged) {
        onChordChanged();
    }
} 