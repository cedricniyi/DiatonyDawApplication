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

    // Structure pour les degrés
    struct DegreeInfo {
        int value;
        std::string name;
    };

    // Structure pour les états d'accord
    struct ChordStateInfo {
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

    const std::vector<DegreeInfo> DEGREES = {
        {FIRST_DEGREE, degreeNames[0]},
        {SECOND_DEGREE, degreeNames[1]},
        {THIRD_DEGREE, degreeNames[2]},
        {FOURTH_DEGREE, degreeNames[3]},
        {FIFTH_DEGREE, degreeNames[4]},
        {SIXTH_DEGREE, degreeNames[5]},
        {SEVENTH_DEGREE, degreeNames[6]},
        {FIFTH_DEGREE_APPOGIATURA, degreeNames[7]},
        {FIVE_OF_TWO, degreeNames[8]},
        {FIVE_OF_THREE, degreeNames[9]},
        {FIVE_OF_FOUR, degreeNames[10]},
        {FIVE_OF_FIVE, degreeNames[11]},
        {FIVE_OF_SIX, degreeNames[12]},
        {FIVE_OF_SEVEN, degreeNames[13]},
        {FLAT_TWO, degreeNames[14]},
        {AUGMENTED_SIXTH, degreeNames[15]}
    };

    // États d'accord
    const std::vector<ChordStateInfo> CHORD_STATES = {
        {FUNDAMENTAL_STATE, stateNames[0]},
        {FIRST_INVERSION, stateNames[1]},
        {SECOND_INVERSION, stateNames[2]},
        {THIRD_INVERSION, stateNames[3]},
        {FOURTH_INVERSION, stateNames[4]}
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

    // Nouvelles fonctions pour les degrés
    inline std::string getDegreeName(int degreeValue) {
        for (const auto& degree : DEGREES) {
            if (degree.value == degreeValue) {
                return degree.name;
            }
        }
        return "Unknown Degree";
    }

    inline int getDegreeValue(const std::string& degreeName) {
        for (const auto& degree : DEGREES) {
            if (degree.name == degreeName) {
                return degree.value;
            }
        }
        return -1;
    }

    // Nouvelles fonctions pour les états d'accord
    inline std::string getChordStateName(int stateValue) {
        for (const auto& state : CHORD_STATES) {
            if (state.value == stateValue) {
                return state.name;
            }
        }
        return "Unknown Chord State";
    }

    inline int getChordStateValue(const std::string& stateName) {
        for (const auto& state : CHORD_STATES) {
            if (state.name == stateName) {
                return state.value;
            }
        }
        return -1;
    }
}

