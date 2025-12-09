#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/StyledButton.h"
#include <vector>
#include <memory>

//==============================================================================
/**
 * @brief Structure représentant un snapshot d'historique
 * 
 * Contient les métadonnées d'une solution générée précédemment.
 */
struct HistoryItem
{
    juce::String name;        // Nom de la solution (ex: "Idée Refrain V2")
    juce::String timestamp;   // Date/heure ("14:30" si aujourd'hui, "08/12" sinon)
    juce::String startKey;    // Tonalité de départ (ex: "Cm", "Eb")
    int numSections;          // Nombre de sections/progressions
    int numModulations;       // Nombre de modulations
    int numChords;            // Nombre total d'accords
};

//==============================================================================
/**
 * @brief Panneau latéral droit affichant l'historique des solutions
 * 
 * Ce panneau se révèle/masque avec animation via SlidingPanelAnimator.
 * Il contient un header "History" et une ListBox scrollable affichant
 * les solutions précédemment générées.
 */
class HistoryPanel : public juce::Component, 
                     public juce::ValueTree::Listener,
                     public juce::ListBoxModel
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
    
    /** Retourne une référence au composant qui fade in/out (la ListBox) */
    juce::Component& getFadingComponent();
    
    /** Retourne une référence à la fraction de largeur pour l'animation */
    float& getWidthFractionRef();
    
    /** Retourne la fraction de largeur actuelle (lecture seule) */
    float getWidthFraction() const;

    // === Callbacks pour que l'Animator puisse s'y abonner ===
    
    /** Callback déclenché quand historyPanelVisible change dans le ValueTree */
    std::function<void(bool)> onVisibilityChange;

    // =================================================================================
    // ListBoxModel interface
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                          int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;

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
    void generateMockData();
    
    // Header du panneau
    juce::Label headerLabel;
    
    // Conteneur pour le contenu qui fade (bouton + liste)
    class ContentContainer : public juce::Component
    {
    public:
        ContentContainer();
        void resized() override;
        
        std::unique_ptr<StyledButton> openFolderButton;
        juce::ListBox historyList;
        
        static constexpr int BUTTON_ZONE_HEIGHT = 50;
    };
    
    ContentContainer contentContainer;
    std::vector<HistoryItem> items;
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    bool isPanelVisible;           // État de visibilité du panneau
    float widthTransitionFraction; // Paramètre d'interpolation (0 = fermé, 1 = 250px)
    
    juce::ValueTree appState;      // Référence au ValueTree global
    
    static constexpr int HEADER_HEIGHT = 60;   // Même hauteur que le HeaderPanel principal
    static constexpr int PANEL_WIDTH = 250;
    static constexpr int ROW_HEIGHT = 54;      // Hauteur d'un item (2 lignes + padding)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HistoryPanel)
};
