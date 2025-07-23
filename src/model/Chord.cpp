#include "Chord.h"

// Constructeurs
Chord::Chord() : degree(0), quality(0), state(0) {}

Chord::Chord(int degree, int quality, int state) 
    : degree(degree), quality(quality), state(state) {}

// Setters
void Chord::setDegree(int newDegree) {
    if (degree != newDegree) {
        degree = newDegree;
        notifyChange();
    }
}

void Chord::setQuality(int newQuality) {
    if (quality != newQuality) {
        quality = newQuality;
        notifyChange();
    }
}

void Chord::setState(int newState) {
    if (state != newState) {
        state = newState;
        notifyChange();
    }
}

// Affichage simple
juce::String Chord::toString() const {
    return "Chord(degree:" + juce::String(degree) + 
           ", quality:" + juce::String(quality) + 
           ", state:" + juce::String(state) + ")";
}

// Méthodes privées
void Chord::notifyChange() {
    if (onChordChanged) {
        onChordChanged();
    }
} 