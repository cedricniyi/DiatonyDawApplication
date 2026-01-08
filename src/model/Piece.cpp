#include "Piece.h"

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

void Piece::addSection(const juce::String& sectionName)
{
    auto sectionNode = createSectionNode(sectionName);
    int newSectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    // Créer une modulation de liaison si ce n'est pas la première section
    if (getSectionCount() > 0)
    {
        auto lastSection = getSection(getSectionCount() - 1);
        int lastSectionId = lastSection.getState().getProperty(ModelIdentifiers::id, -1);
        auto modulationNode = createModulationNode(lastSectionId, newSectionId);
        state.appendChild(modulationNode, &undoManager);
    }
    
    state.appendChild(sectionNode, &undoManager);
    assertInvariants();
}

void Piece::removeSection(int sectionIndex)
{
    auto sectionCount = static_cast<int>(getSectionCount());
    if (sectionIndex < 0 || sectionIndex >= sectionCount)
        return;
    
    auto sectionToRemove = getSection(static_cast<size_t>(sectionIndex));
    int sectionId = sectionToRemove.getState().getProperty(ModelIdentifiers::id, -1);
    int sectionVTIndex = findValueTreeIndex(ModelIdentifiers::SECTION, sectionId);
    
    // Cas simple : section unique
    if (sectionCount == 1)
    {
        state.removeChild(sectionVTIndex, &undoManager);
        assertInvariants();
        return;
    }
    
    // Identifier les sections adjacentes pour la modulation de liaison
    int prevSectionId = -1, nextSectionId = -1;
    if (sectionIndex > 0)
        prevSectionId = getSection(static_cast<size_t>(sectionIndex - 1)).getState().getProperty(ModelIdentifiers::id, -1);
    if (sectionIndex < sectionCount - 1)
        nextSectionId = getSection(static_cast<size_t>(sectionIndex + 1)).getState().getProperty(ModelIdentifiers::id, -1);
    
    // Collecter indices à supprimer (ordre décroissant pour éviter décalages)
    std::vector<int> indicesToRemove;
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            if (static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == sectionId)
                indicesToRemove.push_back(i);
        }
        else if (child.hasType(ModelIdentifiers::MODULATION))
        {
            int fromId = child.getProperty(ModelIdentifiers::fromSectionId, -1);
            int toId = child.getProperty(ModelIdentifiers::toSectionId, -1);
            if (fromId == sectionId || toId == sectionId)
                indicesToRemove.push_back(i);
        }
    }
    
    std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());
    for (int idx : indicesToRemove)
        state.removeChild(idx, &undoManager);
    
    // Section du milieu : créer modulation de liaison
    if (prevSectionId >= 0 && nextSectionId >= 0)
    {
        auto modulationNode = createModulationNode(prevSectionId, nextSectionId);
        int insertIndex = findValueTreeIndex(ModelIdentifiers::SECTION, prevSectionId) + 1;
        state.addChild(modulationNode, insertIndex, &undoManager);
    }
    
    assertInvariants();
}

void Piece::removeLastSection()
{
    if (getSectionCount() > 0)
        removeSection(static_cast<int>(getSectionCount()) - 1);
}

void Piece::clear()
{
    state.removeAllChildren(&undoManager);
    assertInvariants();
}

Section Piece::getSection(size_t index) const { return Section(getChildOfType(ModelIdentifiers::SECTION, index)); }
Modulation Piece::getModulation(size_t index) const { return Modulation(getChildOfType(ModelIdentifiers::MODULATION, index)); }

Section Piece::getSectionById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION) && static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            return Section(child);
    }
    jassertfalse;
    return Section(juce::ValueTree(ModelIdentifiers::SECTION));
}

Modulation Piece::getModulationById(int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::MODULATION) && static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            return Modulation(child);
    }
    jassertfalse;
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
                return sectionIndex;
            sectionIndex++;
        }
    }
    return -1;
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
                return modulationIndex;
            modulationIndex++;
        }
    }
    return -1;
}

std::vector<Section> Piece::getSections() const
{
    std::vector<Section> sections;
    for (const auto& node : getChildrenOfType(ModelIdentifiers::SECTION))
        sections.emplace_back(node);
    return sections;
}

std::vector<Modulation> Piece::getModulations() const
{
    std::vector<Modulation> modulations;
    for (const auto& node : getChildrenOfType(ModelIdentifiers::MODULATION))
        modulations.emplace_back(node);
    return modulations;
}

