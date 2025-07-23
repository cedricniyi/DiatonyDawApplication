#pragma once

#include <vector>
#include <juce_core/juce_core.h>
#include "Chord.h"
#include <functional>

/**
 * Représente une progression harmonique avec une liste d'accords
 * Version simplifiée utilisant la classe Chord
 */
class Progression {
public:
    // Constructeur
    Progression() = default;
    
    // Gestion des accords
    void addChord(const Chord& chord);
    void insertChord(size_t index, const Chord& chord);
    void removeChord(size_t index);
    void setChord(size_t index, const Chord& chord);
    
    // Accesseurs
    const Chord& getChord(size_t index) const;
    Chord& getChord(size_t index);
    const std::vector<Chord>& getChords() const { return chords; }
    
    // Informations sur la progression
    size_t size() const { return chords.size(); }
    bool isEmpty() const { return chords.empty(); }
    
    // Méthodes utilitaires
    void clear();
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onProgressionChanged;
    
private:
    std::vector<Chord> chords;
    
    void notifyChange();
}; 