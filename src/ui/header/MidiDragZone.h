#pragma once

#include <JuceHeader.h>

class AppController;

/**
 * @brief Zone de drag & drop pour exporter le fichier MIDI vers un DAW.
 *
 * États visuels : inactif (grisé) ou prêt (coloré, fichier disponible).
 */
class MidiDragZone : public juce::Component,
                     public juce::SettableTooltipClient,
                     public juce::ValueTree::Listener
{
public:
    MidiDragZone();
    ~MidiDragZone() override;

    /** @brief Configure le ValueTree à écouter (propriétés generationStatus et midiFilePath). */
    void setSelectionState(juce::ValueTree& state);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}

private:
    void updateFromSelectionState();
    bool isMidiFileAvailable() const;

    juce::ValueTree selectionState;
    juce::File midiFile;
    bool isHovering = false;

    static constexpr juce::uint32 inactiveColour = 0xFF555555;  // Gris foncé
    static constexpr juce::uint32 readyColour = 0xFF4A90A4;     // Bleu tonique
    static constexpr juce::uint32 hoverColour = 0xFF5BA0B4;     // Bleu clair hover
    static constexpr float cornerRadius = 6.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDragZone)
};
