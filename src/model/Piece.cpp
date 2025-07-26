#include "Piece.h"

// Constructeurs
Piece::Piece()
{
    state = createPieceNode("Untitled Piece");
}

Piece::Piece(const juce::String& pieceTitle)
{
    state = createPieceNode(pieceTitle);
}

// Gestion des sections
void Piece::addSection(const Section& section)
{
    // Si ce n'est pas la première section, on doit d'abord ajouter une modulation
    if (getSectionCount() > 0)
    {
        // Ajoute une modulation par défaut entre les sections
        // Les index -1 indiquent une modulation symbolique à définir ultérieurement
        addModulation(Diatony::ModulationType::PerfectCadence, -1, -1);
    }
    
    state.appendChild(section.getState().createCopy(), &undoManager);
}

void Piece::addSection(const juce::String& sectionName)
{
    // Si ce n'est pas la première section, on doit d'abord ajouter une modulation
    if (getSectionCount() > 0)
    {
        // Ajoute une modulation par défaut entre les sections
        // Les index -1 indiquent une modulation symbolique à définir ultérieurement
        addModulation(Diatony::ModulationType::PerfectCadence, -1, -1);
    }
    
    auto sectionNode = Section::createSectionNode(sectionName);
    state.appendChild(sectionNode, &undoManager);
}

Section Piece::createSection(const juce::String& sectionName)
{
    return Section::createIn(state, sectionName);
}

void Piece::removeLastSection()
{
    auto sections = getChildrenOfType(Identifiers::SECTION);
    if (!sections.empty())
    {
        // Trouve et supprime la dernière section
        for (int i = state.getNumChildren() - 1; i >= 0; --i)
        {
            if (state.getChild(i).hasType(Identifiers::SECTION))
            {
                state.removeChild(i, &undoManager);
                break;
            }
        }
        
        // Si on avait plus d'une section, supprimer aussi la dernière modulation
        auto modulations = getChildrenOfType(Identifiers::MODULATION);
        if (sections.size() > 1 && !modulations.empty())
        {
            for (int i = state.getNumChildren() - 1; i >= 0; --i)
            {
                if (state.getChild(i).hasType(Identifiers::MODULATION))
                {
                    state.removeChild(i, &undoManager);
                    break;
                }
            }
        }
    }
}

void Piece::clearAllElements()
{
    state.removeAllChildren(&undoManager);
}

void Piece::clear()
{
    clearAllElements();
}

// Gestion des modulations
void Piece::addModulation(const Modulation& modulation)
{
    state.appendChild(modulation.getState().createCopy(), &undoManager);
}

void Piece::addModulation(Diatony::ModulationType type, int fromChord, int toChord)
{
    auto modulationNode = Modulation::createModulationNode(type, fromChord, toChord);
    state.appendChild(modulationNode, &undoManager);
}

Modulation Piece::createModulation(Diatony::ModulationType type, int fromChord, int toChord)
{
    return Modulation::createIn(state, type, fromChord, toChord);
}

// Accès aux éléments par type
std::vector<Section> Piece::getSections() const
{
    std::vector<Section> sections;
    auto sectionNodes = getChildrenOfType(Identifiers::SECTION);
    
    for (const auto& node : sectionNodes)
    {
        sections.emplace_back(node);
    }
    
    return sections;
}

std::vector<Modulation> Piece::getModulations() const
{
    std::vector<Modulation> modulations;
    auto modulationNodes = getChildrenOfType(Identifiers::MODULATION);
    
    for (const auto& node : modulationNodes)
    {
        modulations.emplace_back(node);
    }
    
    return modulations;
}

Section Piece::getSection(size_t index) const
{
    validateSectionIndex(index);
    return Section(getChildOfType(Identifiers::SECTION, index));
}

Section Piece::getSection(size_t index)
{
    validateSectionIndex(index);
    return Section(getChildOfType(Identifiers::SECTION, index));
}

Modulation Piece::getModulation(size_t index) const
{
    validateModulationIndex(index);
    return Modulation(getChildOfType(Identifiers::MODULATION, index));
}

