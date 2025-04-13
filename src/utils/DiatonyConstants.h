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

    struct ChordQualityInfo {
        int value;
        std::string name;
    };

    // ========================================================
        
    const std::vector<NoteInfo> NOTES = {
        {C, noteNames[0]},          // C
        {C_SHARP, "C♯ (C sharp)"},  // C#/Db
        {D, noteNames[2]},          // D
        {E_FLAT, "E♭ (E flat)"},     // Eb
        {E, noteNames[4]},          // E
        {F, noteNames[5]},          // F
        {F_SHARP, "F♯ (F sharp)"},  // F#
        {G, noteNames[7]},          // G
        {A_FLAT, "A♭ (A flat)"},     // Ab
        {A, noteNames[9]},          // A
        {B_FLAT, "B♭ (B flat)"},     // Bb
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

    const std::vector<ChordStateInfo> CHORD_STATES = {
        {FUNDAMENTAL_STATE, stateNames[0]},
        {FIRST_INVERSION, "1st"},
        {SECOND_INVERSION, "2nd"},
        {THIRD_INVERSION, "3rd"}
    };

    const std::vector<ChordQualityInfo> CHORD_QUALITIES = {
        {MAJOR_CHORD                , "maj"},
        {MINOR_CHORD                , "min"},
        {DIMINISHED_CHORD           , "dim"},
        {AUGMENTED_CHORD            , "aug"},
        {AUGMENTED_SIXTH_CHORD      , "6+"},
        {DOMINANT_SEVENTH_CHORD     , "7"},
        {MAJOR_SEVENTH_CHORD        , "maj7"},
        {MINOR_SEVENTH_CHORD        , "min7"},
        {DIMINISHED_SEVENTH_CHORD   , "dim7"},
        {HALF_DIMINISHED_CHORD      , "ø"},
        {MINOR_MAJOR_SEVENTH_CHORD  , "minmaj7"},
        {MAJOR_NINTH_DOMINANT_CHORD , "9"},
        {MINOR_NINTH_DOMINANT_CHORD , "9b"} // 9♭ OR 7♭9
    };

    // ========================================================

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

    inline std::string getChordQualityName(int qualityValue) {
        for (const auto& quality : CHORD_QUALITIES) {
            if (quality.value == qualityValue) {
                return quality.name;
            }
        }
        return "Unknown Chord Quality";
    }

    inline int getChordQualityValue(const std::string& qualityName) {
        for (const auto& quality : CHORD_QUALITIES) {
            if (quality.name == qualityName) {
                return quality.value;
            }
        }
        return -1;
    }
}

