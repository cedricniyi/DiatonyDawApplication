#include "Section.h"

Section::Section() 
    : note(Diatony::Note::C), alteration(Diatony::Alteration::Natural), isMajor(true), name("Untitled Section") {
}

Section::Section(const juce::String& sectionName) 
    : note(Diatony::Note::C), alteration(Diatony::Alteration::Natural), isMajor(true), name(sectionName) {
}

void Section::setNote(Diatony::Note newNote) {
    note = newNote;
    notifyChange();
}

void Section::setAlteration(Diatony::Alteration newAlteration) {
    alteration = newAlteration;
    notifyChange();
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
    // Les paramètres tonals sont maintenant toujours valides grâce aux types fortement typés
    return !name.isEmpty() && !progression.isEmpty();
}

juce::String Section::toString() const {
    juce::String result = name + " (";
    
    // Afficher la note avec correspondance aux enum class
    switch (note) {
        case Diatony::Note::C: result += "C"; break;
        case Diatony::Note::CSharp: result += "C#"; break;
        case Diatony::Note::D: result += "D"; break;
        case Diatony::Note::DSharp: result += "D#"; break;
        case Diatony::Note::E: result += "E"; break;
        case Diatony::Note::F: result += "F"; break;
        case Diatony::Note::FSharp: result += "F#"; break;
        case Diatony::Note::G: result += "G"; break;
        case Diatony::Note::GSharp: result += "G#"; break;
        case Diatony::Note::A: result += "A"; break;
        case Diatony::Note::ASharp: result += "A#"; break;
        case Diatony::Note::B: result += "B"; break;
    }
    
    // Afficher l'altération
    switch (alteration) {
        case Diatony::Alteration::Flat: result += "b"; break;
        case Diatony::Alteration::Sharp: result += "#"; break;
        case Diatony::Alteration::Natural: break; // Pas d'ajout pour naturel
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