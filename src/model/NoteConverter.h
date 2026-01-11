#pragma once
#include "DiatonyTypes.h"

namespace Diatony {

/** @brief Convertit BaseNote + Alteration → Note chromatique (0-11). Ex: Cb→B, B#→C */
inline Note toDiatonyNote(BaseNote base, Alteration alt) {
    int midiValue = static_cast<int>(base) + static_cast<int>(alt);
    return static_cast<Note>((midiValue % 12 + 12) % 12);  // Modulo robuste (négatifs)
}

/** @brief Fonction inverse : Note + Alteration → BaseNote (C,D,E,F,G,A,B) */
inline BaseNote toBaseNote(Note note, Alteration alt) {
    int baseValue = static_cast<int>(note) - static_cast<int>(alt);
    return static_cast<BaseNote>((baseValue % 12 + 12) % 12);
}

}