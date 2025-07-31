#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * Représente un accord avec son degré, sa qualité et son état (renversement)
 * Wrapper léger autour d'un ValueTree qui ne stocke pas de données lui-même
 * mais agit comme interface de haut niveau pour lire/écrire dans le ValueTree
 */
class Chord {
public:
    // Constructeur avec ValueTree existant (mode wrapper)
    explicit Chord(juce::ValueTree state);
    
    // Méthode statique pour créer un nouveau Chord dans un parent
    static Chord createIn(juce::ValueTree parentTree, Diatony::ChordDegree degree, 
                         Diatony::ChordQuality quality, Diatony::ChordState state);
    
    // Setters (modifient directement le ValueTree)
    void setDegree(Diatony::ChordDegree newDegree);
    void setQuality(Diatony::ChordQuality newQuality);
    void setChordState(Diatony::ChordState newState);
    
    // Getters (lisent directement du ValueTree)
    Diatony::ChordDegree getDegree() const;
    Diatony::ChordQuality getQuality() const;
    Diatony::ChordState getChordState() const;  // Renommé pour éviter le conflit
    
    // Accès au ValueTree sous-jacent
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid(); }
    
    // Méthodes utilitaires
    juce::String toString() const;
    
    // Création d'un nouveau nœud Chord dans un ValueTree
    static juce::ValueTree createChordNode(Diatony::ChordDegree degree,
                                          Diatony::ChordQuality quality,
                                          Diatony::ChordState state);
    
private:
    juce::ValueTree state;
    
    // Helpers pour la conversion des types
    static int degreeToInt(Diatony::ChordDegree degree);
    static Diatony::ChordDegree intToDegree(int value);
    static int qualityToInt(Diatony::ChordQuality quality);
    static Diatony::ChordQuality intToQuality(int value);
    static int stateToInt(Diatony::ChordState state);
    static Diatony::ChordState intToState(int value);
}; 