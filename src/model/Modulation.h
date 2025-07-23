#pragma once

#include <juce_core/juce_core.h>
#include <functional>

/**
 * Représente une modulation entre deux sections ou accords
 * Classe simple qui encapsule les paramètres essentiels d'une modulation
 */
class Modulation {
public:
    // Constructeurs
    Modulation();
    Modulation(int type, int fromChordIndex, int toChordIndex);
    
    // Setters
    void setModulationType(int newType);
    void setFromChordIndex(int newFromChordIndex);
    void setToChordIndex(int newToChordIndex);
    
    // Getters
    int getModulationType() const { return modulationType; }
    int getFromChordIndex() const { return fromChordIndex; }
    int getToChordIndex() const { return toChordIndex; }
    
    // Méthodes utilitaires
    juce::String toString() const;
    
    // Callback pour notifier les changements
    std::function<void()> onModulationChanged;
    
private:
    int modulationType;  // Type de modulation (0-3 : PERFECT_CADENCE, PIVOT_CHORD, ALTERATION, CHROMATIC)
    int fromChordIndex;  // Index de l'accord de départ
    int toChordIndex;    // Index de l'accord d'arrivée
    
    void notifyChange();
}; 