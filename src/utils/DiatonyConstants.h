//
// Created by Cédric Niyikiza on 10/03/2025.
//

#pragma once

#include "../../Diatony/c++/headers/aux/Utilities.hpp"
#include <map>
#include <string>
#include <vector>

namespace DiatonyConstants {
    // Structure pour représenter une note avec son nom et sa valeur
    struct NoteInfo {
        int value;
        std::string name;
    };

    // Vecteur qui relie les valeurs des notes à leurs noms
    const std::vector<NoteInfo> NOTES = {
        {C, noteNames[0]},          // C
        {C_SHARP, noteNames[1]},    // C#/Db
        {D, noteNames[2]},          // D
        {E_FLAT, noteNames[3]},     // Eb
        {E, noteNames[4]},          // E
        {F, noteNames[5]},          // F
        {F_SHARP, noteNames[6]},    // F#
        {G, noteNames[7]},          // G
        {A_FLAT, noteNames[8]},     // Ab
        {A, noteNames[9]},          // A
        {B_FLAT, noteNames[10]},    // Bb
        {B, noteNames[11]}          // B
    };

    // Fonction utilitaire pour obtenir le nom d'une note à partir de sa valeur
    inline std::string getNoteName(int noteValue) {
        noteValue = noteValue % 12; // Normaliser la valeur entre 0 et 11
        for (const auto& note : NOTES) {
            if (note.value == noteValue) {
                return note.name;
            }
        }
        return "Unknown";
    }

    // Fonction utilitaire pour obtenir la valeur d'une note à partir de son nom
    inline int getNoteValue(const std::string& noteName) {
        for (const auto& note : NOTES) {
            if (note.name == noteName) {
                return note.value;
            }
        }
        return -1; // Valeur invalide
    }
}

