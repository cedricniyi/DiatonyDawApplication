#pragma once
#include "model/DiatonyTypes.h"
#include "model/NoteConverter.h"
#include <juce_core/juce_core.h>

/** @brief Fonctions de conversion des enums Diatony vers des chaînes affichables. */
namespace DiatonyText {

    /** @brief Retourne le nom de la note de base (C, D, E...). */
    inline juce::String getBaseNoteName(Diatony::BaseNote note) {
        switch (note) {
            case Diatony::BaseNote::C: return "C";
            case Diatony::BaseNote::D: return "D";
            case Diatony::BaseNote::E: return "E";
            case Diatony::BaseNote::F: return "F";
            case Diatony::BaseNote::G: return "G";
            case Diatony::BaseNote::A: return "A";
            case Diatony::BaseNote::B: return "B";
        }
        return {};
    }
    
    /** @brief Retourne le symbole d'altération (♭, ♮, ♯). */
    inline juce::String getAlterationSymbol(Diatony::Alteration alt) {
        switch (alt) {
            case Diatony::Alteration::Flat:    return juce::String::fromUTF8("♭");
            case Diatony::Alteration::Natural: return juce::String::fromUTF8("♮");
            case Diatony::Alteration::Sharp:   return juce::String::fromUTF8("♯");
        }
        return {};
    }
    
    /** @brief Retourne le nom complet d'une note (ex: "C♯"). */
    inline juce::String getFullNoteName(Diatony::BaseNote base, Diatony::Alteration alt) {
        return getBaseNoteName(base) + getAlterationSymbol(alt);
    }
    
    /** @brief Retourne le nom d'une note chromatique (convertit en BaseNote). */
    inline juce::String getNoteName(Diatony::Note note, Diatony::Alteration alt) {
        Diatony::BaseNote base = Diatony::toBaseNote(note, alt);
        return getBaseNoteName(base) + getAlterationSymbol(alt);
    }
    
    /** @brief Retourne le nom du mode (Major, Minor). */
    inline juce::String getModeName(Diatony::Mode mode) {
        switch (mode) {
            case Diatony::Mode::Major: return "Major";
            case Diatony::Mode::Minor: return "Minor";
        }
        return {};
    }
    
    /** @brief Retourne le nom du type de modulation. */
    inline juce::String getModulationTypeName(Diatony::ModulationType type) {
        switch (type) {
            case Diatony::ModulationType::PerfectCadence: return "Perfect Cadence";
            case Diatony::ModulationType::PivotChord:     return "Pivot Chord";
            case Diatony::ModulationType::Alteration:     return "Alteration";
            case Diatony::ModulationType::Chromatic:      return "Chromatic";
        }
        return {};
    }
    
    /** @brief Retourne le chiffre romain du degré d'accord. */
    inline juce::String getChordDegreeName(Diatony::ChordDegree degree) {
        switch (degree) {
            case Diatony::ChordDegree::First:            return "I";
            case Diatony::ChordDegree::Second:           return "II";
            case Diatony::ChordDegree::Third:            return "III";
            case Diatony::ChordDegree::Fourth:           return "IV";
            case Diatony::ChordDegree::Fifth:            return "V";
            case Diatony::ChordDegree::Sixth:            return "VI";
            case Diatony::ChordDegree::Seventh:          return "VII";
            case Diatony::ChordDegree::FifthAppogiatura: return "V (App)";
            case Diatony::ChordDegree::FiveOfTwo:        return "V/II";
            case Diatony::ChordDegree::FiveOfThree:      return "V/III";
            case Diatony::ChordDegree::FiveOfFour:       return "V/IV";
            case Diatony::ChordDegree::FiveOfFive:       return "V/V";
            case Diatony::ChordDegree::FiveOfSix:        return "V/VI";
            case Diatony::ChordDegree::FiveOfSeven:      return "V/VII";
            case Diatony::ChordDegree::FlatTwo:          return juce::String::fromUTF8("♭II");
            case Diatony::ChordDegree::AugmentedSixth:   return "Aug6";
        }
        return {};
    }
    
    /** @brief Retourne le nom de l'état/renversement d'accord. */
    inline juce::String getChordStateName(Diatony::ChordState state) {
        switch (state) {
            case Diatony::ChordState::Fundamental:       return "Root";
            case Diatony::ChordState::FirstInversion:    return "First inversion";
            case Diatony::ChordState::SecondInversion:   return "Second inversion";
            case Diatony::ChordState::ThirdInversion:    return "Third inversion";
            case Diatony::ChordState::FourthInversion:   return "Fourth inversion";
        }
        return {};
    }
    
