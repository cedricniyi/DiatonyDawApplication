#pragma once
#include "DiatonyTypes.h"

namespace Diatony {
    /**
     * @brief Traduit une note de base et son altération en une note chromatique (0-11).
     * Gère les cas limites (ex: Cb -> B, B# -> C).
     */
    inline Note toDiatonyNote(BaseNote base, Alteration alt) {
        int midiValue = static_cast<int>(base) + static_cast<int>(alt);
        
        // Formule robuste pour le modulo qui gère les nombres négatifs
        midiValue = (midiValue % 12 + 12) % 12;
        
        return static_cast<Note>(midiValue);
    }
    
    /**
     * @brief Convertit une note chromatique et son altération en une note de base.
     * C'est la fonction inverse de toDiatonyNote.
     * 
     * Permet de retrouver la "lettre" de la note (C, D, E, F, G, A, B) à partir
     * de la note chromatique et de son altération.
     * 
     * Exemples:
     *   - toBaseNote(Note::C (0), Alteration::Natural (0)) → BaseNote::C
     *   - toBaseNote(Note::CSharp (1), Alteration::Sharp (1)) → BaseNote::C
     *   - toBaseNote(Note::B (11), Alteration::Flat (-1)) → BaseNote::C (car 11 - (-1) = 12 % 12 = 0 = Cb)
     *   - toBaseNote(Note::D (2), Alteration::Natural (0)) → BaseNote::D
     * 
     * @param note La note chromatique (0-11)
     * @param alt L'altération appliquée à cette note
     * @return La note de base correspondante (C, D, E, F, G, A, B)
     * 
     * @warning Cette fonction assume que le modèle est cohérent, c'est-à-dire que:
     *          note == toDiatonyNote(toBaseNote(note, alt), alt)
     *          Si cette invariant n'est pas respecté, le résultat peut être incorrect.
     */
    inline BaseNote toBaseNote(Note note, Alteration alt) {
        // BaseNote = Note - Alteration (modulo 12)
        int baseValue = static_cast<int>(note) - static_cast<int>(alt);
        
        // Formule robuste pour le modulo qui gère les nombres négatifs
        baseValue = (baseValue % 12 + 12) % 12;
        
        return static_cast<BaseNote>(baseValue);
    }
}