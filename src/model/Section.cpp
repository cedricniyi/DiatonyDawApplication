#include "Section.h"

// Constructeurs
Section::Section() : note(0), alteration(0), isMajor(true), name("Section") {}

Section::Section(const juce::String& sectionName) 
    : note(0), alteration(0), isMajor(true), name(sectionName) {}

// Setters
void Section::setNote(int newNote) {
    if (note != newNote) {
        note = newNote;
        notifyChange();
    }
}

void Section::setAlteration(int newAlteration) {
    if (alteration != newAlteration) {
        alteration = newAlteration;
        notifyChange();
    }
}

void Section::setIsMajor(bool newIsMajor) {
    if (isMajor != newIsMajor) {
        isMajor = newIsMajor;
        notifyChange();
    }
}

void Section::setName(const juce::String& newName) {
    if (name != newName) {
        name = newName;
        notifyChange();
    }
}

void Section::setProgression(const Progression& newProgression) {
    progression = newProgression;
    notifyChange();
}

// Méthodes utilitaires
bool Section::isEmpty() const {
    return progression.isEmpty();
}

juce::String Section::toString() const {
    juce::String result = name + " (";
    result += "Note:" + juce::String(note);
    result += ", Alteration:" + juce::String(alteration);
    result += ", Mode:" + juce::String(isMajor ? "Major" : "Minor");
    result += ", Chords:" + juce::String(progression.size());
    result += ")";
    return result;
}

// Méthodes privées
void Section::notifyChange() {
    if (onSectionChanged) {
        onSectionChanged();
    }
} 