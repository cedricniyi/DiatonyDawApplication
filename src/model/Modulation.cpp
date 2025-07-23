#include "Modulation.h"

// Constructeurs
Modulation::Modulation() : modulationType(0), fromChordIndex(0), toChordIndex(0) {}

Modulation::Modulation(int type, int fromChordIndex, int toChordIndex) 
    : modulationType(type), fromChordIndex(fromChordIndex), toChordIndex(toChordIndex) {}

// Setters
void Modulation::setModulationType(int newType) {
    if (modulationType != newType) {
        modulationType = newType;
        notifyChange();
    }
}

void Modulation::setFromChordIndex(int newFromChordIndex) {
    if (fromChordIndex != newFromChordIndex) {
        fromChordIndex = newFromChordIndex;
        notifyChange();
    }
}

void Modulation::setToChordIndex(int newToChordIndex) {
    if (toChordIndex != newToChordIndex) {
        toChordIndex = newToChordIndex;
        notifyChange();
    }
}

// Méthodes utilitaires
juce::String Modulation::toString() const {
    juce::String result = "Modulation(";
    result += "Type:" + juce::String(modulationType);
    result += ", From:" + juce::String(fromChordIndex);
    result += ", To:" + juce::String(toChordIndex);
    result += ")";
    return result;
}

// Méthodes privées
void Modulation::notifyChange() {
    if (onModulationChanged) {
        onModulationChanged();
    }
} 