#include "Modulation.h"

Modulation::Modulation(juce::ValueTree state) : state(state)
{
    jassert(!state.isValid() || state.hasType(ModelIdentifiers::MODULATION));
}

void Modulation::setModulationType(Diatony::ModulationType newType)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::modulationType, modulationTypeToInt(newType), nullptr);
}

void Modulation::setFromSectionId(int newFromSectionId)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::fromSectionId, newFromSectionId, nullptr);
}

void Modulation::setToSectionId(int newToSectionId)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::toSectionId, newToSectionId, nullptr);
}

void Modulation::setFromChordIndex(int newFromChordIndex)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::fromChordIndex, newFromChordIndex, nullptr);
}

void Modulation::setToChordIndex(int newToChordIndex)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::toChordIndex, newToChordIndex, nullptr);
}

void Modulation::setName(const juce::String& newName)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::name, newName, nullptr);
}

int Modulation::getId() const { return state.getProperty(ModelIdentifiers::id, -1); }
Diatony::ModulationType Modulation::getModulationType() const { return intToModulationType(state.getProperty(ModelIdentifiers::modulationType, 0)); }
int Modulation::getFromSectionId() const { return state.getProperty(ModelIdentifiers::fromSectionId, -1); }
int Modulation::getToSectionId() const { return state.getProperty(ModelIdentifiers::toSectionId, -1); }
int Modulation::getFromChordIndex() const { return state.getProperty(ModelIdentifiers::fromChordIndex, -1); }
int Modulation::getToChordIndex() const { return state.getProperty(ModelIdentifiers::toChordIndex, -1); }
juce::String Modulation::getName() const { return state.getProperty(ModelIdentifiers::name, "Unknown Modulation").toString(); }

bool Modulation::hasValidSectionReferences() const { return getFromSectionId() >= 0 && getToSectionId() >= 0; }
bool Modulation::hasChordIndices() const { return getFromChordIndex() >= 0 && getToChordIndex() >= 0; }

juce::String Modulation::toString() const
{
    if (!isValid())
        return "Invalid Modulation";
    
    return getName() + " (ID=" + juce::String(getId()) +
           ", Section " + juce::String(getFromSectionId()) +
           " -> Section " + juce::String(getToSectionId()) + ")";
}

int Modulation::modulationTypeToInt(Diatony::ModulationType type) { return static_cast<int>(type); }
Diatony::ModulationType Modulation::intToModulationType(int value) { return static_cast<Diatony::ModulationType>(value); }
