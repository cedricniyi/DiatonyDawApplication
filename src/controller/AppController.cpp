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
    
    // === LOGS DE VALIDATION DU NOUVEAU SYSTÈME D'IDs ===
    int newSectionIndex = getSectionCount() - 1;
    auto newSection = piece.getSection(newSectionIndex);
    int newSectionId = newSection.getState().getProperty(ModelIdentifiers::id, -1);
    
    DBG("========================================");
    DBG("✅ Section créée : " << sectionName);
    DBG("   - Index : " << newSectionIndex);
    DBG("   - ID : " << newSectionId);
    
    // Si une modulation a été créée, afficher ses infos
    if (piece.getModulationCount() > 0)
    {
        auto lastModulation = piece.getModulation(piece.getModulationCount() - 1);
        int modulationId = static_cast<int>(lastModulation.getState().getProperty(ModelIdentifiers::id, -1));
        
        DBG("✅ Modulation créée automatiquement :");
        DBG("   - ID Modulation : " << modulationId);
        DBG("   - From Section ID : " << lastModulation.getFromSectionId());
        DBG("   - To Section ID : " << lastModulation.getToSectionId());
        DBG("   - From Chord Index : " << lastModulation.getFromChordIndex());
        DBG("   - To Chord Index : " << lastModulation.getToChordIndex());
        
        // Test du helper getAdjacentSections()
        auto [fromSection, toSection] = piece.getAdjacentSections(lastModulation);
        int fromSectionId = static_cast<int>(fromSection.getState().getProperty(ModelIdentifiers::id, -1));
        int toSectionId = static_cast<int>(toSection.getState().getProperty(ModelIdentifiers::id, -1));
        
        DBG("🔗 Test getAdjacentSections() :");
        DBG("   - Section source : " << fromSection.getName() << " (ID=" << fromSectionId << ")");
        DBG("   - Section destination : " << toSection.getName() << " (ID=" << toSectionId << ")");
    }
    DBG("========================================");
    
    // Sélectionner automatiquement la nouvelle section
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

// Génération (asynchrone)
void AppController::startGeneration()
{
    DBG("AppController::startGeneration() - Début de la génération ASYNCHRONE");
    
    // Vérifier que la pièce n'est pas vide
    if (piece.isEmpty())
    {
        DBG("  ❌ Erreur : La pièce est vide, impossible de générer");
        // ⚠️ Notifier l'UI via le selectionState (architecture réactive)
        selectionState.setProperty("generationStatus", "error", nullptr);
        selectionState.setProperty("generationError", juce::String::fromUTF8("La pièce est vide. Veuillez ajouter au moins une section."), nullptr);
        return;
    }
    
    DBG("  ✓ Pièce valide, lancement du thread de génération...");
    
    // Mettre à jour le status IMMÉDIATEMENT (l'UI va réagir et afficher le spinner)
    selectionState.setProperty("generationStatus", "generating", nullptr);
    
    // Lancer la génération sur un thread séparé (NON-BLOQUANT)
    juce::String dummyPath = "";  // Non utilisé, le service génère son propre chemin
    bool launched = generationService.startGeneration(piece, dummyPath, this);
    
    if (!launched)
    {
        // Le thread n'a pas pu être lancé (déjà en cours ou service non prêt)
        DBG("  ❌ Impossible de lancer la génération : " << generationService.getLastError());
        selectionState.setProperty("generationStatus", "error", nullptr);
        selectionState.setProperty("generationError", generationService.getLastError(), nullptr);
        return;
    }
    
    DBG("  ✅ Thread de génération lancé ! (retour immédiat)");
    // NOTE : handleAsyncUpdate() sera appelé automatiquement quand le thread terminera
}

// Callback AsyncUpdater : appelé sur le message thread quand la génération est terminée
void AppController::handleAsyncUpdate()
{
    DBG("=================================================================");
    DBG("📬 AppController::handleAsyncUpdate() - Notification reçue du thread");
    DBG("=================================================================");
    
    // Lire le résultat de la génération (thread-safe)
    bool success = generationService.getLastGenerationSuccess();
    
    if (success)
    {
        DBG("  ✅ Génération réussie ! Mise à jour de l'état...");
        
        // ✅ Mettre à jour l'état : succès
        selectionState.setProperty("generationStatus", "completed", nullptr);
        selectionState.setProperty("midiFilePath", juce::String::fromUTF8("Fichier MIDI généré avec succès"), nullptr);
    }
    else
    {
        DBG("  ❌ Génération échouée : " << generationService.getLastError());
        
        // ❌ Mettre à jour l'état : erreur
        // La Vue (MainContentComponent) réagira à ce changement et affichera le pop-up d'erreur
        selectionState.setProperty("generationStatus", "error", nullptr);
        selectionState.setProperty("generationError", generationService.getLastError(), nullptr);
    }
    
    DBG("=================================================================");
    DBG("📬 handleAsyncUpdate() terminé - L'UI va réagir au changement");
    DBG("=================================================================");
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