Modulation Piece::getModulation(size_t index)
{
    validateModulationIndex(index);
    return Modulation(getChildOfType(Identifiers::MODULATION, index));
}

// Informations sur la pièce
size_t Piece::getSectionCount() const
{
    return getChildrenOfType(Identifiers::SECTION).size();
}

size_t Piece::getModulationCount() const
{
    return getChildrenOfType(Identifiers::MODULATION).size();
}

int Piece::getNumElements() const
{
    return state.getNumChildren();
}

bool Piece::isEmpty() const
{
    return state.getNumChildren() == 0;
}

void Piece::setTitle(const juce::String& newTitle)
{
    state.setProperty(Identifiers::name, newTitle, &undoManager);
}

const juce::String Piece::getTitle() const
{
    return state.getProperty(Identifiers::name, "Untitled Piece").toString();
}

// Validation
bool Piece::isComplete() const
{
    return !isEmpty() && hasValidStructure();
}

bool Piece::hasValidStructure() const
{
    // Vérifie l'invariant : modulations.size() == sections.size() - 1
    auto sectionCount = getSectionCount();
    auto modulationCount = getModulationCount();
    
    if (sectionCount == 0)
        return true; // Pièce vide est valide
        
    return modulationCount == (sectionCount - 1);
}

// Statistiques
int Piece::getTotalChordCount() const
{
    int totalChords = 0;
    auto sections = getSections();
    
    for (const auto& section : sections)
    {
        totalChords += static_cast<int>(section.getProgression().size());
    }
    
    return totalChords;
}

// Méthodes utilitaires
juce::String Piece::toString() const
{
    if (isEmpty())
        return getTitle() + " (Empty)";
        
    return getTitle() + " (" + 
           juce::String(getSectionCount()) + " sections, " +
           juce::String(getModulationCount()) + " modulations, " +
           juce::String(getTotalChordCount()) + " total chords)";
}

juce::String Piece::getDetailedSummary() const
{
    juce::String result = "=== " + getTitle() + " ===\n";
    result += "Structure: " + juce::String(getSectionCount()) + " sections, " +
              juce::String(getModulationCount()) + " modulations\n";
    result += "Total chords: " + juce::String(getTotalChordCount()) + "\n";
    result += juce::String("Valid structure: ") + (hasValidStructure() ? "Yes" : "No") + "\n\n";
    
    if (!isEmpty())
    {
        result += "Elements:\n";
        auto sections = getSections();
        auto modulations = getModulations();
        
        for (size_t i = 0; i < sections.size(); ++i)
        {
            result += juce::String(i + 1) + ". " + sections[i].toString() + "\n";
            
            if (i < modulations.size())
            {
                result += "   -> " + modulations[i].toString() + "\n";
            }
        }
    }
    
    return result;
}

// Création d'un nouveau nœud Piece
juce::ValueTree Piece::createPieceNode(const juce::String& title)
{
    juce::ValueTree pieceNode(Identifiers::PIECE);
    
    // Générer un ID unique pour cette pièce
    static int nextId = 1;
    pieceNode.setProperty(Identifiers::id, nextId++, nullptr);
    pieceNode.setProperty(Identifiers::name, title, nullptr);
    
    return pieceNode;
}

// Helpers pour naviguer dans l'arborescence
std::vector<juce::ValueTree> Piece::getChildrenOfType(const juce::Identifier& type) const
{
    std::vector<juce::ValueTree> children;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(type))
        {
            children.push_back(child);
        }
    }
    
    return children;
}

juce::ValueTree Piece::getChildOfType(const juce::Identifier& type, size_t index) const
{
    auto children = getChildrenOfType(type);
    
    if (index >= children.size())
    {
        jassertfalse;
        return juce::ValueTree(); // Retourne un ValueTree invalide
    }
    
    return children[index];
}

// Validation des index
void Piece::validateSectionIndex(size_t index) const
{
    if (index >= getSectionCount())
    {
        jassertfalse;
        throw std::out_of_range("Section index out of range");
    }
}

void Piece::validateModulationIndex(size_t index) const
{
    if (index >= getModulationCount())
    {
        jassertfalse;
        throw std::out_of_range("Modulation index out of range");
    }
}