#pragma once

#include <vector>
#include <string>
#include <juce_core/juce_core.h>
#include "../utils/DiatonyConstants.h"

class Progression {
public:
    Progression() = default;
    
    // Setters directs pour les *Zone (remplacent les méthodes parseXXXString)
    void setChords(const std::vector<int>& newChords) {
        chords = newChords;
    }
    
    void setStates(const std::vector<int>& newStates) {
        states = newStates;
    }
    
    void setQualities(const std::vector<int>& newQualities) {
        qualities = newQualities;
    }
    
    // Méthodes pour ajouter des éléments individuels
    void addChord(int chord) {
        chords.push_back(chord);
    }
    
    void addState(int state) {
        states.push_back(state);
    }
    
    void addQuality(int quality) {
        qualities.push_back(quality);
    }
    
    // Accesseurs
    const std::vector<int>& getChords() const { return chords; }
    const std::vector<int>& getStates() const { return states; }
    const std::vector<int>& getQualities() const { return qualities; }
    
    // Accesseurs pour la taille
    size_t size() const { return chords.size(); }
    bool isEmpty() const { return chords.empty(); }
    
    bool isValid() const {
        // Vérifie que progression et états existent
        if (chords.empty()) {
            DBG("Progression invalide: aucun accord défini");
            return false;
        }
        
        if (states.empty()) {
            DBG("Progression invalide: aucun état défini");
            return false;
        }
        
        // Vérifie la cohérence entre progression et états
        if (states.size() != chords.size()) {
            DBG(juce::String::fromUTF8("Progression invalide: nombre d'états (") + juce::String(states.size()) + 
                juce::String::fromUTF8(") différent du nombre d'accords (") + juce::String(chords.size()) + juce::String::fromUTF8(")"));
            return false;
        }
        
        // Vérifie les qualités si spécifiées
        if (!qualities.empty() && qualities.size() != chords.size()) {
            DBG("Progression invalide: nombre de qualités (" + juce::String(qualities.size()) + 
                ") différent du nombre d'accords (" + juce::String(chords.size()) + ")");
            return false;
        }
        
        // Si toutes les vérifications sont passées
        return true;
    }
    
    void clear() {
        chords.clear();
        states.clear();
        qualities.clear();
    }
    
private:
    std::vector<int> chords;
    std::vector<int> states;
    std::vector<int> qualities;
}; 