std::pair<Section, Section> Piece::getAdjacentSections(const Modulation& modulation) const
{
    return { getSectionById(modulation.getFromSectionId()), getSectionById(modulation.getToSectionId()) };
}

size_t Piece::getSectionCount() const { return getChildrenOfType(ModelIdentifiers::SECTION).size(); }
size_t Piece::getModulationCount() const { return getChildrenOfType(ModelIdentifiers::MODULATION).size(); }
int Piece::getNumElements() const { return state.getNumChildren(); }
bool Piece::isEmpty() const { return state.getNumChildren() == 0; }
void Piece::setTitle(const juce::String& newTitle) { state.setProperty(ModelIdentifiers::name, newTitle, &undoManager); }
juce::String Piece::getTitle() const { return state.getProperty(ModelIdentifiers::name, "Untitled Piece").toString(); }

bool Piece::hasValidStructure() const
{
    auto sectionCount = getSectionCount();
    auto modulationCount = getModulationCount();
    if (sectionCount == 0) return modulationCount == 0;
    return modulationCount == (sectionCount - 1);
}

bool Piece::isComplete() const { return !isEmpty() && hasValidStructure(); }

void Piece::assertInvariants() const
{
#if JUCE_DEBUG
    auto sectionCount = getSectionCount();
    auto modulationCount = getModulationCount();
    
    if (sectionCount == 0)
        jassert(modulationCount == 0);
    else
        jassert(modulationCount == sectionCount - 1);
    
    // Vérifier alternance S-M-S-M-S
    if (sectionCount > 1)
    {
        bool expectSection = true;
        for (int i = 0; i < state.getNumChildren(); ++i)
        {
            auto child = state.getChild(i);
            jassert(expectSection ? child.hasType(ModelIdentifiers::SECTION) : child.hasType(ModelIdentifiers::MODULATION));
            expectSection = !expectSection;
        }
    }
#endif
}

int Piece::getTotalChordCount() const
{
    int total = 0;
    for (const auto& section : getSections())
        total += static_cast<int>(section.getProgression().size());
    return total;
}

juce::String Piece::toString() const
{
    if (isEmpty()) return getTitle() + " (Empty)";
    return getTitle() + " (" + juce::String(getSectionCount()) + " sections, " +
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

int Piece::generateNextSectionId() const
{
    int maxId = -1;
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
            maxId = std::max(maxId, static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)));
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
            maxId = std::max(maxId, static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)));
    }
    return maxId + 1;
}

juce::ValueTree Piece::createSectionNode(const juce::String& name)
{
    juce::ValueTree sectionNode(ModelIdentifiers::SECTION);
    sectionNode.setProperty(ModelIdentifiers::id, generateNextSectionId(), nullptr);
    sectionNode.setProperty(ModelIdentifiers::name, name, nullptr);
    sectionNode.setProperty(ModelIdentifiers::tonalityNote, 0, nullptr);
    sectionNode.setProperty(ModelIdentifiers::tonalityAlteration, 0, nullptr);
    sectionNode.setProperty(ModelIdentifiers::isMajor, true, nullptr);
    
    juce::ValueTree progressionNode(ModelIdentifiers::PROGRESSION);
    progressionNode.setProperty(ModelIdentifiers::id, 0, nullptr);
    sectionNode.appendChild(progressionNode, nullptr);
    
    return sectionNode;
}

juce::ValueTree Piece::createModulationNode(int fromSectionId, int toSectionId)
{
    juce::ValueTree modulationNode(ModelIdentifiers::MODULATION);
    modulationNode.setProperty(ModelIdentifiers::id, generateNextModulationId(), nullptr);
    modulationNode.setProperty(ModelIdentifiers::modulationType, static_cast<int>(Diatony::ModulationType::PivotChord), nullptr);
    modulationNode.setProperty(ModelIdentifiers::fromSectionId, fromSectionId, nullptr);
    modulationNode.setProperty(ModelIdentifiers::toSectionId, toSectionId, nullptr);
    modulationNode.setProperty(ModelIdentifiers::fromChordIndex, -1, nullptr);
    modulationNode.setProperty(ModelIdentifiers::toChordIndex, -1, nullptr);
    modulationNode.setProperty(ModelIdentifiers::name, "Modulation " + juce::String(generateNextModulationId() - 1), nullptr);
    return modulationNode;
}

int Piece::findValueTreeIndex(const juce::Identifier& type, int id) const
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto child = state.getChild(i);
        if (child.hasType(type) && static_cast<int>(child.getProperty(ModelIdentifiers::id, -1)) == id)
            return i;
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
            children.push_back(child);
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
