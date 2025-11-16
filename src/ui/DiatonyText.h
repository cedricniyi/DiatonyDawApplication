#pragma once
#include "model/DiatonyTypes.h"
#include "model/NoteConverter.h"
#include <juce_core/juce_core.h>

namespace DiatonyText {
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
        return {}; // Ne devrait jamais arriver
    }
    
    inline juce::String getAlterationSymbol(Diatony::Alteration alt) {
        switch (alt) {
            case Diatony::Alteration::Flat: return juce::String::fromUTF8("♭");
            case Diatony::Alteration::Natural: return juce::String::fromUTF8("♮");
            case Diatony::Alteration::Sharp: return juce::String::fromUTF8("♯");
        }
        return {}; // Ne devrait jamais arriver
    }
    
    inline juce::String getFullNoteName(Diatony::BaseNote base, Diatony::Alteration alt) {
        return getBaseNoteName(base) + getAlterationSymbol(alt);
    }
    
    // Version avec Note chromatique (convertit en BaseNote d'abord)
    inline juce::String getNoteName(Diatony::Note note, Diatony::Alteration alt) {
        Diatony::BaseNote base = Diatony::toBaseNote(note, alt);
        return getBaseNoteName(base) + getAlterationSymbol(alt);
    }
    
    inline juce::String getModeName(Diatony::Mode mode) {
        switch (mode) {
            case Diatony::Mode::Major: return "Major";
            case Diatony::Mode::Minor: return "Minor";
        }
        return {}; // Ne devrait jamais arriver
    }
    
    inline juce::String getModulationTypeName(Diatony::ModulationType type) {
        switch (type) {
            case Diatony::ModulationType::PerfectCadence: return juce::String::fromUTF8("Cadence Parfaite");
            case Diatony::ModulationType::PivotChord: return "Accord Pivot";
            case Diatony::ModulationType::Alteration: return juce::String::fromUTF8("Altération");
            case Diatony::ModulationType::Chromatic: return "Chromatique";
        }
        return {}; // Ne devrait jamais arriver
    }
    
    inline juce::String getChordDegreeName(Diatony::ChordDegree degree) {
        switch (degree) {
            case Diatony::ChordDegree::First: return "I";
            case Diatony::ChordDegree::Second: return "II";
            case Diatony::ChordDegree::Third: return "III";
            case Diatony::ChordDegree::Fourth: return "IV";
            case Diatony::ChordDegree::Fifth: return "V";
            case Diatony::ChordDegree::Sixth: return "VI";
            case Diatony::ChordDegree::Seventh: return "VII";
            case Diatony::ChordDegree::FifthAppogiatura: return "V (App)";
            case Diatony::ChordDegree::FiveOfTwo: return "V/II";
            case Diatony::ChordDegree::FiveOfThree: return "V/III";
            case Diatony::ChordDegree::FiveOfFour: return "V/IV";
            case Diatony::ChordDegree::FiveOfFive: return "V/V";
            case Diatony::ChordDegree::FiveOfSix: return "V/VI";
            case Diatony::ChordDegree::FiveOfSeven: return "V/VII";
            case Diatony::ChordDegree::FlatTwo: return juce::String::fromUTF8("♭II");
            case Diatony::ChordDegree::AugmentedSixth: return juce::String::fromUTF8("Aug6");
        }
        return {}; // Ne devrait jamais arriver
    }
    
    inline juce::String getChordStateName(Diatony::ChordState state) {
        switch (state) {
            case Diatony::ChordState::Fundamental: return "Fondamental";
            case Diatony::ChordState::FirstInversion: return juce::String::fromUTF8("1er renversement");
            case Diatony::ChordState::SecondInversion: return juce::String::fromUTF8("2e renversement");
            case Diatony::ChordState::ThirdInversion: return juce::String::fromUTF8("3e renversement");
            case Diatony::ChordState::FourthInversion: return juce::String::fromUTF8("4e renversement");
        }
        return {}; // Ne devrait jamais arriver
    }
    
    inline juce::String getChordQualityName(Diatony::ChordQuality quality) {
        switch (quality) {
            case Diatony::ChordQuality::Auto: return "Auto";
            case Diatony::ChordQuality::Major: return "Majeur";
            case Diatony::ChordQuality::Minor: return "Mineur";
            case Diatony::ChordQuality::Diminished: return juce::String::fromUTF8("Diminué");
            case Diatony::ChordQuality::Augmented: return juce::String::fromUTF8("Augmenté");
            case Diatony::ChordQuality::AugmentedSixth: return juce::String::fromUTF8("6te Aug");
            case Diatony::ChordQuality::DominantSeventh: return juce::String::fromUTF8("7e Dominante");
            case Diatony::ChordQuality::MajorSeventh: return juce::String::fromUTF8("7e Majeure");
            case Diatony::ChordQuality::MinorSeventh: return juce::String::fromUTF8("7e Mineure");
            case Diatony::ChordQuality::DiminishedSeventh: return juce::String::fromUTF8("7e Diminuée");
            case Diatony::ChordQuality::HalfDiminished: return juce::String::fromUTF8("Semi-diminué");
            case Diatony::ChordQuality::MinorMajorSeventh: return juce::String::fromUTF8("7e Min-Maj");
            case Diatony::ChordQuality::MajorNinthDominant: return juce::String::fromUTF8("9e Maj Dom");
            case Diatony::ChordQuality::MinorNinthDominant: return juce::String::fromUTF8("9e Min Dom");
        }
        return {}; // Ne devrait jamais arriver
    }
    
    // Symboles compacts pour affichage dans les listes d'accords
    inline juce::String getChordQualitySymbol(Diatony::ChordQuality quality) {
        switch (quality) {
            case Diatony::ChordQuality::Auto: return "?";
            case Diatony::ChordQuality::Major: return "M";
            case Diatony::ChordQuality::Minor: return "m";
            case Diatony::ChordQuality::Diminished: return juce::String::fromUTF8("°");
            case Diatony::ChordQuality::Augmented: return "+";
            case Diatony::ChordQuality::AugmentedSixth: return "⁺⁶";
            case Diatony::ChordQuality::DominantSeventh: return "⁷";
            case Diatony::ChordQuality::MajorSeventh: return "M⁷";
            case Diatony::ChordQuality::MinorSeventh: return "m⁷";
            case Diatony::ChordQuality::DiminishedSeventh: return "°⁷";
            case Diatony::ChordQuality::HalfDiminished: return "ø⁷";
            case Diatony::ChordQuality::MinorMajorSeventh: return "mM⁷";
            case Diatony::ChordQuality::MajorNinthDominant: return "⁹";
            case Diatony::ChordQuality::MinorNinthDominant: return "m⁹";
        }
        return {}; // Ne devrait jamais arriver
    }
}