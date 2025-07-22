#pragma once

#include <juce_core/juce_core.h>
#include "Progression.h"
#include "../utils/DiatonyConstants.h"

/**
 * Modèle central pour toutes les données nécessaires au solveur Diatony
 * Remplace l'état fragmenté dans différents composants
 */
class DiatonyModel {
public:
    DiatonyModel() : tonality(C), isMajor(true) {}
    
    // Setters pour les *Zone
    void setTonality(int newTonality) {
        tonality = newTonality;
        notifyChange();
    }
    
    void setMode(bool newIsMajor) {
        isMajor = newIsMajor;
        notifyChange();
    }
    
    void setProgression(const Progression& newProgression) {
        progression = newProgression;
        notifyChange();
    }
    
    // Accesseurs
    int getTonality() const { return tonality; }
    bool getIsMajor() const { return isMajor; }
    const Progression& getProgression() const { return progression; }
    Progression& getProgression() { return progression; }
    
    // Validation
    bool isComplete() const {
        return progression.isValid();
    }
    
    bool isEmpty() const {
        return progression.isEmpty();
    }
    
    // Méthodes utilitaires
    void clear() {
        tonality = C;
        isMajor = true;
        progression.clear();
        notifyChange();
    }
    
    // Callback pour notifier les changements
    std::function<void()> onModelChanged;
    
    // Méthode pour obtenir un résumé textuel
    juce::String toString() const {
        juce::String result;
        
        // Tonalité
        result += "Tonality: ";
        for (const auto& note : DiatonyConstants::NOTES) {
            if (note.value == tonality) {
                result += juce::String(note.name);
                break;
            }
        }
        
        // Mode
        result += ", Mode: " + juce::String(isMajor ? "Major" : "Minor");
        
        // Progression
        result += ", Progression: ";
        if (progression.isEmpty()) {
            result += "Empty";
        } else {
            result += juce::String(progression.size()) + " chords";
        }
        
        return result;
    }
    
private:
    int tonality;
    bool isMajor;
    Progression progression;
    
    void notifyChange() {
        if (onModelChanged) {
            onModelChanged();
        }
    }
}; 