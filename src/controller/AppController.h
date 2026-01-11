#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "../model/Piece.h"
#include "../model/ModelIdentifiers.h"
#include "ContextIdentifiers.h"
#include "../services/GenerationService.h"

/**
 * @brief Contrôleur principal MVC, découplé de l'UI.
 *
 * Possède le modèle Piece, gère la sélection et le mode d'édition.
 * Hérite d'AsyncUpdater pour recevoir les callbacks de génération sur le message thread.
 */
class AppController : public juce::AsyncUpdater
{
public:
    AppController();
    explicit AppController(const juce::String& pieceTitle);
    
    enum class EditMode { Overview, SectionEdit, ChordEdit };
    
    // Actions sections
    void addNewSection(const juce::String& sectionName = "New Section");
    void removeSection(int sectionIndex);
    void selectSection(int sectionIndex);
    
    // Actions modulations
    void selectModulation(int modulationIndex);
    
    // Actions accords
    void addChordToSection(int sectionIndex, Diatony::ChordDegree degree, 
                          Diatony::ChordQuality quality, Diatony::ChordState state);
    void removeChordFromSection(int sectionIndex, int chordIndex);
    void selectChord(int sectionIndex, int chordIndex);
    
    // Actions générales
    void clearSelection();
    void clearPiece();
    void setPieceTitle(const juce::String& title);
    void startGeneration();
    
    /** @brief Charge un projet depuis un fichier .diatony (XML). */
    bool loadProjectFromFile(const juce::File& file);
    
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    
    // Getters
    juce::ValueTree& getState() { return piece.getState(); }
    const Piece& getPiece() const { return piece; }
    EditMode getCurrentEditMode() const { return currentEditMode; }
    juce::ValueTree& getSelectionState() { return selectionState; }
    
    bool isEmpty() const { return piece.isEmpty(); }
    bool hasValidStructure() const { return piece.hasValidStructure(); }
    juce::String getPieceTitle() const { return piece.getTitle(); }
    int getSectionCount() const { return static_cast<int>(piece.getSectionCount()); }
    int getModulationCount() const { return static_cast<int>(piece.getModulationCount()); }
    int getTotalChordCount() const { return piece.getTotalChordCount(); }
    
private:
    Piece piece;
    EditMode currentEditMode;
    juce::ValueTree selectionState;
    GenerationService generationService;
    
    void setEditMode(EditMode newMode);
    void updateSelectionFromIndices(int sectionIndex, int chordIndex = -1);
    
    bool isValidSectionIndex(int index) const;
    bool isValidModulationIndex(int index) const;
    bool isValidChordIndex(int sectionIndex, int chordIndex) const;
    
    /** @brief Callback message thread après génération (via triggerAsyncUpdate). */
    void handleAsyncUpdate() override;
};