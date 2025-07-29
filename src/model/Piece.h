#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Section.h"
#include "Modulation.h"
#include "ModelIdentifiers.h"

/**
 * Représente une pièce musicale complète avec ses sections tonales et modulations
 * Propriétaire du ValueTree racine et de l'UndoManager
 * Maintient l'invariant : modulations.size() == sections.size() - 1
 * Architecture ValueTree : les éléments sont stockés dans l'ordre chronologique
 */
class Piece {
public:
    Piece();
    explicit Piece(const juce::String& pieceTitle);
    
    // Gestion des sections
    void addSection(const Section& section);           // Ajoute automatiquement une modulation si nécessaire
    void addSection(const juce::String& sectionName);  // Ajoute automatiquement une modulation si nécessaire
    Section createSection(const juce::String& sectionName);
    void removeLastSection();  // Supprime section et modulation associée
    void clearAllElements();   // Vide toute la pièce
    void clear();             // Alias pour compatibilité
    
    // Gestion des modulations
    void addModulation(const Modulation& modulation);
    void addModulation(Diatony::ModulationType type, int fromChord, int toChord);
    Modulation createModulation(Diatony::ModulationType type, int fromChord, int toChord);
    
    // Accès aux éléments par type
    std::vector<Section> getSections() const;
    std::vector<Modulation> getModulations() const;
    
    Section getSection(size_t index) const;
    Section getSection(size_t index);
    
    Modulation getModulation(size_t index) const;
    Modulation getModulation(size_t index);
    
    // Informations sur la pièce
    size_t getSectionCount() const;
    size_t getModulationCount() const;
    int getNumElements() const; // Nombre total d'éléments (sections + modulations)
    bool isEmpty() const;
    
    void setTitle(const juce::String& newTitle);
    const juce::String getTitle() const;
    
    // Validation
    bool isComplete() const;
    bool hasValidStructure() const;  // Vérifie l'invariant des modulations
    
    // Statistiques
    int getTotalChordCount() const;
    
    // Méthodes utilitaires
    juce::String toString() const;
    juce::String getDetailedSummary() const;
    
    // Accès au ValueTree et UndoManager
    juce::ValueTree getState() const { return state; }
    juce::ValueTree& getState() { return state; }
    juce::UndoManager& getUndoManager() { return undoManager; }
    const juce::UndoManager& getUndoManager() const { return undoManager; }
    
    // Création d'un nouveau nœud Piece
    static juce::ValueTree createPieceNode(const juce::String& title);
    
private:
    juce::ValueTree state;           // ValueTree racine contenant toute la pièce
    juce::UndoManager undoManager;   // Gestionnaire Undo/Redo
    
    // Helpers pour naviguer dans l'arborescence
    std::vector<juce::ValueTree> getChildrenOfType(const juce::Identifier& type) const;
    juce::ValueTree getChildOfType(const juce::Identifier& type, size_t index) const;
    
    // Validation des index
    void validateSectionIndex(size_t index) const;
    void validateModulationIndex(size_t index) const;
}; 