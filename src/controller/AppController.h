#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "../model/Piece.h"
#include "../model/ModelIdentifiers.h"

/**
 * Contrôleur principal de l'application - Le "cerveau" découplé de l'UI
 * 
 * Responsabilités :
 * - Posséder l'instance unique du modèle Piece
 * - Gérer l'état de l'application (sélection, mode d'édition, etc.)
 * - Exposer une API claire pour les actions utilisateur
 * - Être totalement découplé de l'interface graphique
 * 
 * Architecture MVC : C'est le Controller qui fait le lien entre Model (Piece) et View (UI)
 */
class AppController
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
    
    enum class SelectionType
    {
        None,
        Section,
        Modulation,
        Chord
    };
    
    struct Selection
    {
        SelectionType type = SelectionType::None;
        juce::String elementId;         // ID de l'élément sélectionné
        int sectionIndex = -1;          // Index de section (si applicable)
        int chordIndex = -1;            // Index d'accord (si applicable)
        
        bool isValid() const { return type != SelectionType::None && !elementId.isEmpty(); }
        void clear() { type = SelectionType::None; elementId = ""; sectionIndex = -1; chordIndex = -1; }
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
    const Selection& getCurrentSelection() const { return currentSelection; }
    
    // Informations utiles pour l'UI
    bool isEmpty() const { return piece.isEmpty(); }
    bool hasValidStructure() const { return piece.hasValidStructure(); }
    juce::String getPieceTitle() const { return piece.getTitle(); }
    int getSectionCount() const { return static_cast<int>(piece.getSectionCount()); }
    int getModulationCount() const { return static_cast<int>(piece.getModulationCount()); }
    int getTotalChordCount() const { return piece.getTotalChordCount(); }
    
    // === CALLBACKS POUR L'UI ===
    std::function<void()> onModelChanged;           // Appelé quand le modèle change
    std::function<void()> onSelectionChanged;       // Appelé quand la sélection change
    std::function<void()> onEditModeChanged;        // Appelé quand le mode d'édition change
    
private:
    // === DONNÉES MEMBRES ===
    Piece piece;                    // Le modèle de données (propriétaire unique)
    EditMode currentEditMode;       // Mode d'édition actuel
    Selection currentSelection;     // Sélection actuelle
    
    // === MÉTHODES PRIVÉES ===
    void notifyModelChanged();
    void notifySelectionChanged();
    void notifyEditModeChanged();
    
    void setEditMode(EditMode newMode);
    void updateSelectionFromIndices(int sectionIndex, int chordIndex = -1);
    juce::String generateElementId(const juce::Identifier& type, int index) const;
    
    // Validation
    bool isValidSectionIndex(int index) const;
    bool isValidModulationIndex(int index) const;
    bool isValidChordIndex(int sectionIndex, int chordIndex) const;
};