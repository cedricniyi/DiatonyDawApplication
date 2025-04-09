#pragma once

#include <vector>
#include <string>
#include <juce_core/juce_core.h>
#include "../utils/DiatonyConstants.h"

class Progression {
public:
    Progression() = default;
    
    // Méthodes de parsing pour les entrées utilisateur
    bool parseProgressionString(const juce::String& progression) {
        if (progression.isEmpty()) {
            return false;
        }
        
        std::vector<int> newChords;
        auto tokens = juce::StringArray::fromTokens(progression, " -", "");
        
        if (tokens.isEmpty()) {
            return false;
        }

        for (const auto& token : tokens) {
            int degreeValue = DiatonyConstants::getDegreeValue(token.toStdString());
            if (degreeValue != -1) {
                newChords.push_back(degreeValue);
            } else {
                return false; // Degré invalide trouvé
            }
        }
        
        this->chords = newChords;
        return true;
    }
    
    bool parseStatesString(const juce::String& states) {
        if (states.isEmpty()) {
            return false;
        }

        // Nous n'exigeons plus que chords ne soit pas vide
        // Cela permet d'entrer les états même si la progression n'est pas encore définie

        std::vector<int> newStates;
        auto tokens = juce::StringArray::fromTokens(states, " -", "");

        // Suppression de cette vérification immédiate pour permettre la validation ultérieure
        // if (tokens.size() != chords.size()) {
        //    return false;
        // }

        for (const auto& token : tokens) {
            if (token.isNotEmpty() && token.containsOnly("0123")) {
                int stateValue = token.getIntValue();
                if (stateValue >= 0 && stateValue <= 3) {
                    newStates.push_back(stateValue);
                    continue;
                }
            }
            
            int stateValue = DiatonyConstants::getChordStateValue(token.toStdString());
            if (stateValue != -1) {
                newStates.push_back(stateValue);
            } else {
                return false;
            }
        }

        this->states = newStates;
        return true;
    }
    
    bool parseQualitiesString(const juce::String& qualities) {
        if (qualities.isEmpty()) {
            this->qualities.clear();
            return true; // Permet les qualités vides
        }

        if (chords.empty()) {
            return false;
        }

        auto tokens = juce::StringArray::fromTokens(qualities, " ", "");
        
        if (tokens.size() != chords.size()) {
            return false;
        }

        std::vector<int> newQualities(chords.size(), -1); // -1 signifie "utiliser la qualité par défaut"

        int qualityIndex = 0;
        for (const auto& token : tokens) {
            if (token.isEmpty() || token == "-") {
                qualityIndex++;
                continue;
            }

            int qualityValue = DiatonyConstants::getChordQualityValue(token.toStdString());
            if (qualityValue == -1) {
                return false;
            }
            newQualities[qualityIndex++] = qualityValue;
        }

        this->qualities = newQualities;
        return true;
    }
    
    // Accesseurs
    const std::vector<int>& getChords() const { return chords; }
    const std::vector<int>& getStates() const { return states; }
    const std::vector<int>& getQualities() const { return qualities; }
    
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