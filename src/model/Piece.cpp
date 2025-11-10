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
        // Récupérer l'ID de la dernière section
        auto lastSection = getSection(getSectionCount() - 1);
        int lastSectionId = lastSection.getState().getProperty(ModelIdentifiers::id, -1);
        
        // Récupérer l'ID de la nouvelle section
        int newSectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
        
        // Ajoute une modulation par défaut entre les sections avec les IDs
        // Les index d'accords -1 indiquent qu'ils seront définis ultérieurement
        addModulation(Diatony::ModulationType::PivotChord, lastSectionId, newSectionId, -1, -1);
    }
    
    state.appendChild(section.getState().createCopy(), &undoManager);
}

void Piece::addSection(const juce::String& sectionName)
{
    // Créer d'abord le nœud de section pour avoir son ID
    auto sectionNode = Section::createSectionNode(sectionName);
    int newSectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    // Si ce n'est pas la première section, on doit d'abord ajouter une modulation
    if (getSectionCount() > 0)
    {
        // Récupérer l'ID de la dernière section
        auto lastSection = getSection(getSectionCount() - 1);
        int lastSectionId = lastSection.getState().getProperty(ModelIdentifiers::id, -1);
        
        // Ajoute une modulation par défaut entre les sections avec les IDs
        // Les index d'accords -1 indiquent qu'ils seront définis ultérieurement
        addModulation(Diatony::ModulationType::PivotChord, lastSectionId, newSectionId, -1, -1);
    }
    
    state.appendChild(sectionNode, &undoManager);
}

Section Piece::createSection(const juce::String& sectionName)
{
    return Section::createIn(state, sectionName);
}

void Piece::removeLastSection()
{
    auto sections = getChildrenOfType(ModelIdentifiers::SECTION);
    if (!sections.empty())
    {
        // Trouve et supprime la dernière section
        for (int i = state.getNumChildren() - 1; i >= 0; --i)
        {
            if (state.getChild(i).hasType(ModelIdentifiers::SECTION))
            {
                state.removeChild(i, &undoManager);
                break;
            }
        }
        
        // Si on avait plus d'une section, supprimer aussi la dernière modulation
        auto modulations = getChildrenOfType(ModelIdentifiers::MODULATION);
        if (sections.size() > 1 && !modulations.empty())
        {
            for (int i = state.getNumChildren() - 1; i >= 0; --i)
            {
                if (state.getChild(i).hasType(ModelIdentifiers::MODULATION))
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

void Piece::addModulation(Diatony::ModulationType type, int fromSectionId, int toSectionId, 
                         int fromChord, int toChord)
{
    auto modulationNode = Modulation::createModulationNode(type, fromSectionId, toSectionId, 
                                                          fromChord, toChord);
    state.appendChild(modulationNode, &undoManager);
}

Modulation Piece::createModulation(Diatony::ModulationType type, int fromSectionId, int toSectionId,
                                  int fromChord, int toChord)
{
    return Modulation::createIn(state, type, fromSectionId, toSectionId, fromChord, toChord);
}

// Accès aux éléments par type
std::vector<Section> Piece::getSections() const
{
    std::vector<Section> sections;
    auto sectionNodes = getChildrenOfType(ModelIdentifiers::SECTION);
    
    for (const auto& node : sectionNodes)
    {
        sections.emplace_back(node);
    }
    
    return sections;
}

std::vector<Modulation> Piece::getModulations() const
{
    std::vector<Modulation> modulations;
    auto modulationNodes = getChildrenOfType(ModelIdentifiers::MODULATION);
    
    for (const auto& node : modulationNodes)
    {
        modulations.emplace_back(node);
    }
    
    return modulations;
}

Section Piece::getSection(size_t index) const
{
    validateSectionIndex(index);
    return Section(getChildOfType(ModelIdentifiers::SECTION, index));
}

Section Piece::getSection(size_t index)
{
    validateSectionIndex(index);
    return Section(getChildOfType(ModelIdentifiers::SECTION, index));
}

Modulation Piece::getModulation(size_t index) const
{
    validateModulationIndex(index);
    return Modulation(getChildOfType(ModelIdentifiers::MODULATION, index));
}

Modulation Piece::getModulation(size_t index)
{
    validateModulationIndex(index);
    return Modulation(getChildOfType(ModelIdentifiers::MODULATION, index));
}

// Accès aux éléments par ID
Section Piece::getSectionById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            int childId = child.getProperty(ModelIdentifiers::id, -1);
            if (childId == id)
            {
                return Section(child);
            }
        }
    }
    
    // Si pas trouvé, retourner une section invalide
    jassertfalse; // En debug, on veut savoir qu'il y a un problème
    return Section(juce::ValueTree(ModelIdentifiers::SECTION));
}

Modulation Piece::getModulationById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::MODULATION))
        {
            int childId = child.getProperty(ModelIdentifiers::id, -1);
            if (childId == id)
            {
                return Modulation(child);
            }
        }
    }
    
    // Si pas trouvé, retourner une modulation invalide
    jassertfalse; // En debug, on veut savoir qu'il y a un problème
    return Modulation(juce::ValueTree(ModelIdentifiers::MODULATION));
}

// Helper pour récupérer les sections adjacentes d'une modulation
std::pair<Section, Section> Piece::getAdjacentSections(const Modulation& modulation) const
{
    int fromSectionId = modulation.getFromSectionId();
    int toSectionId = modulation.getToSectionId();
    
    Section fromSection = getSectionById(fromSectionId);
    Section toSection = getSectionById(toSectionId);
    
    return std::make_pair(fromSection, toSection);
}

// Conversion ID → Index (retourne l'index RELATIF parmi les éléments du même type)
int Piece::getSectionIndexById(int id) const
{
    int sectionIndex = 0;  // Compteur pour les sections uniquement
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            int childId = static_cast<int>(child.getProperty(ModelIdentifiers::id, -1));
            if (childId == id)
                return sectionIndex;  // Retourner l'index relatif
            
            sectionIndex++;  // Incrémenter uniquement si c'est une section
        }
    }
    
    // Si pas trouvé, retourner -1
    return -1;
}

int Piece::getModulationIndexById(int id) const
{
    int modulationIndex = 0;  // Compteur pour les modulations uniquement
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::MODULATION))
        {
            int childId = static_cast<int>(child.getProperty(ModelIdentifiers::id, -1));
            if (childId == id)
                return modulationIndex;  // Retourner l'index relatif
            
            modulationIndex++;  // Incrémenter uniquement si c'est une modulation
        }
    }
    
    // Si pas trouvé, retourner -1
    return -1;
}

// Informations sur la pièce
size_t Piece::getSectionCount() const
{
    return getChildrenOfType(ModelIdentifiers::SECTION).size();
}

size_t Piece::getModulationCount() const
{
    return getChildrenOfType(ModelIdentifiers::MODULATION).size();
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
    state.setProperty(ModelIdentifiers::name, newTitle, &undoManager);
}

const juce::String Piece::getTitle() const
{
    return state.getProperty(ModelIdentifiers::name, "Untitled Piece").toString();
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
    juce::ValueTree pieceNode(ModelIdentifiers::PIECE);
    
    // Générer un ID unique pour cette pièce (commence à 0 pour correspondre aux index)
    static int nextId = 0;
    pieceNode.setProperty(ModelIdentifiers::id, nextId++, nullptr);
    pieceNode.setProperty(ModelIdentifiers::name, title, nullptr);
    
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