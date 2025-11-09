#include "AppController.h"

// === CONSTRUCTEURS ===

AppController::AppController() 
    : piece(), currentEditMode(EditMode::Overview), selectionState(ContextIdentifiers::SELECTION_STATE)
{
    // Initialiser l'état de sélection avec les valeurs par défaut (pas de sélection)
    selectionState.setProperty(ContextIdentifiers::selectionType, "None", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, "", &piece.getUndoManager());
}

AppController::AppController(const juce::String& pieceTitle) 
    : piece(pieceTitle), currentEditMode(EditMode::Overview), selectionState(ContextIdentifiers::SELECTION_STATE)
{
    // Initialiser l'état de sélection avec les valeurs par défaut (pas de sélection)
    selectionState.setProperty(ContextIdentifiers::selectionType, "None", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, "", &piece.getUndoManager());
}

// === API POUR ACTIONS UTILISATEUR ===

// Actions sur les sections
void AppController::addNewSection(const juce::String& sectionName)
{
    piece.addSection(sectionName);
    
    // Sélectionner automatiquement la nouvelle section
    int newSectionIndex = getSectionCount() - 1;
    selectSection(newSectionIndex);
    
}

void AppController::removeSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
        
    // Si on supprime la section sélectionnée, clear la sélection
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    juce::String currentElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String sectionElementId = generateElementId(ModelIdentifiers::SECTION, sectionIndex);
    
    if (currentSelectionType == "Section" && currentElementId == sectionElementId)
    {
        clearSelection();
    }
    
    // Note: Pour l'instant, on ne peut que supprimer la dernière section
    // Pour une implémentation complète, il faudrait ajouter removeSection(index) à Piece
    if (sectionIndex == getSectionCount() - 1)
    {
        piece.removeLastSection();
    }
}

void AppController::selectSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
        
    // Mettre à jour l'état de sélection via ValueTree
    selectionState.setProperty(ContextIdentifiers::selectionType, "Section", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              generateElementId(ModelIdentifiers::SECTION, sectionIndex), 
                              &piece.getUndoManager());
    
    setEditMode(EditMode::SectionEdit);
    // Plus besoin de notifySelectionChanged() - l'UI s'abonne directement au ValueTree
}

// Actions sur les modulations
void AppController::selectModulation(int modulationIndex)
{
    if (!isValidModulationIndex(modulationIndex))
        return;
        
    // Mettre à jour l'état de sélection via ValueTree
    selectionState.setProperty(ContextIdentifiers::selectionType, "Modulation", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              generateElementId(ModelIdentifiers::MODULATION, modulationIndex), 
                              &piece.getUndoManager());
    
    setEditMode(EditMode::Overview);  // Les modulations restent en mode overview
    // Plus besoin de notifySelectionChanged() - l'UI s'abonne directement au ValueTree
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
    
}

void AppController::removeChordFromSection(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
        
    // Si on supprime l'accord sélectionné, clear la sélection
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    juce::String currentElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String chordElementId = generateElementId(ModelIdentifiers::CHORD, chordIndex);
    
    if (currentSelectionType == "Chord" && currentElementId == chordElementId)
    {
        clearSelection();
    }
    
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    progression.removeChord(static_cast<size_t>(chordIndex));
    
}

void AppController::selectChord(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
        
    // Mettre à jour l'état de sélection via ValueTree
    selectionState.setProperty(ContextIdentifiers::selectionType, "Chord", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              generateElementId(ModelIdentifiers::CHORD, chordIndex), 
                              &piece.getUndoManager());
    
    setEditMode(EditMode::ChordEdit);
    // Plus besoin de notifySelectionChanged() - l'UI s'abonne directement au ValueTree
}

// Actions générales
void AppController::clearSelection()
{
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    if (currentSelectionType == "None")
        return;  // Déjà vide
        
    selectionState.setProperty(ContextIdentifiers::selectionType, "None", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, "", &piece.getUndoManager());
    setEditMode(EditMode::Overview);
    // Plus besoin de notifySelectionChanged() - l'UI s'abonne directement au ValueTree
}

void AppController::clearPiece()
{
    piece.clear();
    clearSelection();
}

void AppController::setPieceTitle(const juce::String& title)
{
    piece.setTitle(title);
}

// Génération
void AppController::startGeneration()
{
    DBG("AppController::startGeneration() - Début de la génération");
    
    // Vérifier que la pièce n'est pas vide
    if (piece.isEmpty())
    {
        DBG("  ❌ Erreur : La pièce est vide, impossible de générer");
        return;
    }
    
    // TODO: Dans le futur, on pourra mettre un statut "generating" dans le ValueTree
    // pour que l'UI affiche un loader
    // Ex: getState().setProperty("generationStatus", "generating", nullptr);
    
    DBG("  ✓ Pièce valide, appel du service de génération...");
    
    // Appeler le service de génération (le chemin est généré automatiquement)
    juce::String dummyPath = "";  // Non utilisé, le service génère son propre chemin
    bool success = generationService.generateMidiFromPiece(piece, dummyPath);
    
    if (success)
    {
        DBG("  ✅ Génération réussie !");
    }
    else
    {
        DBG("  ❌ Erreur lors de la génération : " << generationService.getLastError());
    }
    
    DBG("AppController::startGeneration() - Fin");
}

// Undo/Redo
void AppController::undo()
{
    if (canUndo())
    {
        piece.getUndoManager().undo();
        // La sélection pourrait ne plus être valide après undo
        clearSelection();
    }
}

void AppController::redo()
{
    if (canRedo())
    {
        piece.getUndoManager().redo();
        // La sélection pourrait ne plus être valide après redo
        clearSelection();
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

// Méthodes de notification supprimées - l'UI s'abonne directement aux ValueTree

void AppController::setEditMode(EditMode newMode)
{
    if (currentEditMode != newMode)
    {
        currentEditMode = newMode;
        // Plus besoin de notifyEditModeChanged() - l'UI peut surveiller le mode via d'autres moyens
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