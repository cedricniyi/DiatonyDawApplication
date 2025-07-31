#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "InteractivePianoContentArea.h"

// Forward declarations
class FontManager;

//==============================================================================
// Conteneur principal avec padding
class InteractivePiano : public ColoredPanel
{
public:
    InteractivePiano(juce::Colour color);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::SharedResourcePointer<FontManager> fontManager;
    InteractivePianoContentArea contentArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InteractivePiano)
}; 