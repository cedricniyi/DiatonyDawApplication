#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Chord.h"
#include "ModelIdentifiers.h"

/**
 * Wrapper autour d'un ValueTree représentant une progression harmonique.
 * 
 * RESPONSABILITÉS:
 * - Gérer la collection d'accords (ajout, suppression, accès)
 * - Générer les IDs des accords (comme Piece gère les IDs des Sections)
 * 
 * ARCHITECTURE:
 * - Ne stocke AUCUNE donnée en dehors du ValueTree
 * - Gère les IDs des Chords enfants (max + 1)
 * - Pas de std::vector<Chord> caché - lit toujours du ValueTree
 * 
 * PROPRIÉTÉS GÉRÉES:
 * - id: Identifiant unique (géré par Piece/Section)
 * - Chords (enfants): Collection d'accords
 */
class Progression {
public:
    /**
     * Constructeur wrapper - prend un ValueTree existant.
     * @param state ValueTree de type PROGRESSION
     */
    explicit Progression(juce::ValueTree state);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // GESTION DES ACCORDS
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Ajoute un nouvel accord à la fin de la progression.
     * L'ID est généré automatiquement par Progression.
     */
    void addChord(Diatony::ChordDegree degree, 
                  Diatony::ChordQuality quality = Diatony::ChordQuality::Auto, 
                  Diatony::ChordState chordState = Diatony::ChordState::Fundamental);
    
    /**
     * Insère un accord à une position donnée.
     */
    void insertChord(size_t index, Diatony::ChordDegree degree,
                     Diatony::ChordQuality quality = Diatony::ChordQuality::Auto,
                     Diatony::ChordState chordState = Diatony::ChordState::Fundamental);
    
    /**
     * Supprime l'accord à l'index donné.
     */
    void removeChord(size_t index);
    
    /**
     * Vide tous les accords de la progression.
     */
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AUX ACCORDS
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Retourne l'accord à l'index donné.
     */
    Chord getChord(size_t index) const;
    Chord getChord(size_t index);
    
    /**
     * Retourne l'accord par son ID.
     */
    Chord getChordById(int id) const;
    
    /**
     * Convertit un ID en index.
     * @return L'index ou -1 si non trouvé
     */
    int getChordIndexById(int id) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // INFORMATIONS
    // ═══════════════════════════════════════════════════════════════════════════
    
    size_t size() const;
    bool isEmpty() const;
    int getId() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AU VALUETREE
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::PROGRESSION); }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // MÉTHODES UTILITAIRES
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    /**
     * Génère le prochain ID disponible pour un Chord.
     * Parcourt les accords existants et retourne max(id) + 1, ou 0 si aucun.
     */
    int generateNextChordId() const;
    
    /**
     * Crée un nœud Chord avec l'ID généré par Progression.
     */
    juce::ValueTree createChordNode(Diatony::ChordDegree degree,
                                    Diatony::ChordQuality quality,
                                    Diatony::ChordState chordState);
    
    /**
     * Valide qu'un index est dans les bornes.
     */
    void validateIndex(size_t index) const;
};
