#pragma once

#include <juce_core/juce_core.h>
#include <functional>
#include "DiatonyTypes.h"
#include "PieceElement.h"

/**
 * Représente une modulation entre deux sections ou accords
 * Classe simple qui encapsule les paramètres essentiels d'une modulation
 */
class Modulation : public PieceElement {
public:
    // Constructeurs
    Modulation();
    Modulation(Diatony::ModulationType type, int fromChordIndex, int toChordIndex);
    
    // Setters
    void setModulationType(Diatony::ModulationType newType);
    void setFromChordIndex(int newFromChordIndex);
    void setToChordIndex(int newToChordIndex);
    
    // Getters
    Diatony::ModulationType getModulationType() const { return modulationType; }
    int getFromChordIndex() const { return fromChordIndex; }
    int getToChordIndex() const { return toChordIndex; }
    
    // Implémentation des méthodes virtuelles de PieceElement
    Type getType() const override { return Type::Modulation; }
    const juce::String& getName() const override { return name; }
    
    // Méthodes utilitaires
    juce::String toString() const;
    
private:
    Diatony::ModulationType modulationType;  // Type de modulation fortement typé
    int fromChordIndex;  // Index de l'accord de départ
    int toChordIndex;    // Index de l'accord d'arrivée
    juce::String name;   // Nom généré automatiquement basé sur le type
    
    void notifyChange();
    void updateName();   // Met à jour le nom basé sur les paramètres actuels
}; 