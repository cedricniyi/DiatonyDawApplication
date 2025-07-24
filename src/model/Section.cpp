#include "Section.h"

Section::Section() 
    : note(0), alteration(0), isMajor(true), name("Untitled Section") {
}

Section::Section(const juce::String& sectionName) 
    : note(0), alteration(0), isMajor(true), name(sectionName) {
}

void Section::setNote(int newNote) {
    if (newNote >= 0 && newNote <= 6) {
        note = newNote;
        notifyChange();
    }
}

void Section::setAlteration(int newAlteration) {
    if (newAlteration >= 0 && newAlteration <= 2) {
        alteration = newAlteration;
        notifyChange();
    }
}

void Section::setIsMajor(bool newIsMajor) {
    isMajor = newIsMajor;
    notifyChange();
}

void Section::setName(const juce::String& newName) {
    name = newName;
    notifyChange();
}

void Section::setProgression(const Progression& newProgression) {
    progression = newProgression;
    
    // Connecter le callback de la progression
    progression.onProgressionChanged = [this]() {
        notifyChange();
    };
    
    notifyChange();
}

bool Section::isEmpty() const {
    return progression.isEmpty();
}

bool Section::isValid() const {
    // Une section est valide si :
    // - Elle a un nom non vide
    // - Sa progression n'est pas vide
    // - Les paramètres tonals sont dans les bonnes plages
    return !name.isEmpty() && 
           !progression.isEmpty() && 
           note >= 0 && note <= 6 && 
           alteration >= 0 && alteration <= 2;
}

juce::String Section::toString() const {
    juce::String result = name + " (";
    
    // Afficher la note
    const char* noteNames[] = {"C", "D", "E", "F", "G", "A", "B"};
    result += noteNames[note];
    
    // Afficher l'altération
    if (alteration == 1) {
        result += "#";
    } else if (alteration == 2) {
        result += "b";
    }
    
    // Afficher le mode
    result += isMajor ? " major" : " minor";
    result += ") - ";
    
    // Afficher la progression
    if (progression.isEmpty()) {
        result += "Empty progression";
    } else {
        result += juce::String(progression.size()) + " chords";
    }
    
    return result;
}

void Section::notifyChange() {
    if (onSectionChanged) {
        onSectionChanged();
    }
} 