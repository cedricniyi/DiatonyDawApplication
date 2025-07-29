#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Chord.h"
#include "ModelIdentifiers.h"

/**
 * Représente une progression harmonique avec une liste d'accords
 * Wrapper autour d'un ValueTree qui gère une collection de nœuds Chord
 * Ne stocke pas de données lui-même mais manipule le ValueTree sous-jacent
 */
class Progression {
public:
    // Constructeur avec ValueTree existant (mode wrapper)
    explicit Progression(juce::ValueTree state);
    
    // Méthode statique pour créer une nouvelle Progression dans un parent
    static Progression createIn(juce::ValueTree parentTree);
    
    // Gestion des accords (manipulent directement le ValueTree)
    void addChord(const Chord& chord);
    void addChord(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState chordState);
    void insertChord(size_t index, const Chord& chord);
    void removeChord(size_t index);
    void setChord(size_t index, const Chord& chord);
    
    // Accesseurs (créent des wrappers Chord à la demande)
    Chord getChord(size_t index) const;
    Chord getChord(size_t index);
    
    // Accès direct aux ValueTree des accords
    juce::ValueTree getChordState(size_t index) const;
    
    // Informations sur la progression
    size_t size() const;
    bool isEmpty() const;
    
    // Méthodes utilitaires
    void clear();
    juce::String toString() const;
    
    // Accès au ValueTree sous-jacent
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid(); }
    
    // Création d'un nouveau nœud Progression dans un ValueTree
    static juce::ValueTree createProgressionNode();
    
private:
    juce::ValueTree state;
    
    // Validation des index
    void validateIndex(size_t index) const;
}; 