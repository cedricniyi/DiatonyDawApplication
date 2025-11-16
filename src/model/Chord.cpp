#include "Chord.h"

// Constructeur avec ValueTree existant
Chord::Chord(juce::ValueTree state) : state(state)
{
    jassert(state.hasType(ModelIdentifiers::CHORD));
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
    state.setProperty(ModelIdentifiers::degree, degreeToInt(newDegree), nullptr);
}

void Chord::setQuality(Diatony::ChordQuality newQuality)
{
    state.setProperty(ModelIdentifiers::quality, qualityToInt(newQuality), nullptr);
}

void Chord::setChordState(Diatony::ChordState newState)
{
    state.setProperty(ModelIdentifiers::state, stateToInt(newState), nullptr);
}

// Getters
Diatony::ChordDegree Chord::getDegree() const
{
    return intToDegree(state.getProperty(ModelIdentifiers::degree, 0));
}

Diatony::ChordQuality Chord::getQuality() const
{
    // Valeur par défaut : Auto (-1) pour détection automatique
    return intToQuality(state.getProperty(ModelIdentifiers::quality, -1));
}

Diatony::ChordState Chord::getChordState() const
{
    return intToState(state.getProperty(ModelIdentifiers::state, 0));
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
    juce::ValueTree chordNode(ModelIdentifiers::CHORD);
    
    // Générer un ID unique pour cet accord (commence à 0 pour correspondre aux index)
    static int nextId = 0;
    chordNode.setProperty(ModelIdentifiers::id, nextId++, nullptr);
    
    // Définir les propriétés
    chordNode.setProperty(ModelIdentifiers::degree, degreeToInt(degree), nullptr);
    chordNode.setProperty(ModelIdentifiers::quality, qualityToInt(quality), nullptr);
    chordNode.setProperty(ModelIdentifiers::state, stateToInt(state), nullptr);
    
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