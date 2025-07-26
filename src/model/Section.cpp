#include "Section.h"

// Constructeur avec ValueTree existant
Section::Section(juce::ValueTree state) : state(state)
{
    jassert(state.hasType(Identifiers::SECTION));
    ensureProgressionExists();
}

// Méthode statique pour créer une nouvelle Section dans un parent
Section Section::createIn(juce::ValueTree parentTree, const juce::String& sectionName)
{
    auto sectionNode = createSectionNode(sectionName);
    parentTree.appendChild(sectionNode, nullptr);
    return Section(sectionNode);
}

// Setters
void Section::setNote(Diatony::Note newNote)
{
    state.setProperty(Identifiers::tonalityNote, noteToInt(newNote), nullptr);
}

void Section::setAlteration(Diatony::Alteration newAlteration)
{
    state.setProperty(Identifiers::tonalityAlteration, alterationToInt(newAlteration), nullptr);
}

void Section::setIsMajor(bool newIsMajor)
{
    state.setProperty(Identifiers::isMajor, newIsMajor, nullptr);
}

void Section::setName(const juce::String& newName)
{
    state.setProperty(Identifiers::name, newName, nullptr);
}

void Section::setProgression(const Progression& newProgression)
{
    // Remplace le nœud progression existant
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        if (state.getChild(i).hasType(Identifiers::PROGRESSION))
        {
            state.removeChild(i, nullptr);
            break;
        }
    }
    
    // Ajoute la nouvelle progression
    state.appendChild(newProgression.getState().createCopy(), nullptr);
}

// Getters
Diatony::Note Section::getNote() const
{
    return intToNote(state.getProperty(Identifiers::tonalityNote, 0));
}

Diatony::Alteration Section::getAlteration() const
{
    return intToAlteration(state.getProperty(Identifiers::tonalityAlteration, 0));
}

bool Section::getIsMajor() const
{
    return state.getProperty(Identifiers::isMajor, true);
}

const juce::String Section::getName() const
{
    return state.getProperty(Identifiers::name, "Unnamed Section").toString();
}

// Accès à la progression
Progression Section::getProgression() const
{
    // Trouve le child progression
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(Identifiers::PROGRESSION))
        {
            return Progression(child);
        }
    }
    
    // Si pas trouvé, retourne une progression vide (cela ne devrait pas arriver si ensureProgressionExists() fonctionne)
    jassert(false);
    return Progression(juce::ValueTree(Identifiers::PROGRESSION));
}

Progression Section::getProgression()
{
    // Version non-const, même logique
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(Identifiers::PROGRESSION))
        {
            return Progression(child);
        }
    }
    
    jassert(false);
    return Progression(juce::ValueTree(Identifiers::PROGRESSION));
}

// Méthodes utilitaires
bool Section::isEmpty() const
{
    return !isValid() || getProgression().isEmpty();
}

bool Section::isValidSection() const
{
    return isValid() && 
           !getName().isEmpty() &&
           getProgression().isValid();
}

juce::String Section::toString() const
{
    if (!isValid())
        return "Invalid Section";
        
    juce::String result = getName() + " (";
    result += juce::String(static_cast<int>(getNote()));
    result += juce::String(static_cast<int>(getAlteration()));
    result += getIsMajor() ? " Major" : " Minor";
    result += ") - " + getProgression().toString();
    
    return result;
}

// Création d'un nouveau nœud Section
juce::ValueTree Section::createSectionNode(const juce::String& sectionName)
{
    juce::ValueTree sectionNode(Identifiers::SECTION);
    
    // Générer un ID unique pour cette section
    static int nextId = 1;
    sectionNode.setProperty(Identifiers::id, nextId++, nullptr);
    
    // Définir les propriétés par défaut
    sectionNode.setProperty(Identifiers::name, sectionName, nullptr);
    sectionNode.setProperty(Identifiers::tonalityNote, noteToInt(Diatony::Note::C), nullptr);
    sectionNode.setProperty(Identifiers::tonalityAlteration, alterationToInt(Diatony::Alteration::Natural), nullptr);
    sectionNode.setProperty(Identifiers::isMajor, true, nullptr);
    
    // Ajouter une progression vide
    auto progressionNode = Progression::createProgressionNode();
    sectionNode.appendChild(progressionNode, nullptr);
    
    return sectionNode;
}

// Helpers pour la conversion des types
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

// Assure qu'un nœud progression existe
void Section::ensureProgressionExists()
{
    // Vérifie si une progression existe déjà
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        if (state.getChild(i).hasType(Identifiers::PROGRESSION))
        {
            return; // Progression trouvée, rien à faire
        }
    }
    
    // Aucune progression trouvée, en créer une
    auto progressionNode = Progression::createProgressionNode();
    state.appendChild(progressionNode, nullptr);
} 