#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Progression.h"
#include "DiatonyTypes.h"
#include "ModelIdentifiers.h"

/**
 * Wrapper léger autour d'un ValueTree représentant une section tonale.
 * 
 * RESPONSABILITÉS:
 * - Fournir une interface de haut niveau pour lire/écrire les propriétés
 * - Donner accès à la Progression contenue
 * 
 * ARCHITECTURE:
 * - Ne stocke AUCUNE donnée en dehors du ValueTree
 * - Ne génère AUCUN ID (c'est Piece qui gère ça)
 * - Ne crée PAS de nouveaux nœuds (c'est Piece qui gère ça)
 * - Est un simple "wrapper" / "vue" sur le ValueTree
 * 
 * PROPRIÉTÉS GÉRÉES:
 * - id: Identifiant unique (géré par Piece)
 * - name: Nom de la section
 * - tonalityNote: Note de la tonalité (C, D, E, etc.)
 * - tonalityAlteration: Altération (Natural, Sharp, Flat)
 * - isMajor: Mode majeur ou mineur
 * - Progression (enfant): Contient les accords
 */
class Section {
public:
    /**
     * Constructeur wrapper - prend un ValueTree existant.
     * @param state ValueTree de type SECTION
     */
    explicit Section(juce::ValueTree state);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // SETTERS (modifient directement le ValueTree)
    // ═══════════════════════════════════════════════════════════════════════════
    
    void setNote(Diatony::Note newNote);
    void setAlteration(Diatony::Alteration newAlteration);
    void setIsMajor(bool newIsMajor);
    void setName(const juce::String& newName);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // GETTERS (lisent directement du ValueTree)
    // ═══════════════════════════════════════════════════════════════════════════
    
    int getId() const;
    Diatony::Note getNote() const;
    Diatony::Alteration getAlteration() const;
    bool getIsMajor() const;
    juce::String getName() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS À LA PROGRESSION
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * Retourne la Progression contenue dans cette Section.
     * @return Wrapper Progression sur le ValueTree enfant
     */
    Progression getProgression() const;
    Progression getProgression();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ACCÈS AU VALUETREE
    // ═══════════════════════════════════════════════════════════════════════════
    
    juce::ValueTree getState() const { return state; }
    bool isValid() const { return state.isValid() && state.hasType(ModelIdentifiers::SECTION); }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // MÉTHODES UTILITAIRES
    // ═══════════════════════════════════════════════════════════════════════════
    
    bool isEmpty() const;
    bool hasProgression() const;
    juce::String toString() const;
    
private:
    juce::ValueTree state;
    
    // Helpers pour la conversion des types enum ↔ int
    static int noteToInt(Diatony::Note note);
    static Diatony::Note intToNote(int value);
    static int alterationToInt(Diatony::Alteration alteration);
    static Diatony::Alteration intToAlteration(int value);
};
