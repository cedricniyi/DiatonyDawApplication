#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/extra/Button/IconStyledButton.h"
#include "MidiDragZone.h"
#include <memory>

// Forward declarations
class FontManager;
class AppController;
class AudioPluginAudioProcessorEditor;

class HeaderPanel : public ColoredPanel, public juce::ValueTree::Listener
{
public:
    HeaderPanel();
    ~HeaderPanel() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);

    void resized() override;
    void paint (juce::Graphics& g) override;
    
    // === DÉCOUVERTE DE SERVICE ===
    void parentHierarchyChanged() override;

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
    
    // === DÉCOUVERTE DE SERVICE ===
    AppController* appController = nullptr;
    void findAppController();
    void connectGenerateButton();
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Label      mainLabel;
    StyledButton     generateButton;  // Bouton Generate (vert)
    MidiDragZone     midiDragZone;    // Zone de drag MIDI
    StyledButton     dButton;         // Bouton D (gris)
    std::unique_ptr<IconStyledButton> hamburgerButton;  // Bouton menu hamburger (SVG)
    juce::ValueTree  appState;        // Référence au ValueTree global
    int              titleZoneWidth = 250;  // Largeur de la zone titre pour les séparations

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
