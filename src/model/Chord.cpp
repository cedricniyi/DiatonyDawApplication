#include "Chord.h"

Chord::Chord(juce::ValueTree state) : state(state)
{
    jassert(!state.isValid() || state.hasType(ModelIdentifiers::CHORD));
}

void Chord::setDegree(Diatony::ChordDegree newDegree)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::degree, degreeToInt(newDegree), nullptr);
}

void Chord::setQuality(Diatony::ChordQuality newQuality)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::quality, qualityToInt(newQuality), nullptr);
}

void Chord::setChordState(Diatony::ChordState newState)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::state, stateToInt(newState), nullptr);
}

int Chord::getId() const
{
    return state.getProperty(ModelIdentifiers::id, -1);
}

Diatony::ChordDegree Chord::getDegree() const
{
    return intToDegree(state.getProperty(ModelIdentifiers::degree, 0));
}

Diatony::ChordQuality Chord::getQuality() const
{
    // -1 = Auto (qualité déduite du mode)
    return intToQuality(state.getProperty(ModelIdentifiers::quality, -1));
}

Diatony::ChordState Chord::getChordState() const
{
    return intToState(state.getProperty(ModelIdentifiers::state, 0));
}

juce::String Chord::toString() const
{
    if (!isValid())
        return "Invalid Chord";
    
    return "Chord (ID=" + juce::String(getId()) + 
           ", Degree=" + juce::String(static_cast<int>(getDegree())) + 
           ", Quality=" + juce::String(static_cast<int>(getQuality())) + 
           ", State=" + juce::String(static_cast<int>(getChordState())) + ")";
}

int Chord::degreeToInt(Diatony::ChordDegree degree) { return static_cast<int>(degree); }
Diatony::ChordDegree Chord::intToDegree(int value) { return static_cast<Diatony::ChordDegree>(value); }
int Chord::qualityToInt(Diatony::ChordQuality quality) { return static_cast<int>(quality); }
Diatony::ChordQuality Chord::intToQuality(int value) { return static_cast<Diatony::ChordQuality>(value); }
int Chord::stateToInt(Diatony::ChordState state) { return static_cast<int>(state); }
Diatony::ChordState Chord::intToState(int value) { return static_cast<Diatony::ChordState>(value); }
