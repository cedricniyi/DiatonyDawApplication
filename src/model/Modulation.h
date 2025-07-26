#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "DiatonyTypes.h"
#include "Identifiers.h"

/**
 * Représente une modulation entre deux sections ou accords
 * Wrapper léger autour d'un ValueTree qui ne stocke pas de données lui-même
 * mais agit comme interface de haut niveau pour lire/écrire dans le ValueTree
 */
class Modulation {
public:
    // Constructeur avec ValueTree existant (mode wrapper)
    explicit Modulation(juce::ValueTree state);
    
    // Méthode statique pour créer une nouvelle Modulation dans un parent
    static Modulation createIn(juce::ValueTree parentTree, Diatony::ModulationType type, 
                              int fromChordIndex, int toChordIndex);
    
    // Setters (modifient directement le ValueTree)
    void setModulationType(Diatony::ModulationType newType);
    void setFromChordIndex(int newFromChordIndex);
    void setToChordIndex(int newToChordIndex);
    void setName(const juce::String& newName);
    
    // Getters (lisent directement du ValueTree)
    Diatony::ModulationType getModulationType() const;
    int getFromChordIndex() const;
    int getToChordIndex() const;
    const juce::String getName() const;
    
    // Accès au ValueTree sous-jacent
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid(); }
    
    // Méthodes utilitaires
    juce::String toString() const;
    
    // Création d'un nouveau nœud Modulation dans un ValueTree
    static juce::ValueTree createModulationNode(Diatony::ModulationType type,
                                               int fromChordIndex,
                                               int toChordIndex);
    
private:
    juce::ValueTree state;
    
    // Helpers pour la conversion des types
    static int modulationTypeToInt(Diatony::ModulationType type);
    static Diatony::ModulationType intToModulationType(int value);
    
    // Met à jour le nom basé sur les paramètres actuels
    void updateName();
}; 