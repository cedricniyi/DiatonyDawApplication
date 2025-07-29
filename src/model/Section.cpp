#include "Section.h"

// Constructeur avec ValueTree existant
Section::Section(juce::ValueTree state) : state(state)
{
    jassert(state.hasType(ModelIdentifiers::SECTION));
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
    state.setProperty(ModelIdentifiers::tonalityNote, noteToInt(newNote), nullptr);
}

void Section::setAlteration(Diatony::Alteration newAlteration)
{
    state.setProperty(ModelIdentifiers::tonalityAlteration, alterationToInt(newAlteration), nullptr);
}

void Section::setIsMajor(bool newIsMajor)
{
    state.setProperty(ModelIdentifiers::isMajor, newIsMajor, nullptr);
}

void Section::setName(const juce::String& newName)
{
    state.setProperty(ModelIdentifiers::name, newName, nullptr);
}

void Section::setProgression(const Progression& newProgression)
{
    // Remplace le nœud progression existant
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        if (state.getChild(i).hasType(ModelIdentifiers::PROGRESSION))
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

const juce::String Section::getName() const
{
    return state.getProperty(ModelIdentifiers::name, "Unnamed Section").toString();
}

// Accès à la progression
Progression Section::getProgression() const
{
    // Trouve le child progression
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::PROGRESSION))
        {
            return Progression(child);
        }
    }
    
    // Si pas trouvé, retourne une progression vide (cela ne devrait pas arriver si ensureProgressionExists() fonctionne)
    jassert(false);
    return Progression(juce::ValueTree(ModelIdentifiers::PROGRESSION));
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
    
    jassert(false);
    return Progression(juce::ValueTree(ModelIdentifiers::PROGRESSION));
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
    juce::ValueTree sectionNode(ModelIdentifiers::SECTION);
    
    // Générer un ID unique pour cette section
    static int nextId = 1;
    sectionNode.setProperty(ModelIdentifiers::id, nextId++, nullptr);
    
    // Définir les propriétés par défaut
    sectionNode.setProperty(ModelIdentifiers::name, sectionName, nullptr);
    sectionNode.setProperty(ModelIdentifiers::tonalityNote, noteToInt(Diatony::Note::C), nullptr);
    sectionNode.setProperty(ModelIdentifiers::tonalityAlteration, alterationToInt(Diatony::Alteration::Natural), nullptr);
    sectionNode.setProperty(ModelIdentifiers::isMajor, true, nullptr);
    
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
        if (state.getChild(i).hasType(ModelIdentifiers::PROGRESSION))
        {
            return; // Progression trouvée, rien à faire
        }
    }
    
    // Aucune progression trouvée, en créer une
    auto progressionNode = Progression::createProgressionNode();
    state.appendChild(progressionNode, nullptr);
} 