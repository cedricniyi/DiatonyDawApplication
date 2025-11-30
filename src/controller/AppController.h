#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "../model/Piece.h"
#include "../model/ModelIdentifiers.h"
#include "ContextIdentifiers.h"
#include "../services/GenerationService.h"

/**
 * Contrôleur principal de l'application - Le "cerveau" découplé de l'UI
 * 
 * Responsabilités :
 * - Posséder l'instance unique du modèle Piece
 * - Gérer l'état de l'application (sélection, mode d'édition, etc.)
 * - Exposer une API claire pour les actions utilisateur
 * - Être totalement découplé de l'interface graphique
 * - Recevoir les callbacks asynchrones du GenerationService via AsyncUpdater
 * 
 * Architecture MVC : C'est le Controller qui fait le lien entre Model (Piece) et View (UI)
 * 
 * Hérite de juce::AsyncUpdater pour recevoir les notifications du thread de génération
 * de manière thread-safe (handleAsyncUpdate sera appelé sur le message thread)
 */
class AppController : public juce::AsyncUpdater
{
public:
    // === CONSTRUCTEURS ===
    AppController();
    explicit AppController(const juce::String& pieceTitle);
    
    // === ÉTAT DE L'APPLICATION ===
    enum class EditMode
    {
        Overview,       // Vue d'ensemble
        SectionEdit,    // Édition d'une section
        ChordEdit       // Édition d'un accord
    };
    
    // === API POUR ACTIONS UTILISATEUR ===
    
    // Actions sur les sections
    void addNewSection(const juce::String& sectionName = "New Section");
    void removeSection(int sectionIndex);
    void selectSection(int sectionIndex);
    
    // Action sur les modulations (on peut les sélectionner pour éditer, mais pas les créer/détruire directement)
    void selectModulation(int modulationIndex);
    
    // Actions sur les accords
    void addChordToSection(int sectionIndex, Diatony::ChordDegree degree, 
                          Diatony::ChordQuality quality, Diatony::ChordState state);
    void removeChordFromSection(int sectionIndex, int chordIndex);
    void selectChord(int sectionIndex, int chordIndex);
    
    // Actions générales
    void clearSelection();
    void clearPiece();
    void setPieceTitle(const juce::String& title);
    
    // Génération
    void startGeneration();
    
    // Undo/Redo
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    
    // === GETTERS POUR L'ÉTAT ===
    
    // Accès au modèle (ValueTree) pour l'UI
    juce::ValueTree& getState() { return piece.getState(); }
    
    // Accès au modèle (objet Piece) pour la logique interne (lecture seule pour l'UI)
    const Piece& getPiece() const { return piece; }
    
    // État de l'application
    EditMode getCurrentEditMode() const { return currentEditMode; }
    juce::ValueTree& getSelectionState() { return selectionState; }
    
    // Informations utiles pour l'UI
    bool isEmpty() const { return piece.isEmpty(); }
    bool hasValidStructure() const { return piece.hasValidStructure(); }
    juce::String getPieceTitle() const { return piece.getTitle(); }
    int getSectionCount() const { return static_cast<int>(piece.getSectionCount()); }
    int getModulationCount() const { return static_cast<int>(piece.getModulationCount()); }
    int getTotalChordCount() const { return piece.getTotalChordCount(); }
    
    // === ACCÈS À L'ÉTAT DE SÉLECTION ===
    // L'UI peut s'abonner directement au ValueTree selectionState via getSelectionState()
    
private:
    // === DONNÉES MEMBRES ===
    Piece piece;                    // Le modèle de données (propriétaire unique)
    EditMode currentEditMode;       // Mode d'édition actuel
    juce::ValueTree selectionState; // État de sélection centralisé
    GenerationService generationService; // Service de génération
    
    // === MÉTHODES PRIVÉES ===
    
    void setEditMode(EditMode newMode);
    void updateSelectionFromIndices(int sectionIndex, int chordIndex = -1);
    
    // Validation
    bool isValidSectionIndex(int index) const;
    bool isValidModulationIndex(int index) const;
    bool isValidChordIndex(int sectionIndex, int chordIndex) const;
    
    // === ASYNCUPDATER CALLBACK ===
    
    /**
     * @brief Callback appelé sur le message thread quand la génération est terminée
     * 
     * Cette méthode est appelée automatiquement par JUCE sur le message thread
     * quand triggerAsyncUpdate() est invoqué depuis le thread worker.
     * C'est ici qu'on met à jour le ValueTree avec les résultats.
     */
    void handleAsyncUpdate() override;
};