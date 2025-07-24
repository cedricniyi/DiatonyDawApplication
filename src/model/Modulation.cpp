#include "Modulation.h"

Modulation::Modulation() 
    : modulationType(0), fromChordIndex(-1), toChordIndex(-1) {
}

Modulation::Modulation(int type, int fromChordIndex, int toChordIndex) 
    : modulationType(type), fromChordIndex(fromChordIndex), toChordIndex(toChordIndex) {
}

void Modulation::setModulationType(int newType) {
    if (newType >= 0 && newType <= 3) {  // 0-3 pour les 4 types de modulation
        modulationType = newType;
        notifyChange();
    }
}

void Modulation::setFromChordIndex(int newFromChordIndex) {
    fromChordIndex = newFromChordIndex;
    notifyChange();
}

void Modulation::setToChordIndex(int newToChordIndex) {
    toChordIndex = newToChordIndex;
    notifyChange();
}

juce::String Modulation::toString() const {
    juce::String result;
    
    // Types de modulation
    const char* modulationTypes[] = {
        "Perfect Cadence", 
        "Pivot Chord", 
        "Alteration", 
        "Chromatic"
    };
    
    if (modulationType >= 0 && modulationType <= 3) {
        result = modulationTypes[modulationType];
    } else {
        result = "Unknown";
    }
    
    result += " (";
    
    if (fromChordIndex >= 0) {
        result += "from chord " + juce::String(fromChordIndex);
    } else {
        result += "from unknown";
    }
    
    if (toChordIndex >= 0) {
        result += " to chord " + juce::String(toChordIndex);
    } else {
        result += " to unknown";
    }
    
    result += ")";
    
    return result;
}

void Modulation::notifyChange() {
    if (onModulationChanged) {
        onModulationChanged();
    }
} 