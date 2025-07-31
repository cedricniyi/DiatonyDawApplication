#pragma once

#include <JuceHeader.h>
#include "PianoKey.h"

//==============================================================================
// Zone de contenu pour le piano (octave) - Simplifié
class InteractivePianoContentArea : public juce::Component
{
public:
    InteractivePianoContentArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    void setupWhiteKeys();
    void setupBlackKeys();
    void setupKeyGrid();
    void layoutBlackKeys(const juce::Rectangle<int>& bounds);
    
    juce::Grid keyGrid;
    
    // Touches blanches
    PianoKey whiteKeyC, whiteKeyD, whiteKeyE, whiteKeyF, whiteKeyG, whiteKeyA, whiteKeyB;
    
    // Touches noires
    PianoKey blackKeyCSharp, blackKeyDSharp, blackKeyFSharp, blackKeyGSharp, blackKeyASharp;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractivePianoContentArea)
}; 