    /** @brief Retourne le nom complet de la qualité d'accord. */
    inline juce::String getChordQualityName(Diatony::ChordQuality quality) {
        switch (quality) {
            case Diatony::ChordQuality::Auto:               return "Auto";
            case Diatony::ChordQuality::Major:              return "Major";
            case Diatony::ChordQuality::Minor:              return "Minor";
            case Diatony::ChordQuality::Diminished:         return "Diminished";
            case Diatony::ChordQuality::Augmented:          return "Augmented";
            case Diatony::ChordQuality::AugmentedSixth:     return "6te Aug";
            case Diatony::ChordQuality::DominantSeventh:    return "Dominant Seventh";
            case Diatony::ChordQuality::MajorSeventh:       return "Major Seventh";
            case Diatony::ChordQuality::MinorSeventh:       return "Minor Seventh";
            case Diatony::ChordQuality::DiminishedSeventh:  return "Diminished Seventh";
            case Diatony::ChordQuality::HalfDiminished:     return "Half-diminished";
            case Diatony::ChordQuality::MinorMajorSeventh:  return "Minor Major Seventh";
            case Diatony::ChordQuality::MajorNinthDominant: return "Major Ninth Dominant";
            case Diatony::ChordQuality::MinorNinthDominant: return "Minor Ninth Dominant";
        }
        return {};
    }
    
    /** @brief Symboles compacts pour affichage dans les listes d'accords (M, m, °...). */
    inline juce::String getChordQualitySymbol(Diatony::ChordQuality quality) {
        switch (quality) {
            case Diatony::ChordQuality::Auto:               return "?";
            case Diatony::ChordQuality::Major:              return "M";
            case Diatony::ChordQuality::Minor:              return "m";
            case Diatony::ChordQuality::Diminished:         return juce::String::fromUTF8("°");
            case Diatony::ChordQuality::Augmented:          return "+";
            case Diatony::ChordQuality::AugmentedSixth:     return juce::String::fromUTF8("⁺⁶");
            case Diatony::ChordQuality::DominantSeventh:    return juce::String::fromUTF8("⁷");
            case Diatony::ChordQuality::MajorSeventh:       return juce::String::fromUTF8("M⁷");
            case Diatony::ChordQuality::MinorSeventh:       return juce::String::fromUTF8("m⁷");
            case Diatony::ChordQuality::DiminishedSeventh:  return juce::String::fromUTF8("°⁷");
            case Diatony::ChordQuality::HalfDiminished:     return juce::String::fromUTF8("ø⁷");
            case Diatony::ChordQuality::MinorMajorSeventh:  return juce::String::fromUTF8("mM⁷");
            case Diatony::ChordQuality::MajorNinthDominant: return juce::String::fromUTF8("⁹");
            case Diatony::ChordQuality::MinorNinthDominant: return juce::String::fromUTF8("m⁹");
        }
        return {};
    }
    
    /** @brief Abréviations courtes pour affichage compact (M, m, dim, aug...). */
    inline juce::String getShortQualityName(Diatony::ChordQuality quality) {
        switch (quality) {
            case Diatony::ChordQuality::Auto:               return "Auto";
            case Diatony::ChordQuality::Major:              return "M";
            case Diatony::ChordQuality::Minor:              return "m";
            case Diatony::ChordQuality::Diminished:         return "dim";
            case Diatony::ChordQuality::Augmented:          return "aug";
            case Diatony::ChordQuality::AugmentedSixth:     return "Aug6";
            case Diatony::ChordQuality::DominantSeventh:    return "7";
            case Diatony::ChordQuality::MajorSeventh:       return "M7";
            case Diatony::ChordQuality::MinorSeventh:       return "m7";
            case Diatony::ChordQuality::DiminishedSeventh:  return "dim7";
            case Diatony::ChordQuality::HalfDiminished:     return "m7b5";
            case Diatony::ChordQuality::MinorMajorSeventh:  return "mM7";
            case Diatony::ChordQuality::MajorNinthDominant: return "9";
            case Diatony::ChordQuality::MinorNinthDominant: return "m9";
        }
        return "?";
    }
    
    /** @brief Abréviations courtes pour les états d'accords (Root, 1st, 2nd...). */
    inline juce::String getShortStateName(Diatony::ChordState state) {
        switch (state) {
            case Diatony::ChordState::Fundamental:      return "Root";
            case Diatony::ChordState::FirstInversion:   return "1st";
            case Diatony::ChordState::SecondInversion:  return "2nd";
            case Diatony::ChordState::ThirdInversion:   return "3rd";
            case Diatony::ChordState::FourthInversion:  return "4th";
        }
        return "?";
    }
}
