#include "Progression.h"

Progression::Progression(juce::ValueTree state) : state(state)
{
    jassert(!state.isValid() || state.hasType(ModelIdentifiers::PROGRESSION));
}

void Progression::addChord(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState chordState)
{
    if (!state.isValid()) return;
    state.appendChild(createChordNode(degree, quality, chordState), nullptr);
}

void Progression::insertChord(size_t index, Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState chordState)
{
    if (!state.isValid()) return;
    if (index > size()) index = size();
    state.addChild(createChordNode(degree, quality, chordState), static_cast<int>(index), nullptr);
}

void Progression::removeChord(size_t index)
{
    if (!state.isValid()) return;
    validateIndex(index);
    state.removeChild(static_cast<int>(index), nullptr);
}

void Progression::clear()
{
    if (state.isValid())
        state.removeAllChildren(nullptr);
}

Chord Progression::getChord(size_t index) const { validateIndex(index); return Chord(state.getChild(static_cast<int>(index))); }
Chord Progression::getChord(size_t index) { validateIndex(index); return Chord(state.getChild(static_cast<int>(index))); }

Chord Progression::getChordById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::CHORD) && static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            return Chord(child);
    }
    return Chord(juce::ValueTree());
}

int Progression::getChordIndexById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::CHORD) && static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            return i;
    }
    return -1;
}

size_t Progression::size() const { return static_cast<size_t>(state.getNumChildren()); }
bool Progression::isEmpty() const { return state.getNumChildren() == 0; }
int Progression::getId() const { return state.getProperty(ModelIdentifiers::id, -1); }

juce::String Progression::toString() const
{
    if (!isValid()) return "Invalid Progression";
    if (isEmpty()) return "Progression (empty)";
    return "Progression (ID=" + juce::String(getId()) + ", " + juce::String(size()) + " chords)";
}

int Progression::generateNextChordId() const
{
    int maxId = -1;
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::CHORD))
            maxId = std::max(maxId, static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)));
    }
    return maxId + 1;
}

juce::ValueTree Progression::createChordNode(Diatony::ChordDegree degree, Diatony::ChordQuality quality, Diatony::ChordState chordState)
{
    juce::ValueTree chordNode(ModelIdentifiers::CHORD);
    chordNode.setProperty(ModelIdentifiers::id, generateNextChordId(), nullptr);
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
