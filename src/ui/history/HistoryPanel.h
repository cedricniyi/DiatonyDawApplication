#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"

//==============================================================================
/**
 * @brief Panneau latéral droit affichant l'historique
 * 
 * Ce panneau se révèle/masque avec animation via HistoryPanelAnimator.
 * Il contient un header "History" et une zone de contenu scrollable.
 */
class HistoryPanel : public juce::Component, public juce::ValueTree::Listener
{
public:
    HistoryPanel();
    ~HistoryPanel() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === Setters publics pour que l'Animator puisse les appeler ===
    
    /** Met à jour l'état d'expansion du panneau (pour l'animator) */
    void setExpanded(bool expanded);
    
    // === Accesseurs publics pour que l'Animator puisse les lire ===
    
    /** Retourne l'état d'expansion actuel */
    bool getExpanded() const;
    
    /** Retourne une référence au composant qui fade in/out (le contenu) */
    juce::Component& getFadingComponent();
    
    /** Retourne une référence à la fraction de largeur pour l'animation */
    float& getWidthFractionRef();
    
    /** Retourne la fraction de largeur actuelle (lecture seule) */
    float getWidthFraction() const;

    // === Callbacks pour que l'Animator puisse s'y abonner ===
    
    /** Callback déclenché quand historyPanelVisible change dans le ValueTree */
    std::function<void(bool)> onVisibilityChange;

    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;
    
private:
    void updateVisibilityState();
    
    // Header du panneau
    juce::Label headerLabel;
    
    // Zone de contenu (placeholder pour l'instant)
    class ContentArea : public juce::Component
    {
    public:
        ContentArea();
        void paint(juce::Graphics& g) override;
    };
    
    ContentArea contentArea;
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    bool isPanelVisible;           // État de visibilité du panneau
    float widthTransitionFraction; // Paramètre d'interpolation (0 = fermé, 1 = 250px)
    
    juce::ValueTree appState;      // Référence au ValueTree global
    
    static constexpr int HEADER_HEIGHT = 60;  // Même hauteur que le HeaderPanel principal
    static constexpr int PANEL_WIDTH = 250;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HistoryPanel)
};

