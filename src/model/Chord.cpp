#include "Chord.h"

// Constructeur avec ValueTree existant
Chord::Chord(juce::ValueTree state) : state(state)
{
    jassert(state.hasType(Identifiers::CHORD));
}

// Méthode statique pour créer un nouveau Chord dans un parent
Chord Chord::createIn(juce::ValueTree parentTree, Diatony::ChordDegree degree, 
                     Diatony::ChordQuality quality, Diatony::ChordState state)
{
    auto chordNode = createChordNode(degree, quality, state);
    parentTree.appendChild(chordNode, nullptr);
    return Chord(chordNode);
}

// Setters
void Chord::setDegree(Diatony::ChordDegree newDegree)
{
    state.setProperty(Identifiers::degree, degreeToInt(newDegree), nullptr);
}

void Chord::setQuality(Diatony::ChordQuality newQuality)
{
    state.setProperty(Identifiers::quality, qualityToInt(newQuality), nullptr);
}

void Chord::setChordState(Diatony::ChordState newState)
{
    state.setProperty(Identifiers::state, stateToInt(newState), nullptr);
}

// Getters
Diatony::ChordDegree Chord::getDegree() const
{
    return intToDegree(state.getProperty(Identifiers::degree, 0));
}

Diatony::ChordQuality Chord::getQuality() const
{
    return intToQuality(state.getProperty(Identifiers::quality, 0));
}

Diatony::ChordState Chord::getChordState() const
{
    return intToState(state.getProperty(Identifiers::state, 0));
}

// Méthodes utilitaires
juce::String Chord::toString() const
{
    if (!isValid())
        return "Invalid Chord";
        
    // Réutilise la logique existante de toString avec les nouvelles données
    return juce::String("Chord: ") + 
           juce::String(static_cast<int>(getDegree())) + " " +
           juce::String(static_cast<int>(getQuality())) + " " +
           juce::String(static_cast<int>(getChordState()));
}

// Création d'un nouveau nœud Chord
juce::ValueTree Chord::createChordNode(Diatony::ChordDegree degree,
                                      Diatony::ChordQuality quality,
                                      Diatony::ChordState state)
{
    juce::ValueTree chordNode(Identifiers::CHORD);
    
    // Générer un ID unique pour ce chord
    static int nextId = 1;
    chordNode.setProperty(Identifiers::id, nextId++, nullptr);
    
    // Définir les propriétés
    chordNode.setProperty(Identifiers::degree, degreeToInt(degree), nullptr);
    chordNode.setProperty(Identifiers::quality, qualityToInt(quality), nullptr);
    chordNode.setProperty(Identifiers::state, stateToInt(state), nullptr);
    
    return chordNode;
}

// Helpers pour la conversion des types
int Chord::degreeToInt(Diatony::ChordDegree degree)
{
    return static_cast<int>(degree);
}

Diatony::ChordDegree Chord::intToDegree(int value)
{
    return static_cast<Diatony::ChordDegree>(value);
}

int Chord::qualityToInt(Diatony::ChordQuality quality)
{
    return static_cast<int>(quality);
}

Diatony::ChordQuality Chord::intToQuality(int value)
{
    return static_cast<Diatony::ChordQuality>(value);
}

int Chord::stateToInt(Diatony::ChordState state)
{
    return static_cast<int>(state);
}

Diatony::ChordState Chord::intToState(int value)
{
    return static_cast<Diatony::ChordState>(value);
} 