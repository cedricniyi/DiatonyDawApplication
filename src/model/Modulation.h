#pragma once

#include <juce_core/juce_core.h>
#include <functional>
#include "DiatonyTypes.h"

/**
 * Représente une modulation entre deux sections ou accords
 * Classe simple qui encapsule les paramètres essentiels d'une modulation
 */
class Modulation {
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
    
    // Méthodes utilitaires
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onModulationChanged;
    
private:
    Diatony::ModulationType modulationType;  // Type de modulation fortement typé
    int fromChordIndex;  // Index de l'accord de départ
    int toChordIndex;    // Index de l'accord d'arrivée
    
    void notifyChange();
}; 