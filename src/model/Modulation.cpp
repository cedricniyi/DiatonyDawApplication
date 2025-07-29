#include "Modulation.h"

// Constructeur avec ValueTree existant
Modulation::Modulation(juce::ValueTree state) : state(state)
{
    jassert(state.hasType(ModelIdentifiers::MODULATION));
}

// Méthode statique pour créer une nouvelle Modulation dans un parent
Modulation Modulation::createIn(juce::ValueTree parentTree, Diatony::ModulationType type, 
                                int fromChordIndex, int toChordIndex)
{
    auto modulationNode = createModulationNode(type, fromChordIndex, toChordIndex);
    parentTree.appendChild(modulationNode, nullptr);
    return Modulation(modulationNode);
}

// Setters
void Modulation::setModulationType(Diatony::ModulationType newType)
{
    state.setProperty(ModelIdentifiers::modulationType, modulationTypeToInt(newType), nullptr);
    updateName();
}

void Modulation::setFromChordIndex(int newFromChordIndex)
{
    state.setProperty("fromChordIndex", newFromChordIndex, nullptr);
}

void Modulation::setToChordIndex(int newToChordIndex)
{
    state.setProperty("toChordIndex", newToChordIndex, nullptr);
}

void Modulation::setName(const juce::String& newName)
{
    state.setProperty(ModelIdentifiers::name, newName, nullptr);
}

// Getters
Diatony::ModulationType Modulation::getModulationType() const
{
    return intToModulationType(state.getProperty(ModelIdentifiers::modulationType, 0));
}

int Modulation::getFromChordIndex() const
{
    return state.getProperty("fromChordIndex", -1);
}

int Modulation::getToChordIndex() const
{
    return state.getProperty("toChordIndex", -1);
}

const juce::String Modulation::getName() const
{
    return state.getProperty(ModelIdentifiers::name, "Unknown Modulation").toString();
}

// Méthodes utilitaires
juce::String Modulation::toString() const
{
    if (!isValid())
        return "Invalid Modulation";
        
    return juce::String("Modulation: ") + getName() + 
           " (from: " + juce::String(getFromChordIndex()) + 
           ", to: " + juce::String(getToChordIndex()) + ")";
}

// Création d'un nouveau nœud Modulation
juce::ValueTree Modulation::createModulationNode(Diatony::ModulationType type,
                                                int fromChordIndex,
                                                int toChordIndex)
{
    juce::ValueTree modulationNode(ModelIdentifiers::MODULATION);
    
    // Générer un ID unique pour cette modulation (commence à 0 pour correspondre aux index)
    static int nextId = 0;
    modulationNode.setProperty(ModelIdentifiers::id, nextId++, nullptr);
    
    // Définir les propriétés
    modulationNode.setProperty(ModelIdentifiers::modulationType, modulationTypeToInt(type), nullptr);
    modulationNode.setProperty("fromChordIndex", fromChordIndex, nullptr);
    modulationNode.setProperty("toChordIndex", toChordIndex, nullptr);
    
    // Générer le nom automatiquement
    juce::String name = "Modulation " + juce::String(static_cast<int>(type));
    modulationNode.setProperty(ModelIdentifiers::name, name, nullptr);
    
    return modulationNode;
}

// Helpers pour la conversion des types
int Modulation::modulationTypeToInt(Diatony::ModulationType type)
{
    return static_cast<int>(type);
}

Diatony::ModulationType Modulation::intToModulationType(int value)
{
    return static_cast<Diatony::ModulationType>(value);
}

// Met à jour le nom basé sur les paramètres actuels
void Modulation::updateName()
{
    juce::String newName = "Modulation " + juce::String(static_cast<int>(getModulationType()));
    state.setProperty(ModelIdentifiers::name, newName, nullptr);
} 