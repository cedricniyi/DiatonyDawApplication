#pragma once

/**
 * @file DiatonyTypes.h
 * @brief Définitions des types fortement typés pour le modèle Diatony
 * 
 * Ce fichier contient tous les types utilisés par le modèle de l'application,
 * indépendamment de la librairie Diatony sous-jacente. Les valeurs entières
 * correspondent exactement à celles définies dans Diatony/c++/headers/aux/Utilities.hpp
 */

namespace Diatony {

    /**
     * @brief Types de modulations disponibles
     * Correspond à l'enum 'modulations' de Utilities.hpp
     */
    enum class ModulationType : int {
        PerfectCadence = 0,     // PERFECT_CADENCE_MODULATION
        PivotChord = 1,         // PIVOT_CHORD_MODULATION  
        Alteration = 2,         // ALTERATION_MODULATION
        Chromatic = 3           // CHROMATIC_MODULATION
    };

    /**
     * @brief Degrés d'accords dans une tonalité
     * Correspond à l'enum 'degrees' de Utilities.hpp
     */
    enum class ChordDegree : int {
        First = 0,              // FIRST_DEGREE
        Second = 1,             // SECOND_DEGREE
        Third = 2,              // THIRD_DEGREE
        Fourth = 3,             // FOURTH_DEGREE
        Fifth = 4,              // FIFTH_DEGREE
        Sixth = 5,              // SIXTH_DEGREE
        Seventh = 6,            // SEVENTH_DEGREE
        FifthAppogiatura = 7,   // FIFTH_DEGREE_APPOGIATURA
        FiveOfTwo = 8,          // FIVE_OF_TWO
        FiveOfThree = 9,        // FIVE_OF_THREE
        FiveOfFour = 10,        // FIVE_OF_FOUR
        FiveOfFive = 11,        // FIVE_OF_FIVE
        FiveOfSix = 12,         // FIVE_OF_SIX
        FiveOfSeven = 13,       // FIVE_OF_SEVEN
        FlatTwo = 14,           // FLAT_TWO
        AugmentedSixth = 15     // AUGMENTED_SIXTH
    };

    /**
     * @brief Qualités d'accords (majeur, mineur, diminué, etc.)
     * Correspond à l'enum 'chordTypes' de Utilities.hpp
     */
    enum class ChordQuality : int {
        Major = 0,              // MAJOR_CHORD
        Minor = 1,              // MINOR_CHORD
        Diminished = 2,         // DIMINISHED_CHORD
        Augmented = 3,          // AUGMENTED_CHORD
        AugmentedSixth = 4,     // AUGMENTED_SIXTH_CHORD
        DominantSeventh = 5,    // DOMINANT_SEVENTH_CHORD
        MajorSeventh = 6,       // MAJOR_SEVENTH_CHORD
        MinorSeventh = 7,       // MINOR_SEVENTH_CHORD
        DiminishedSeventh = 8,  // DIMINISHED_SEVENTH_CHORD
        HalfDiminished = 9,     // HALF_DIMINISHED_CHORD
        MinorMajorSeventh = 10, // MINOR_MAJOR_SEVENTH_CHORD
        MajorNinthDominant = 11,// MAJOR_NINTH_DOMINANT_CHORD
        MinorNinthDominant = 12 // MINOR_NINTH_DOMINANT_CHORD
    };

    /**
     * @brief États d'inversion des accords
     * Correspond à l'enum 'chordStates' de Utilities.hpp
     */
    enum class ChordState : int {
        Fundamental = 0,        // FUNDAMENTAL_STATE
        FirstInversion = 1,     // FIRST_INVERSION
        SecondInversion = 2,    // SECOND_INVERSION
        ThirdInversion = 3,     // THIRD_INVERSION
        FourthInversion = 4     // FOURTH_INVERSION
    };

    /**
     * @brief Notes chromatiques (0-11)
     * Basé sur les constantes de notes de Utilities.hpp
     */
    enum class Note : int {
        C = 0,          // C / B_SHARP
        CSharp = 1,     // C_SHARP / D_FLAT
        D = 2,          // D
        DSharp = 3,     // D_SHARP / E_FLAT
        E = 4,          // E / F_FLAT
        F = 5,          // F / E_SHARP
        FSharp = 6,     // F_SHARP / G_FLAT
        G = 7,          // G
        GSharp = 8,     // G_SHARP / A_FLAT
        A = 9,          // A
        ASharp = 10,    // A_SHARP / B_FLAT
        B = 11          // B / C_FLAT
    };

    /**
     * @brief Les 7 notes de base du système diatonique.
     * Utilisé principalement par l'UI pour une sélection intuitive.
     */
    enum class BaseNote : int {
        C = 0, D = 2, E = 4, F = 5, G = 7, A = 9, B = 11
    };

    /**
     * @brief Altérations pour les notes
     */
    enum class Alteration : int {
        Flat = -1,      // Bémol
        Natural = 0,    // Naturel
        Sharp = 1       // Dièse
    };

} // namespace Diatony 