#pragma once

#include <JuceHeader.h>
#include "ui/extra/Component/Panel/ColoredPanel.h"
#include "ui/extra/Button/StyledButton.h"
#include "ui/extra/Button/IconStyledButton.h"
#include "MidiDragZone.h"
#include <memory>

class FontManager;
class AppController;
class AudioPluginAudioProcessorEditor;

/** @brief Panneau d'en-tête avec logo, bouton Generate, zone MIDI et bouton History. */
class HeaderPanel : public ColoredPanel, public juce::ValueTree::Listener
{
public:
    HeaderPanel();
    ~HeaderPanel() override;

    void setAppState(juce::ValueTree& state);
    void resized() override;
    void paint(juce::Graphics& g) override;
    void parentHierarchyChanged() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;

private:
    void updateDockState();
    void findAppController();
    void connectGenerateButton();
    void loadLogo();
    
    AppController* appController = nullptr;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    std::unique_ptr<juce::Drawable> logoDrawable;
    juce::Label mainLabel;
    StyledButton generateButton;
    MidiDragZone midiDragZone;
    std::unique_ptr<IconStyledButton> hamburgerButton;
    juce::ValueTree appState;
    
    int titleZoneWidth = 250;
    static constexpr int LOGO_SIZE = 32;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
