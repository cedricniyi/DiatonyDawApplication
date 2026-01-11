#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Section.h"
#include "Modulation.h"
#include "ModelIdentifiers.h"

/**
 * @brief Pièce musicale complète : sections tonales reliées par des modulations.
 * 
 * Propriétaire du ValueTree racine. Gère les IDs et garantit l'invariant :
 * modulations.count == sections.count - 1, avec alternance S-M-S-M-S.
 */
class Piece {
public:
    Piece();
    explicit Piece(const juce::String& pieceTitle);
    
    /** @brief Ajoute une section (+ modulation automatique si pas la première) */
    void addSection(const juce::String& sectionName = "Section");
    
    /** @brief Supprime une section et gère les modulations adjacentes */
    void removeSection(int sectionIndex);
    void removeLastSection();
    void clear();
    
    Section getSection(size_t index) const;
    Modulation getModulation(size_t index) const;
    Section getSectionById(int id) const;
    Modulation getModulationById(int id) const;
    int getSectionIndexById(int id) const;
    int getModulationIndexById(int id) const;
    
    std::vector<Section> getSections() const;
    std::vector<Modulation> getModulations() const;
    std::pair<Section, Section> getAdjacentSections(const Modulation& modulation) const;
    
    size_t getSectionCount() const;
    size_t getModulationCount() const;
    int getNumElements() const;
    bool isEmpty() const;
    
    void setTitle(const juce::String& newTitle);
    juce::String getTitle() const;
    
    bool hasValidStructure() const;
    bool isComplete() const;
    
    int getTotalChordCount() const;
    juce::String toString() const;
    juce::String getDetailedSummary() const;
    
    juce::ValueTree getState() const { return state; }
    juce::ValueTree& getState() { return state; }
    juce::UndoManager& getUndoManager() { return undoManager; }
    const juce::UndoManager& getUndoManager() const { return undoManager; }
    
private:
    juce::ValueTree state;
    juce::UndoManager undoManager;
    
    int generateNextSectionId() const;
    int generateNextModulationId() const;
    
    juce::ValueTree createSectionNode(const juce::String& name);
    juce::ValueTree createModulationNode(int fromSectionId, int toSectionId);
    int findValueTreeIndex(const juce::Identifier& type, int id) const;
    std::vector<juce::ValueTree> getChildrenOfType(const juce::Identifier& type) const;
    juce::ValueTree getChildOfType(const juce::Identifier& type, size_t index) const;
    void assertInvariants() const;
};
