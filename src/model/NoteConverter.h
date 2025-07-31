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
}