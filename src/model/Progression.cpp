#include "Progression.h"

// Constructeur avec ValueTree existant
Progression::Progression(juce::ValueTree state) : state(state)
{
    jassert(state.hasType(Identifiers::PROGRESSION));
}

// Méthode statique pour créer une nouvelle Progression dans un parent
Progression Progression::createIn(juce::ValueTree parentTree)
{
    auto progressionNode = createProgressionNode();
    parentTree.appendChild(progressionNode, nullptr);
    return Progression(progressionNode);
}

// Gestion des accords
void Progression::addChord(const Chord& chord)
{
    // Copie le ValueTree du chord dans notre progression
    state.appendChild(chord.getState().createCopy(), nullptr);
}

void Progression::addChord(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState chordState)
{
    // Crée directement un nouveau chord dans notre ValueTree
    auto chordNode = Chord::createChordNode(degree, quality, chordState);
    this->state.appendChild(chordNode, nullptr);
}

void Progression::insertChord(size_t index, const Chord& chord)
{
    if (index > size())
        index = size(); // Insert à la fin si index trop grand
        
    state.addChild(chord.getState().createCopy(), static_cast<int>(index), nullptr);
}

void Progression::removeChord(size_t index)
{
    validateIndex(index);
    state.removeChild(static_cast<int>(index), nullptr);
}

void Progression::setChord(size_t index, const Chord& chord)
{
    validateIndex(index);
    
    // Remplace le child existant
    state.removeChild(static_cast<int>(index), nullptr);
    state.addChild(chord.getState().createCopy(), static_cast<int>(index), nullptr);
}

// Accesseurs
Chord Progression::getChord(size_t index) const
{
    validateIndex(index);
    return Chord(state.getChild(static_cast<int>(index)));
}

Chord Progression::getChord(size_t index)
{
    validateIndex(index);
    return Chord(state.getChild(static_cast<int>(index)));
}

juce::ValueTree Progression::getChordState(size_t index) const
{
    validateIndex(index);
    return state.getChild(static_cast<int>(index));
}

// Informations sur la progression
size_t Progression::size() const
{
    return static_cast<size_t>(state.getNumChildren());
}

bool Progression::isEmpty() const
{
    return state.getNumChildren() == 0;
}

// Méthodes utilitaires
void Progression::clear()
{
    state.removeAllChildren(nullptr);
}

juce::String Progression::toString() const
{
    if (!isValid())
        return "Invalid Progression";
        
    if (isEmpty())
        return "Empty Progression";
        
    juce::String result = "Progression: [";
    for (size_t i = 0; i < size(); ++i)
    {
        if (i > 0) result += ", ";
        result += getChord(i).toString();
    }
    result += "]";
    
    return result;
}

// Création d'un nouveau nœud Progression
juce::ValueTree Progression::createProgressionNode()
{
    juce::ValueTree progressionNode(Identifiers::PROGRESSION);
    
    // Générer un ID unique pour cette progression
    static int nextId = 1;
    progressionNode.setProperty(Identifiers::id, nextId++, nullptr);
    
    return progressionNode;
}

// Validation des index
void Progression::validateIndex(size_t index) const
{
    if (index >= size())
    {
        jassertfalse; // En debug, on veut savoir qu'il y a un problème
        throw std::out_of_range("Chord index out of range");
    }
} 