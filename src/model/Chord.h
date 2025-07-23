#pragma once

#include <juce_core/juce_core.h>
#include <functional>

/**
 * Représente un accord avec son degré, sa qualité et son état (renversement)
 * Classe simple qui encapsule les trois composants essentiels d'un accord harmonique
 */
class Chord {
public:
    // Constructeurs
    Chord();
    Chord(int degree, int quality, int state);
    
    // Setters
    void setDegree(int newDegree);
    void setQuality(int newQuality);
    void setState(int newState);
    
    // Getters
    int getDegree() const { return degree; }
    int getQuality() const { return quality; }
    int getState() const { return state; }
    
    // Affichage simple
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onChordChanged;
    
private:
    int degree;
    int quality;
    int state;
    
    void notifyChange();
}; 