#pragma once

#include <juce_core/juce_core.h>
#include "Progression.h"
#include "DiatonyTypes.h"
#include <functional>

/**
 * Représente une section tonale complète avec sa tonalité, son mode et sa progression
 * Version avec types fortement typés utilisant DiatonyTypes.h
 */
class Section {
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
    const juce::String& getName() const { return name; }
    const Progression& getProgression() const { return progression; }
    Progression& getProgression() { return progression; }
    
    // Méthodes utilitaires simples
    bool isEmpty() const;
    bool isValid() const;  // Vérifie si la section est valide
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onSectionChanged;
    
private:
    Diatony::Note note;            // Note principale fortement typée
    Diatony::Alteration alteration; // Altération fortement typée
    bool isMajor;        // Mode : true = majeur, false = mineur
    juce::String name;   // Nom de la section
    Progression progression;
    
    void notifyChange();
}; 