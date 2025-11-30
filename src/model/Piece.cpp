#include "Piece.h"

// ═══════════════════════════════════════════════════════════════════════════════
// CONSTRUCTEURS
// ═══════════════════════════════════════════════════════════════════════════════

Piece::Piece()
{
    state = juce::ValueTree(ModelIdentifiers::PIECE);
    state.setProperty(ModelIdentifiers::id, 0, nullptr);
    state.setProperty(ModelIdentifiers::name, "Untitled Piece", nullptr);
}

Piece::Piece(const juce::String& pieceTitle)
{
    state = juce::ValueTree(ModelIdentifiers::PIECE);
    state.setProperty(ModelIdentifiers::id, 0, nullptr);
    state.setProperty(ModelIdentifiers::name, pieceTitle, nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════════
// GESTION DES SECTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void Piece::addSection(const juce::String& sectionName)
{
    // 1. Créer la nouvelle section avec un ID généré par Piece
    auto sectionNode = createSectionNode(sectionName);
    int newSectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    DBG("[Piece::addSection] Création section '" << sectionName << "' avec ID=" << newSectionId);
    
    // 2. Si ce n'est pas la première section, créer d'abord une modulation
    if (getSectionCount() > 0)
    {
        // Récupérer l'ID de la dernière section existante
        auto lastSection = getSection(getSectionCount() - 1);
        int lastSectionId = lastSection.getState().getProperty(ModelIdentifiers::id, -1);
        
        // Créer la modulation (elle sera ajoutée AVANT la nouvelle section)
        auto modulationNode = createModulationNode(lastSectionId, newSectionId);
        
        DBG("[Piece::addSection] Création modulation " << lastSectionId << " -> " << newSectionId);
        
        // Ajouter la modulation
        state.appendChild(modulationNode, &undoManager);
    }
    
    // 3. Ajouter la section
    state.appendChild(sectionNode, &undoManager);
    
    // 4. Vérifier les invariants
    assertInvariants();
    
    DBG("[Piece::addSection] Structure: " << getSectionCount() << " sections, " << getModulationCount() << " modulations");
}

void Piece::removeSection(int sectionIndex)
{
    auto sectionCount = static_cast<int>(getSectionCount());
    
    // Validation
    if (sectionIndex < 0 || sectionIndex >= sectionCount)
    {
        DBG("[Piece::removeSection] Index invalide: " << sectionIndex << " (count=" << sectionCount << ")");
        return;
    }
    
    // Récupérer les informations AVANT toute modification
    auto sectionToRemove = getSection(static_cast<size_t>(sectionIndex));
    int sectionId = sectionToRemove.getState().getProperty(ModelIdentifiers::id, -1);
    int sectionVTIndex = findValueTreeIndex(ModelIdentifiers::SECTION, sectionId);
    
    DBG("[Piece::removeSection] Suppression section index=" << sectionIndex << " id=" << sectionId << " vtIndex=" << sectionVTIndex);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // CAS 1: Section unique → Supprimer juste la section
    // ═══════════════════════════════════════════════════════════════════════════
    if (sectionCount == 1)
    {
        state.removeChild(sectionVTIndex, &undoManager);
        assertInvariants();
        return;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // CAS 2-4: Plusieurs sections → Gérer les modulations
    // ═══════════════════════════════════════════════════════════════════════════
    
    // Identifier les sections adjacentes
    int prevSectionId = -1;
    int nextSectionId = -1;
    
    if (sectionIndex > 0)
    {
        prevSectionId = getSection(static_cast<size_t>(sectionIndex - 1))
                            .getState().getProperty(ModelIdentifiers::id, -1);
    }
    
    if (sectionIndex < sectionCount - 1)
    {
        nextSectionId = getSection(static_cast<size_t>(sectionIndex + 1))
                            .getState().getProperty(ModelIdentifiers::id, -1);
    }
    
    // Collecter les ValueTree indices à supprimer (en ordre décroissant pour éviter les décalages)
    std::vector<int> indicesToRemove;
    
    // Parcourir le ValueTree pour trouver tout ce qui doit être supprimé
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            int childId = child.getProperty(ModelIdentifiers::id, -1);
            if (childId == sectionId)
            {
                indicesToRemove.push_back(i);
            }
        }
        else if (child.hasType(ModelIdentifiers::MODULATION))
        {
            int fromId = child.getProperty(ModelIdentifiers::fromSectionId, -1);
            int toId = child.getProperty(ModelIdentifiers::toSectionId, -1);
            
            // Supprimer les modulations qui référencent la section à supprimer
            if (fromId == sectionId || toId == sectionId)
            {
                indicesToRemove.push_back(i);
            }
        }
    }
    
    // Trier en ordre décroissant pour supprimer de la fin vers le début
    std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());
    
    // Supprimer tous les éléments
    for (int idx : indicesToRemove)
    {
        DBG("[Piece::removeSection] Suppression élément à l'index VT=" << idx);
        state.removeChild(idx, &undoManager);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Si c'était une section du MILIEU, créer une nouvelle modulation de liaison
    // ═══════════════════════════════════════════════════════════════════════════
    if (prevSectionId >= 0 && nextSectionId >= 0)
    {
        DBG("[Piece::removeSection] Création modulation de liaison: " << prevSectionId << " -> " << nextSectionId);
        
        // Créer la nouvelle modulation
        auto modulationNode = createModulationNode(prevSectionId, nextSectionId);
        
        // Trouver la position d'insertion (juste après la section précédente)
        int prevSectionVTIndex = findValueTreeIndex(ModelIdentifiers::SECTION, prevSectionId);
        int insertIndex = prevSectionVTIndex + 1;
        
        DBG("[Piece::removeSection] Insertion modulation à l'index VT=" << insertIndex);
        state.addChild(modulationNode, insertIndex, &undoManager);
    }
    
    assertInvariants();
    
    DBG("[Piece::removeSection] Structure finale: " << getSectionCount() << " sections, " << getModulationCount() << " modulations");
}

void Piece::removeLastSection()
{
    if (getSectionCount() > 0)
    {
        removeSection(static_cast<int>(getSectionCount()) - 1);
    }
}

void Piece::clear()
{
    state.removeAllChildren(&undoManager);
    assertInvariants();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ACCÈS AUX ÉLÉMENTS
// ═══════════════════════════════════════════════════════════════════════════════

Section Piece::getSection(size_t index) const
{
    auto sectionNode = getChildOfType(ModelIdentifiers::SECTION, index);
    jassert(sectionNode.isValid());
    return Section(sectionNode);
}

Modulation Piece::getModulation(size_t index) const
{
    auto modulationNode = getChildOfType(ModelIdentifiers::MODULATION, index);
    jassert(modulationNode.isValid());
    return Modulation(modulationNode);
}

Section Piece::getSectionById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return Section(child);
            }
        }
    }
    
    jassertfalse; // Section non trouvée
    return Section(juce::ValueTree(ModelIdentifiers::SECTION));
}

