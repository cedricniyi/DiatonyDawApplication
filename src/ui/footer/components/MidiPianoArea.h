#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "MidiPianoContentArea.h"

// Forward declarations
class FontManager;

//==============================================================================
class MidiPianoArea : public ColoredPanel
{
public:
    MidiPianoArea();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** Getter pour accéder au contentArea directement */
    MidiPianoContentArea& getContentArea();
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    juce::Label headerLabel;
    MidiPianoContentArea contentArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPianoArea)
}; 