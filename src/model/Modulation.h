#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * Wrapper léger autour d'un ValueTree représentant une modulation.
 * 
 * RESPONSABILITÉS:
 * - Fournir une interface de haut niveau pour lire/écrire les propriétés
 * - Donner accès aux références des sections liées
 * 
 * ARCHITECTURE:
 * - Ne stocke AUCUNE donnée en dehors du ValueTree
 * - Ne génère AUCUN ID (c'est Piece qui gère ça)
 * - Ne crée PAS de nouveaux nœuds (c'est Piece qui gère ça)
 * - Est un simple "wrapper" / "vue" sur le ValueTree
 * 
 * PROPRIÉTÉS GÉRÉES:
 * - id: Identifiant unique (géré par Piece)
 * - name: Nom de la modulation
 * - modulationType: Type de modulation (PivotChord, DirectModulation, etc.)
 * - fromSectionId: ID de la section source
 * - toSectionId: ID de la section destination
 * - fromChordIndex: Index de l'accord de départ dans la section source
 * - toChordIndex: Index de l'accord d'arrivée dans la section destination
 */
class Modulation {
public:
    /**
     * Constructeur wrapper - prend un ValueTree existant.
     * @param state ValueTree de type MODULATION
     */
    explicit Modulation(juce::ValueTree state);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // SETTERS (modifient directement le ValueTree)
    // ═══════════════════════════════════════════════════════════════════════════
    
    void setModulationType(Diatony::ModulationType newType);
    void setFromSectionId(int newFromSectionId);
    void setToSectionId(int newToSectionId);
    void setFromChordIndex(int newFromChordIndex);
    void setToChordIndex(int newToChordIndex);
    void setName(const juce::String& newName);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // GETTERS (lisent directement du ValueTree)
    // ═══════════════════════════════════════════════════════════════════════════
    
    int getId() const;
    Diatony::ModulationType getModulationType() const;
    int getFromSectionId() const;
    int getToSectionId() const;
    int getFromChordIndex() const;
    int getToChordIndex() const;
    juce::String getName() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AU VALUETREE
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::MODULATION); }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // MÉTHODES UTILITAIRES
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Vérifie si les références de sections sont définies
     */
    bool hasValidSectionReferences() const;
    
    /**
     * Vérifie si les indices d'accords sont définis
     */
    bool hasChordIndices() const;
    
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    // Helpers pour la conversion des types enum ↔ int
    static int modulationTypeToInt(Diatony::ModulationType type);
    static Diatony::ModulationType intToModulationType(int value);
};
