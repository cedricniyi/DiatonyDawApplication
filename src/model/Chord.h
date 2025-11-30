#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * Wrapper léger autour d'un ValueTree représentant un accord.
 * 
 * RESPONSABILITÉS:
 * - Fournir une interface de haut niveau pour lire/écrire les propriétés
 * 
 * ARCHITECTURE:
 * - Ne stocke AUCUNE donnée en dehors du ValueTree
 * - Ne génère AUCUN ID (c'est Progression qui gère ça)
 * - Ne crée PAS de nouveaux nœuds (c'est Progression qui gère ça)
 * - Est un simple "wrapper" / "vue" sur le ValueTree
 * 
 * PROPRIÉTÉS GÉRÉES:
 * - id: Identifiant unique (géré par Progression)
 * - degree: Degré de l'accord (I, II, III, IV, V, VI, VII)
 * - quality: Qualité de l'accord (Major, Minor, Diminished, etc.)
 * - state: État/renversement de l'accord (Fundamental, FirstInversion, etc.)
 */
class Chord {
public:
    /**
     * Constructeur wrapper - prend un ValueTree existant.
     * @param state ValueTree de type CHORD
     */
    explicit Chord(juce::ValueTree state);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // SETTERS (modifient directement le ValueTree)
    // ═══════════════════════════════════════════════════════════════════════════
    
    void setDegree(Diatony::ChordDegree newDegree);
    void setQuality(Diatony::ChordQuality newQuality);
    void setChordState(Diatony::ChordState newState);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // GETTERS (lisent directement du ValueTree)
    // ═══════════════════════════════════════════════════════════════════════════
    
    int getId() const;
    Diatony::ChordDegree getDegree() const;
    Diatony::ChordQuality getQuality() const;
    Diatony::ChordState getChordState() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AU VALUETREE
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::CHORD); }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // MÉTHODES UTILITAIRES
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    // Helpers pour la conversion des types enum ↔ int
    static int degreeToInt(Diatony::ChordDegree degree);
    static Diatony::ChordDegree intToDegree(int value);
    static int qualityToInt(Diatony::ChordQuality quality);
    static Diatony::ChordQuality intToQuality(int value);
    static int stateToInt(Diatony::ChordState state);
    static Diatony::ChordState intToState(int value);
};
