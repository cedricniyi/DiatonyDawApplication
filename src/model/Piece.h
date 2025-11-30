#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Section.h"
#include "Modulation.h"
#include "ModelIdentifiers.h"

/**
 * Représente une pièce musicale complète avec ses sections tonales et modulations.
 * 
 * RESPONSABILITÉS:
 * - Propriétaire du ValueTree racine et de l'UndoManager
 * - Seul gestionnaire des IDs (aucune classe enfant ne génère d'ID)
 * - Garant de la structure alternée Section-Modulation-Section
 * 
 * INVARIANTS:
 * 1. modulations.count() == sections.count() - 1 (ou 0 si vide)
 * 2. L'ordre dans le ValueTree est TOUJOURS: [Section, Modulation, Section, Modulation, ...]
 * 3. Chaque ID est unique au sein de son type (Section IDs, Modulation IDs)
 * 4. Une Modulation relie toujours deux Sections adjacentes existantes
 */
class Piece {
public:
    Piece();
    explicit Piece(const juce::String& pieceTitle);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // GESTION DES SECTIONS
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Ajoute une nouvelle section à la fin de la pièce.
     * Si ce n'est pas la première section, une modulation est automatiquement créée.
     * @param sectionName Nom de la section (par défaut "Section")
     */
    void addSection(const juce::String& sectionName = "Section");
    
    /**
     * Supprime une section par son index (0-based).
     * Gère automatiquement les modulations adjacentes:
     * - Section unique: supprime juste la section
     * - Première section: supprime section + modulation suivante
     * - Dernière section: supprime section + modulation précédente
     * - Section du milieu: supprime section + 2 modulations, crée 1 nouvelle modulation
     * @param sectionIndex Index de la section à supprimer
     */
    void removeSection(int sectionIndex);
    
    /**
     * Supprime la dernière section (raccourci pour removeSection(getSectionCount()-1))
     */
    void removeLastSection();
    
    /**
     * Vide toute la pièce (toutes les sections et modulations)
     */
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AUX ÉLÉMENTS
    // ═══════════════════════════════════════════════════════════════════════════
    
    // Par index (0-based, dans l'ordre chronologique)
    Section getSection(size_t index) const;
    Modulation getModulation(size_t index) const;
    
    // Par ID (retourne un élément invalide si non trouvé)
    Section getSectionById(int id) const;
    Modulation getModulationById(int id) const;
    
    // Conversion ID ↔ Index
    int getSectionIndexById(int id) const;      // Retourne -1 si non trouvé
    int getModulationIndexById(int id) const;   // Retourne -1 si non trouvé
    
    // Collections complètes
    std::vector<Section> getSections() const;
    std::vector<Modulation> getModulations() const;
    
    // Helper pour récupérer les sections adjacentes d'une modulation
    std::pair<Section, Section> getAdjacentSections(const Modulation& modulation) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // INFORMATIONS
    // ═══════════════════════════════════════════════════════════════════════════
    
    size_t getSectionCount() const;
    size_t getModulationCount() const;
    int getNumElements() const;  // Total (sections + modulations)
    bool isEmpty() const;
    
    void setTitle(const juce::String& newTitle);
    juce::String getTitle() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // VALIDATION
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Vérifie que tous les invariants sont respectés.
     * En mode DEBUG, lance une assertion si un invariant est violé.
     * @return true si la structure est valide
     */
    bool hasValidStructure() const;
    
    /**
     * Vérifie si la pièce est complète (non vide et valide)
     */
    bool isComplete() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // STATISTIQUES ET DEBUG
    // ═══════════════════════════════════════════════════════════════════════════
    
    int getTotalChordCount() const;
    juce::String toString() const;
    juce::String getDetailedSummary() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AU VALUETREE (pour les listeners UI)
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::ValueTree getState() const { return state; }
    juce::ValueTree& getState() { return state; }
    juce::UndoManager& getUndoManager() { return undoManager; }
    const juce::UndoManager& getUndoManager() const { return undoManager; }
    
private:
    juce::ValueTree state;
    juce::UndoManager undoManager;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // GÉNÉRATION D'IDS (CENTRALISÉE - SEUL ENDROIT OÙ LES IDS SONT CRÉÉS)
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Génère le prochain ID disponible pour une Section.
     * Parcourt les sections existantes et retourne max(id) + 1, ou 0 si aucune.
     */
    int generateNextSectionId() const;
    
    /**
     * Génère le prochain ID disponible pour une Modulation.
     * Parcourt les modulations existantes et retourne max(id) + 1, ou 0 si aucune.
     */
    int generateNextModulationId() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // HELPERS INTERNES
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Crée un nœud Section avec un ID généré par Piece.
     */
    juce::ValueTree createSectionNode(const juce::String& name);
    
    /**
     * Crée un nœud Modulation avec un ID généré par Piece.
     */
    juce::ValueTree createModulationNode(int fromSectionId, int toSectionId);
    
    /**
     * Trouve l'index ValueTree d'un enfant par son type et son ID.
     * @return L'index dans state.getChild(), ou -1 si non trouvé
     */
    int findValueTreeIndex(const juce::Identifier& type, int id) const;
    
    /**
     * Récupère tous les enfants d'un type donné (dans l'ordre du ValueTree)
     */
    std::vector<juce::ValueTree> getChildrenOfType(const juce::Identifier& type) const;
    
    /**
     * Récupère le n-ième enfant d'un type donné
     */
    juce::ValueTree getChildOfType(const juce::Identifier& type, size_t index) const;
    
    /**
     * Vérifie les invariants et lance une assertion en mode DEBUG si violation
     */
    void assertInvariants() const;
};
