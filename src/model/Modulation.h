#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * @brief Wrapper autour d'un ValueTree représentant une modulation tonale.
 * 
 * Relie deux sections via un changement de tonalité (accord pivot, modulation directe, etc.).
 * Propriétés : fromSectionId/toSectionId, fromChordIndex/toChordIndex, modulationType.
 */
class Modulation {
public:
    explicit Modulation(juce::ValueTree state);
    
    void setModulationType(Diatony::ModulationType newType);
    void setFromSectionId(int newFromSectionId);
    void setToSectionId(int newToSectionId);
    void setFromChordIndex(int newFromChordIndex);
    void setToChordIndex(int newToChordIndex);
    void setName(const juce::String& newName);
    
    int getId() const;
    Diatony::ModulationType getModulationType() const;
    int getFromSectionId() const;
    int getToSectionId() const;
    int getFromChordIndex() const;
    int getToChordIndex() const;
    juce::String getName() const;
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::MODULATION); }
    
    bool hasValidSectionReferences() const;
    bool hasChordIndices() const;
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    static int modulationTypeToInt(Diatony::ModulationType type);
    static Diatony::ModulationType intToModulationType(int value);
};
