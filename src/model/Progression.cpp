#include "Progression.h"

// Gestion des accords
void Progression::addChord(const Chord& chord) {
    chords.push_back(chord);
    notifyChange();
}

void Progression::insertChord(size_t index, const Chord& chord) {
    if (index <= chords.size()) {
        chords.insert(chords.begin() + index, chord);
        notifyChange();
    }
}

void Progression::removeChord(size_t index) {
    if (index < chords.size()) {
        chords.erase(chords.begin() + index);
        notifyChange();
    }
}

void Progression::setChord(size_t index, const Chord& chord) {
    if (index < chords.size()) {
        chords[index] = chord;
        notifyChange();
    }
}

// Accesseurs
const Chord& Progression::getChord(size_t index) const {
    if (index >= chords.size()) {
        throw std::out_of_range("Chord index out of range");
    }
    return chords[index];
}

Chord& Progression::getChord(size_t index) {
    if (index >= chords.size()) {
        throw std::out_of_range("Chord index out of range");
    }
    return chords[index];
}

// Méthodes utilitaires
void Progression::clear() {
    chords.clear();
    notifyChange();
}

juce::String Progression::toString() const {
    if (isEmpty()) {
        return "Empty progression";
    }
    
    juce::String result = "Progression [";
    for (size_t i = 0; i < chords.size(); ++i) {
        if (i > 0) result += ", ";
        result += chords[i].toString();
    }
    result += "]";
    return result;
}

// Méthodes privées
void Progression::notifyChange() {
    if (onProgressionChanged) {
        onProgressionChanged();
    }
} 