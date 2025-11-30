#include "Progression.h"

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════════

Progression::Progression(juce::ValueTree state) : state(state)
{
    // En mode debug, vérifier que le ValueTree est du bon type
    jassert(!state.isValid() || state.hasType(ModelIdentifiers::PROGRESSION));
}

// ═══════════════════════════════════════════════════════════════════════════════
// GESTION DES ACCORDS
// ═══════════════════════════════════════════════════════════════════════════════

void Progression::addChord(Diatony::ChordDegree degree, 
                           Diatony::ChordQuality quality, 
                           Diatony::ChordState chordState)
{
    if (!state.isValid()) return;
    
    auto chordNode = createChordNode(degree, quality, chordState);
    state.appendChild(chordNode, nullptr);
    
    DBG("[Progression::addChord] Ajout accord ID=" << static_cast<int>(chordNode.getProperty(ModelIdentifiers::id)) 
        << ", total=" << size());
}

void Progression::insertChord(size_t index, Diatony::ChordDegree degree,
                              Diatony::ChordQuality quality,
                              Diatony::ChordState chordState)
{
    if (!state.isValid()) return;
    
    // Si l'index est trop grand, on ajoute à la fin
    if (index > size())
        index = size();
    
    auto chordNode = createChordNode(degree, quality, chordState);
    state.addChild(chordNode, static_cast<int>(index), nullptr);
}

void Progression::removeChord(size_t index)
{
    if (!state.isValid()) return;
    validateIndex(index);
    
    state.removeChild(static_cast<int>(index), nullptr);
    
    DBG("[Progression::removeChord] Suppression accord index=" << index << ", restant=" << size());
}

void Progression::clear()
{
    if (state.isValid())
        state.removeAllChildren(nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════════
// ACCÈS AUX ACCORDS
// ═══════════════════════════════════════════════════════════════════════════════

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

Chord Progression::getChordById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::CHORD))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return Chord(child);
            }
        }
    }
    
    // Si pas trouvé, retourner un wrapper invalide
    return Chord(juce::ValueTree());
}

int Progression::getChordIndexById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::CHORD))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return i;
            }
        }
    }
    return -1;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INFORMATIONS
// ═══════════════════════════════════════════════════════════════════════════════

size_t Progression::size() const
{
    return static_cast<size_t>(state.getNumChildren());
}

bool Progression::isEmpty() const
{
    return state.getNumChildren() == 0;
}

int Progression::getId() const
{
    return state.getProperty(ModelIdentifiers::id, -1);
}

// ═══════════════════════════════════════════════════════════════════════════════
// MÉTHODES UTILITAIRES
// ═══════════════════════════════════════════════════════════════════════════════

juce::String Progression::toString() const
{
    if (!isValid())
        return "Invalid Progression";
    
    if (isEmpty())
        return "Progression (empty)";
    
    return "Progression (ID=" + juce::String(getId()) + ", " + juce::String(size()) + " chords)";
}

// ═══════════════════════════════════════════════════════════════════════════════
// MÉTHODES PRIVÉES - GÉNÉRATION D'IDS
// ═══════════════════════════════════════════════════════════════════════════════

int Progression::generateNextChordId() const
{
    int maxId = -1;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::CHORD))
        {
            int id = child.getProperty(ModelIdentifiers::id, -1);
            if (id > maxId) maxId = id;
        }
    }
    
    return maxId + 1;
}

juce::ValueTree Progression::createChordNode(Diatony::ChordDegree degree,
                                              Diatony::ChordQuality quality,
                                              Diatony::ChordState chordState)
{
    // L'ID est généré ICI par Progression, pas par Chord
    int chordId = generateNextChordId();
    
    juce::ValueTree chordNode(ModelIdentifiers::CHORD);
    chordNode.setProperty(ModelIdentifiers::id, chordId, nullptr);
    chordNode.setProperty(ModelIdentifiers::degree, static_cast<int>(degree), nullptr);
    chordNode.setProperty(ModelIdentifiers::quality, static_cast<int>(quality), nullptr);
    chordNode.setProperty(ModelIdentifiers::state, static_cast<int>(chordState), nullptr);
    
    return chordNode;
}

void Progression::validateIndex(size_t index) const
{
    if (index >= size())
    {
        jassertfalse;
        throw std::out_of_range("Chord index out of range");
    }
}
