#pragma once

#include <JuceHeader.h>
#include "components/MidiPianoArea.h"
#include "components/InteractivePiano/InteractivePiano.h"

// Forward declarations
class MidiPianoContentArea;

//==============================================================================
class FooterPanel : public juce::Component, public juce::ValueTree::Listener
{
public:
    FooterPanel();
    ~FooterPanel() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === Setters publics pour que l'Animator puisse les appeler ===
    
    /** Met à jour l'état d'expansion */
    void setExpanded(bool expanded);
    
    // === Accesseurs publics pour que l'Animator puisse les lire ===
    
    /** Retourne l'état d'expansion actuel */
    bool getExpanded() const;
    
    /** Retourne une référence au composant qui fade in/out */
    juce::Component& getFadingComponent();
    
    /** Retourne une référence à la fraction de grille pour l'animation */
    float& getGridFractionRef();
    
    /** Accès au MidiPianoContentArea pour configuration des callbacks */
    MidiPianoContentArea& getMidiPianoContentArea();

    // === Callbacks pour que l'Animator puisse s'y abonner ===
    
    /** Callback déclenché quand interactivePianoVisible change dans le ValueTree */
    std::function<void(bool)> onInteractivePianoVisibilityChange;

    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;
    
private:
    void updateFooterState();
    void updateInteractivePianoState();
    
    MidiPianoArea midiPianoArea;
    InteractivePiano interactivePiano;
    bool isExpanded; // État du toggle de la grille
    float gridTransitionFraction; // Paramètre d'interpolation (0 = compact, 1 = élargi)
    
    juce::ValueTree appState;      // Référence au ValueTree global
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FooterPanel)
}; 