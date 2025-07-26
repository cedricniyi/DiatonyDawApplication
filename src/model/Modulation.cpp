#include "Modulation.h"

Modulation::Modulation() 
    : modulationType(Diatony::ModulationType::PerfectCadence), fromChordIndex(-1), toChordIndex(-1) {
    updateName();
}

Modulation::Modulation(Diatony::ModulationType type, int fromChordIndex, int toChordIndex) 
    : modulationType(type), fromChordIndex(fromChordIndex), toChordIndex(toChordIndex) {
    updateName();
}

void Modulation::setModulationType(Diatony::ModulationType newType) {
    modulationType = newType;
    updateName();
    notifyChange();
}

void Modulation::setFromChordIndex(int newFromChordIndex) {
    fromChordIndex = newFromChordIndex;
    updateName();
    notifyChange();
}

void Modulation::setToChordIndex(int newToChordIndex) {
    toChordIndex = newToChordIndex;
    updateName();
    notifyChange();
}

juce::String Modulation::toString() const {
    juce::String result;
    
    // Types de modulation avec correspondance aux enum class
    switch (modulationType) {
        case Diatony::ModulationType::PerfectCadence:
            result = "Perfect Cadence";
            break;
        case Diatony::ModulationType::PivotChord:
            result = "Pivot Chord";
            break;
        case Diatony::ModulationType::Alteration:
            result = "Alteration";
            break;
        case Diatony::ModulationType::Chromatic:
            result = "Chromatic";
            break;
        default:
            result = "Unknown";
            break;
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

void Modulation::updateName() {
    // Générer le nom basé sur le type de modulation
    switch (modulationType) {
        case Diatony::ModulationType::PerfectCadence:
            name = "Perfect Cadence";
            break;
        case Diatony::ModulationType::PivotChord:
            name = "Pivot Chord";
            break;
        case Diatony::ModulationType::Alteration:
            name = "Alteration";
            break;
        case Diatony::ModulationType::Chromatic:
            name = "Chromatic";
            break;
        default:
            name = "Unknown Modulation";
            break;
    }
}

void Modulation::notifyChange() {
    // Appeler le callback unifié de PieceElement
    PieceElement::notifyChange();
} 