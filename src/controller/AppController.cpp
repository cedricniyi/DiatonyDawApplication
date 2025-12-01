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
    int newSectionIndex = getSectionCount() - 1;
    selectSection(newSectionIndex);
}

void AppController::removeSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
    
    // Récupérer l'ID de la section à supprimer
    auto section = piece.getSection(sectionIndex);
    int sectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
        
    // === SOLUTION "GOOD ENOUGH" ===
    // Si on supprime une section, les modulations adjacentes sont aussi supprimées.
    // Plutôt que de vérifier précisément quelle modulation meurt, on nettoie
    // la sélection si c'est la section OU une modulation qui est sélectionnée.
    // L'utilisateur revient à la WelcomeView, ce qui est acceptable.
    
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    juce::String currentElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String sectionElementId = "Section_" + juce::String(sectionId);
    
    // Clear si c'est la section sélectionnée qui est supprimée
    if (currentSelectionType == "Section" && currentElementId == sectionElementId)
    {
        clearSelection();
    }
    // Clear aussi si une modulation est sélectionnée (elle pourrait être supprimée)
    else if (currentSelectionType == "Modulation")
    {
        // Les modulations adjacentes à la section supprimée vont mourir.
        // Pour simplifier : on clear toute sélection de modulation lors d'une suppression.
        // TODO: Implémenter une logique plus fine (voir TODO.md - Sélection intelligente)
        clearSelection();
    }
    
    // Suppression de la section avec gestion automatique des modulations
    piece.removeSection(sectionIndex);
}

void AppController::selectSection(int sectionIndex)
{
    if (!isValidSectionIndex(sectionIndex))
        return;
    
    // Récupérer l'ID de la section (stable, ne change pas après suppression d'autres sections)
    auto section = piece.getSection(sectionIndex);
    int sectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
        
    // Mettre à jour l'état de sélection via ValueTree avec l'ID (pas l'index)
    selectionState.setProperty(ContextIdentifiers::selectionType, "Section", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              "Section_" + juce::String(sectionId), 
                              &piece.getUndoManager());
    
    setEditMode(EditMode::SectionEdit);
}

// Actions sur les modulations
void AppController::selectModulation(int modulationIndex)
{
    if (!isValidModulationIndex(modulationIndex))
        return;
    
    // Récupérer l'ID de la modulation (stable)
    auto modulation = piece.getModulation(modulationIndex);
    int modulationId = modulation.getState().getProperty(ModelIdentifiers::id, -1);
        
    // Mettre à jour l'état de sélection via ValueTree avec l'ID (pas l'index)
    selectionState.setProperty(ContextIdentifiers::selectionType, "Modulation", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              "Modulation_" + juce::String(modulationId), 
                              &piece.getUndoManager());
    
    setEditMode(EditMode::Overview);
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
    
    // Récupérer l'ID réel de l'accord
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    auto chord = progression.getChord(static_cast<size_t>(chordIndex));
    int chordId = chord.getId();
    
    // Vérifier si cet accord est sélectionné (par ID, pas par index)
    juce::String currentSelectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    juce::String currentElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String chordElementId = "Chord_" + juce::String(chordId);
    
    if (currentSelectionType == "Chord" && currentElementId == chordElementId)
    {
        clearSelection();
    }
    
    progression.removeChord(static_cast<size_t>(chordIndex));
}

void AppController::selectChord(int sectionIndex, int chordIndex)
{
    if (!isValidChordIndex(sectionIndex, chordIndex))
        return;
    
    // Récupérer l'ID réel de l'accord via les wrappers
    auto section = piece.getSection(sectionIndex);
    auto progression = section.getProgression();
    auto chord = progression.getChord(static_cast<size_t>(chordIndex));
    int chordId = chord.getId();
    
    selectionState.setProperty(ContextIdentifiers::selectionType, "Chord", &piece.getUndoManager());
    selectionState.setProperty(ContextIdentifiers::selectedElementId, 
                              "Chord_" + juce::String(chordId), 
                              &piece.getUndoManager());
    
    setEditMode(EditMode::ChordEdit);
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
        
        // 1. Sauvegarder le fichier .diatony associé (état du modèle en XML)
        juce::String midiPath = generationService.getLastGeneratedMidiPath();
        if (midiPath.isNotEmpty())
        {
            juce::File midiFile(midiPath);
            juce::File diatonyFile = midiFile.withFileExtension("diatony");
            
            // Sérialiser le ValueTree en XML
            juce::String xmlContent = piece.getState().toXmlString();
            bool saved = diatonyFile.replaceWithText(xmlContent);
            
            if (saved)
            {
                DBG("  📄 Fichier .diatony sauvegardé : " << diatonyFile.getFullPathName());
            }
            else
            {
                DBG("  ⚠️ Impossible de sauvegarder le fichier .diatony");
            }
        }
        
        // 2. Mettre à jour l'état UI : succès
        selectionState.setProperty("generationStatus", "completed", nullptr);
        selectionState.setProperty("midiFilePath", midiPath, nullptr);
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