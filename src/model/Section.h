#pragma once

#include <juce_core/juce_core.h>
#include "Progression.h"
#include "DiatonyTypes.h"
#include "PieceElement.h"
#include <functional>

/**
 * Représente une section tonale complète avec sa tonalité, son mode et sa progression
 * Version avec types fortement typés utilisant DiatonyTypes.h
 */
class Section : public PieceElement {
public:
    // Constructeurs
    Section();
    explicit Section(const juce::String& sectionName);
    
    // Setters
    void setNote(Diatony::Note newNote);
    void setAlteration(Diatony::Alteration newAlteration);
    void setIsMajor(bool newIsMajor);
    void setName(const juce::String& newName);
    void setProgression(const Progression& newProgression);
    
    // Getters
    Diatony::Note getNote() const { return note; }
    Diatony::Alteration getAlteration() const { return alteration; }
    bool getIsMajor() const { return isMajor; }
    const Progression& getProgression() const { return progression; }
    Progression& getProgression() { return progression; }
    
    // Implémentation des méthodes virtuelles de PieceElement
    Type getType() const override { return Type::Section; }
    const juce::String& getName() const override { return name; }
    
    // Méthodes utilitaires simples
    bool isEmpty() const;
    bool isValid() const;  // Vérifie si la section est valide
    juce::String toString() const;
    
private:
    Diatony::Note note;            // Note principale fortement typée
    Diatony::Alteration alteration; // Altération fortement typée
    bool isMajor;        // Mode : true = majeur, false = mineur
    juce::String name;   // Nom de la section
    Progression progression;
    
    void notifyChange();
}; 