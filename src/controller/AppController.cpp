#include "AppController.h"

// === CONSTRUCTEURS ===

AppController::AppController() 
    : piece(), currentEditMode(EditMode::Overview)
{
}

AppController::AppController(const juce::String& pieceTitle) 
    : piece(pieceTitle), currentEditMode(EditMode::Overview)
{
}

// === API POUR ACTIONS UTILISATEUR ===

// Actions sur les sections
void AppController::addNewSection(const juce::String& sectionName)
{
    piece.addSection(sectionName);
    
    // Sélectionner automatiquement la nouvelle section
    int newSectionIndex = getSectionCount() - 1;
    selectSection(newSectionIndex);
    
    notifyModelChanged();
}

void AppController::removeSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
        
    // Si on supprime la section sélectionnée, clear la sélection
    if (currentSelection.type == SelectionType::Section && 
        currentSelection.sectionIndex == sectionIndex)
    {
        clearSelection();
    }
    
    // Note: Pour l'instant, on ne peut que supprimer la dernière section
    // Pour une implémentation complète, il faudrait ajouter removeSection(index) à Piece
    if (sectionIndex == getSectionCount() - 1)
    {
        piece.removeLastSection();
        notifyModelChanged();
    }
}

void AppController::selectSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
        
    currentSelection.clear();
    currentSelection.type = SelectionType::Section;
    currentSelection.sectionIndex = sectionIndex;
    currentSelection.elementId = generateElementId(ModelIdentifiers::SECTION, sectionIndex);
    
    setEditMode(EditMode::SectionEdit);
    notifySelectionChanged();
}

// Actions sur les modulations
void AppController::selectModulation(int modulationIndex)
{
    if (!isValidModulationIndex(modulationIndex))
        return;
        
    currentSelection.clear();
    currentSelection.type = SelectionType::Modulation;
    currentSelection.sectionIndex = modulationIndex;  // Réutilise sectionIndex pour l'index de modulation
    currentSelection.elementId = generateElementId(ModelIdentifiers::MODULATION, modulationIndex);
    
    setEditMode(EditMode::Overview);  // Les modulations restent en mode overview
    notifySelectionChanged();
}

// Actions sur les accords
void AppController::addChordToSection(int sectionIndex, Diatony::ChordDegree degree, 
                                     Diatony::ChordQuality quality, Diatony::ChordState state)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
        
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    progression.addChord(degree, quality, state);
    
    // Sélectionner automatiquement le nouvel accord
    int newChordIndex = static_cast<int>(progression.size()) - 1;
    selectChord(sectionIndex, newChordIndex);
    
    notifyModelChanged();
}

void AppController::removeChordFromSection(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
        
    // Si on supprime l'accord sélectionné, clear la sélection
    if (currentSelection.type == SelectionType::Chord && 
        currentSelection.sectionIndex == sectionIndex &&
        currentSelection.chordIndex == chordIndex)
    {
        clearSelection();
    }
    
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    progression.removeChord(static_cast<size_t>(chordIndex));
    
    notifyModelChanged();
}

void AppController::selectChord(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
        
    currentSelection.clear();
    currentSelection.type = SelectionType::Chord;
    currentSelection.sectionIndex = sectionIndex;
    currentSelection.chordIndex = chordIndex;
    currentSelection.elementId = generateElementId(ModelIdentifiers::CHORD, chordIndex);
    
    setEditMode(EditMode::ChordEdit);
    notifySelectionChanged();
}

// Actions générales
void AppController::clearSelection()
{
    if (currentSelection.type == SelectionType::None)
        return;  // Déjà vide
        
    currentSelection.clear();
    setEditMode(EditMode::Overview);
    notifySelectionChanged();
}

void AppController::clearPiece()
{
    piece.clear();
    clearSelection();
    notifyModelChanged();
}

void AppController::setPieceTitle(const juce::String& title)
{
    piece.setTitle(title);
    notifyModelChanged();
}

// Undo/Redo
void AppController::undo()
{
    if (canUndo())
    {
        piece.getUndoManager().undo();
        // La sélection pourrait ne plus être valide après undo
        clearSelection();
        notifyModelChanged();
    }
}

void AppController::redo()
{
    if (canRedo())
    {
        piece.getUndoManager().redo();
        // La sélection pourrait ne plus être valide après redo
        clearSelection();
        notifyModelChanged();
    }
}

bool AppController::canUndo() const
{
    return piece.getUndoManager().canUndo();
}

bool AppController::canRedo() const
{
    return piece.getUndoManager().canRedo();
}

// === MÉTHODES PRIVÉES ===

void AppController::notifyModelChanged()
{
    if (onModelChanged)
        onModelChanged();
}

void AppController::notifySelectionChanged()
{
    if (onSelectionChanged)
        onSelectionChanged();
}

void AppController::notifyEditModeChanged()
{
    if (onEditModeChanged)
        onEditModeChanged();
}

void AppController::setEditMode(EditMode newMode)
{
    if (currentEditMode != newMode)
    {
        currentEditMode = newMode;
        notifyEditModeChanged();
    }
}

void AppController::updateSelectionFromIndices(int sectionIndex, int chordIndex)
{
    if (chordIndex >= 0)
    {
        // Sélection d'accord
        selectChord(sectionIndex, chordIndex);
    }
    else if (sectionIndex >= 0)
    {
        // Sélection de section
        selectSection(sectionIndex);
    }
    else
    {
        // Pas de sélection
        clearSelection();
    }
}

juce::String AppController::generateElementId(const juce::Identifier& type, int index) const
{
    return type.toString() + "_" + juce::String(index);
}

// Validation
bool AppController::isValidSectionIndex(int index) const
{
    return index >= 0 && index < getSectionCount();
}

bool AppController::isValidModulationIndex(int index) const
{
    return index >= 0 && index < getModulationCount();
}

bool AppController::isValidChordIndex(int sectionIndex, int chordIndex) const
{
    if (!isValidSectionIndex(sectionIndex))
        return false;
        
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    return chordIndex >= 0 && chordIndex < static_cast<int>(progression.size());
} 