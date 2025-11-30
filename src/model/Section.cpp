#include "Section.h"

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════════

Section::Section(juce::ValueTree state) : state(state)
{
    // En mode debug, vérifier que le ValueTree est du bon type
    jassert(!state.isValid() || state.hasType(ModelIdentifiers::SECTION));
}

// ═══════════════════════════════════════════════════════════════════════════════
// SETTERS
// ═══════════════════════════════════════════════════════════════════════════════

void Section::setNote(Diatony::Note newNote)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::tonalityNote, noteToInt(newNote), nullptr);
}

void Section::setAlteration(Diatony::Alteration newAlteration)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::tonalityAlteration, alterationToInt(newAlteration), nullptr);
}

void Section::setIsMajor(bool newIsMajor)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::isMajor, newIsMajor, nullptr);
}

void Section::setName(const juce::String& newName)
{
    if (state.isValid())
        state.setProperty(ModelIdentifiers::name, newName, nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════════
// GETTERS
// ═══════════════════════════════════════════════════════════════════════════════

int Section::getId() const
{
    return state.getProperty(ModelIdentifiers::id, -1);
}

Diatony::Note Section::getNote() const
{
    return intToNote(state.getProperty(ModelIdentifiers::tonalityNote, 0));
}

Diatony::Alteration Section::getAlteration() const
{
    return intToAlteration(state.getProperty(ModelIdentifiers::tonalityAlteration, 0));
}

bool Section::getIsMajor() const
{
    return state.getProperty(ModelIdentifiers::isMajor, true);
}

juce::String Section::getName() const
{
    return state.getProperty(ModelIdentifiers::name, "Unnamed Section").toString();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ACCÈS À LA PROGRESSION
// ═══════════════════════════════════════════════════════════════════════════════

Progression Section::getProgression() const
{
    // Chercher le premier enfant de type PROGRESSION
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::PROGRESSION))
        {
            return Progression(child);
        }
    }
    
    // Si pas trouvé, retourner un wrapper invalide
    return Progression(juce::ValueTree());
}

Progression Section::getProgression()
{
    // Version non-const, même logique
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::PROGRESSION))
        {
            return Progression(child);
        }
    }
    
    return Progression(juce::ValueTree());
}

// ═══════════════════════════════════════════════════════════════════════════════
// MÉTHODES UTILITAIRES
// ═══════════════════════════════════════════════════════════════════════════════

bool Section::isEmpty() const
{
    if (!isValid()) return true;
    
    auto progression = getProgression();
    return !progression.isValid() || progression.isEmpty();
}

bool Section::hasProgression() const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        if (state.getChild(i).hasType(ModelIdentifiers::PROGRESSION))
            return true;
    }
    return false;
}

juce::String Section::toString() const
{
    if (!isValid())
        return "Invalid Section";
    
    juce::String result = getName();
    result += " (ID=" + juce::String(getId()) + ", ";
    result += getIsMajor() ? "Major" : "Minor";
    result += ")";
    
    auto progression = getProgression();
    if (progression.isValid())
    {
        result += " - " + juce::String(progression.size()) + " chords";
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPERS DE CONVERSION
// ═══════════════════════════════════════════════════════════════════════════════

int Section::noteToInt(Diatony::Note note)
{
    return static_cast<int>(note);
}

Diatony::Note Section::intToNote(int value)
{
    return static_cast<Diatony::Note>(value);
}

int Section::alterationToInt(Diatony::Alteration alteration)
{
    return static_cast<int>(alteration);
}

Diatony::Alteration Section::intToAlteration(int value)
{
    return static_cast<Diatony::Alteration>(value);
}
