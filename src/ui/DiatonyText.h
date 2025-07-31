#pragma once
#include "model/DiatonyTypes.h"
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
}