#pragma once

#include <juce_core/juce_core.h>
#include "Progression.h"
#include <functional>

/**
 * Représente une section tonale complète avec sa tonalité, son mode et sa progression
 * Version simplifiée sans dépendances vers les constantes Diatony
 */
class Section {
public:
    // Constructeurs
    Section();
    explicit Section(const juce::String& sectionName);
    
    // Setters
    void setNote(int newNote);           // 0-6 pour C, D, E, F, G, A, B
    void setAlteration(int newAlteration); // 0, 1, 2 pour naturel, dièse, bémol
    void setIsMajor(bool newIsMajor);
    void setName(const juce::String& newName);
    void setProgression(const Progression& newProgression);
    
    // Getters
    int getNote() const { return note; }
    int getAlteration() const { return alteration; }
    bool getIsMajor() const { return isMajor; }
    const juce::String& getName() const { return name; }
    const Progression& getProgression() const { return progression; }
    Progression& getProgression() { return progression; }
    
    // Méthodes utilitaires simples
    bool isEmpty() const;
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onSectionChanged;
    
private:
    int note;            // Note principale (0-6 : C, D, E, F, G, A, B)
    int alteration;      // Altération (0: naturel, 1: dièse, 2: bémol)
    bool isMajor;        // Mode : true = majeur, false = mineur
    juce::String name;   // Nom de la section
    Progression progression;
    
    void notifyChange();
}; 