#pragma once

#include <JuceHeader.h>
#include "../../../../utils/FontManager.h"

//==============================================================================
// Classe pour une touche de piano
class PianoKey : public juce::Component
{
public:
    PianoKey(const juce::String& noteName, bool isBlackKey = false);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseDown(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    
    void setActive(bool active);
    
    const juce::String& getNoteName() const;
    bool isBlackKey() const;
    
private:
    juce::String name;
    bool isBlack;
    bool isActive;
    juce::Label label;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoKey)
}; 