Modulation Piece::getModulationById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::MODULATION))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return Modulation(child);
            }
        }
    }
    
    jassertfalse; // Modulation non trouvée
    return Modulation(juce::ValueTree(ModelIdentifiers::MODULATION));
}

int Piece::getSectionIndexById(int id) const
{
    int sectionIndex = 0;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return sectionIndex;
            }
            sectionIndex++;
        }
    }
    
    return -1; // Non trouvé
}

int Piece::getModulationIndexById(int id) const
{
    int modulationIndex = 0;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::MODULATION))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return modulationIndex;
            }
            modulationIndex++;
        }
    }
    
    return -1; // Non trouvé
}

std::vector<Section> Piece::getSections() const
{
    std::vector<Section> sections;
    auto nodes = getChildrenOfType(ModelIdentifiers::SECTION);
    
    for (const auto& node : nodes)
    {
        sections.emplace_back(node);
    }
    
    return sections;
}

std::vector<Modulation> Piece::getModulations() const
{
    std::vector<Modulation> modulations;
    auto nodes = getChildrenOfType(ModelIdentifiers::MODULATION);
    
    for (const auto& node : nodes)
    {
        modulations.emplace_back(node);
    }
    
    return modulations;
}

std::pair<Section, Section> Piece::getAdjacentSections(const Modulation& modulation) const
{
    return std::make_pair(
        getSectionById(modulation.getFromSectionId()),
        getSectionById(modulation.getToSectionId())
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
// INFORMATIONS
// ═══════════════════════════════════════════════════════════════════════════════

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

juce::String Piece::getTitle() const
{
    return state.getProperty(ModelIdentifiers::name, "Untitled Piece").toString();
}

// ═══════════════════════════════════════════════════════════════════════════════
// VALIDATION
// ═══════════════════════════════════════════════════════════════════════════════

bool Piece::hasValidStructure() const
{
    auto sectionCount = getSectionCount();
    auto modulationCount = getModulationCount();
    
    // Invariant 1: modulations == sections - 1 (ou 0 si vide)
    if (sectionCount == 0)
    {
        return modulationCount == 0;
    }
    
    return modulationCount == (sectionCount - 1);
}

bool Piece::isComplete() const
{
    return !isEmpty() && hasValidStructure();
}

void Piece::assertInvariants() const
{
#if JUCE_DEBUG
    auto sectionCount = getSectionCount();
    auto modulationCount = getModulationCount();
    
    // Invariant 1: modulations == sections - 1
    if (sectionCount == 0)
    {
        jassert(modulationCount == 0);
    }
    else
    {
        jassert(modulationCount == sectionCount - 1);
    }
    
    // Invariant 2: Vérifier l'alternance S-M-S-M-S dans le ValueTree
    if (sectionCount > 1)
    {
        bool expectSection = true;
        for (int i = 0; i < state.getNumChildren(); ++i)
        {
            auto child = state.getChild(i);
            
            if (expectSection)
            {
                jassert(child.hasType(ModelIdentifiers::SECTION));
            }
            else
            {
                jassert(child.hasType(ModelIdentifiers::MODULATION));
            }
            
            expectSection = !expectSection;
        }
    }
    
    DBG("[Piece::assertInvariants] ✓ Tous les invariants respectés");
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATISTIQUES ET DEBUG
// ═══════════════════════════════════════════════════════════════════════════════

int Piece::getTotalChordCount() const
{
    int total = 0;
    for (const auto& section : getSections())
    {
        total += static_cast<int>(section.getProgression().size());
    }
    return total;
}

juce::String Piece::toString() const
{
    if (isEmpty())
        return getTitle() + " (Empty)";
    
    return getTitle() + " (" + 
           juce::String(getSectionCount()) + " sections, " +
           juce::String(getModulationCount()) + " modulations, " +
           juce::String(getTotalChordCount()) + " chords)";
}

juce::String Piece::getDetailedSummary() const
{
    juce::String result = "=== " + getTitle() + " ===\n";
    result += "Structure: " + juce::String(getSectionCount()) + " sections, " +
              juce::String(getModulationCount()) + " modulations\n";
    result += "Valid: " + juce::String(hasValidStructure() ? "Yes" : "NO!") + "\n\n";
    
    if (!isEmpty())
    {
        result += "ValueTree order:\n";
        for (int i = 0; i < state.getNumChildren(); ++i)
        {
            auto child = state.getChild(i);
            int id = child.getProperty(ModelIdentifiers::id, -1);
            result += "  [" + juce::String(i) + "] " + child.getType().toString() + " (id=" + juce::String(id) + ")\n";
        }
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// GÉNÉRATION D'IDS (CENTRALISÉE)
// ═══════════════════════════════════════════════════════════════════════════════

int Piece::generateNextSectionId() const
{
    int maxId = -1;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            int id = child.getProperty(ModelIdentifiers::id, -1);
            if (id > maxId) maxId = id;
        }
    }
    
    return maxId + 1;
}

int Piece::generateNextModulationId() const
{
    int maxId = -1;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::MODULATION))
        {
            int id = child.getProperty(ModelIdentifiers::id, -1);
            if (id > maxId) maxId = id;
        }
    }
    
    return maxId + 1;
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPERS INTERNES
// ═══════════════════════════════════════════════════════════════════════════════

juce::ValueTree Piece::createSectionNode(const juce::String& name)
{
    // L'ID est généré ICI par Piece, pas par Section
    int sectionId = generateNextSectionId();
    
    juce::ValueTree sectionNode(ModelIdentifiers::SECTION);
    sectionNode.setProperty(ModelIdentifiers::id, sectionId, nullptr);
    sectionNode.setProperty(ModelIdentifiers::name, name, nullptr);
    sectionNode.setProperty(ModelIdentifiers::tonalityNote, 0, nullptr);  // C par défaut
    sectionNode.setProperty(ModelIdentifiers::tonalityAlteration, 0, nullptr);  // Natural
    sectionNode.setProperty(ModelIdentifiers::isMajor, true, nullptr);
    
    // Créer une progression vide
    juce::ValueTree progressionNode(ModelIdentifiers::PROGRESSION);
    progressionNode.setProperty(ModelIdentifiers::id, 0, nullptr);
    sectionNode.appendChild(progressionNode, nullptr);
    
    return sectionNode;
}

juce::ValueTree Piece::createModulationNode(int fromSectionId, int toSectionId)
{
    // L'ID est généré ICI par Piece, pas par Modulation
    int modulationId = generateNextModulationId();
    
    juce::ValueTree modulationNode(ModelIdentifiers::MODULATION);
    modulationNode.setProperty(ModelIdentifiers::id, modulationId, nullptr);
    modulationNode.setProperty(ModelIdentifiers::modulationType, 
                               static_cast<int>(Diatony::ModulationType::PivotChord), nullptr);
    modulationNode.setProperty(ModelIdentifiers::fromSectionId, fromSectionId, nullptr);
    modulationNode.setProperty(ModelIdentifiers::toSectionId, toSectionId, nullptr);
    modulationNode.setProperty(ModelIdentifiers::fromChordIndex, -1, nullptr);
    modulationNode.setProperty(ModelIdentifiers::toChordIndex, -1, nullptr);
    modulationNode.setProperty(ModelIdentifiers::name, 
                               "Modulation " + juce::String(modulationId), nullptr);
    
    return modulationNode;
}

int Piece::findValueTreeIndex(const juce::Identifier& type, int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(type))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            {
                return i;
            }
        }
    }
    return -1;
}

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
        return juce::ValueTree();
    }
    
    return children[index];
}
