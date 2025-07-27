#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"

// Forward declarations
class FontManager;

class HeaderPanel : public ColoredPanel, public juce::ValueTree::Listener
{
public:
    HeaderPanel();
    ~HeaderPanel() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);

    void resized() override;
    void paint (juce::Graphics& g) override;

    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;

private:
    void updateDockState();
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Label      mainLabel;
    StyledButton     dButton;
    juce::ValueTree  appState;  // Référence au ValueTree global

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
}; 