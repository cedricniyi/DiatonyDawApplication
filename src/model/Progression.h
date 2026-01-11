#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Chord.h"
#include "ModelIdentifiers.h"

/**
 * @brief Wrapper autour d'un ValueTree représentant une progression harmonique.
 * 
 * Gère une collection ordonnée d'accords. Génère les IDs des Chords enfants.
 * Pattern "vue" : ne stocke aucune donnée, délègue tout au ValueTree.
 */
class Progression {
public:
    explicit Progression(juce::ValueTree state);
    
    void addChord(Diatony::ChordDegree degree, 
                  Diatony::ChordQuality quality = Diatony::ChordQuality::Auto, 
                  Diatony::ChordState chordState = Diatony::ChordState::Fundamental);
    void insertChord(size_t index, Diatony::ChordDegree degree,
                     Diatony::ChordQuality quality = Diatony::ChordQuality::Auto,
                     Diatony::ChordState chordState = Diatony::ChordState::Fundamental);
    void removeChord(size_t index);
    void clear();
    
    Chord getChord(size_t index) const;
    Chord getChord(size_t index);
    Chord getChordById(int id) const;
    int getChordIndexById(int id) const;
    
    size_t size() const;
    bool isEmpty() const;
    int getId() const;
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::PROGRESSION); }
    
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    int generateNextChordId() const;
    juce::ValueTree createChordNode(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState chordState);
    void validateIndex(size_t index) const;
};
