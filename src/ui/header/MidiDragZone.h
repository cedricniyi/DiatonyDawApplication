#pragma once

#include <JuceHeader.h>

// Forward declarations
class AppController;

/**
 * Zone de drag & drop pour exporter le fichier MIDI généré vers un DAW.
 * 
 * Cette zone permet à l'utilisateur de glisser-déposer le fichier MIDI
 * directement vers une piste de son DAW, sans passer par l'explorateur.
 * 
 * États visuels :
 * - Inactif : grisé, pas de fichier disponible
 * - Prêt : coloré, fichier MIDI disponible pour le drag
 */
class MidiDragZone : public juce::Component,
                     public juce::SettableTooltipClient,
                     public juce::ValueTree::Listener
{
public:
    MidiDragZone();
    ~MidiDragZone() override;

    /**
     * Configure le ValueTree d'état de sélection à écouter.
     * La zone surveillera les propriétés "generationStatus" et "midiFilePath".
     */
    void setSelectionState(juce::ValueTree& state);

    // === Component overrides ===
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    // === ValueTree::Listener overrides ===
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}

private:
    /** Met à jour l'état de la zone en fonction du selectionState */
    void updateFromSelectionState();

    /** Vérifie si un fichier MIDI est disponible pour le drag */
    bool isMidiFileAvailable() const;

    // === Données membres ===
    juce::ValueTree selectionState;  // Référence au ValueTree de sélection
    juce::File midiFile;             // Fichier MIDI courant (si disponible)
    bool isHovering = false;         // État de survol pour le feedback visuel

    // === Couleurs ===
    static constexpr juce::uint32 inactiveColour = 0xFF555555;  // Gris foncé
    static constexpr juce::uint32 readyColour = 0xFF4A90A4;     // Bleu tonique
    static constexpr juce::uint32 hoverColour = 0xFF5BA0B4;     // Bleu plus clair au survol
    static constexpr float cornerRadius = 6.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDragZone)
};

