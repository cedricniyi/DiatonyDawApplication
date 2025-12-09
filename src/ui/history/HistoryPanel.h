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
 * Contient les métadonnées d'une solution générée précédemment,
 * incluant les fichiers source pour le drag and drop.
 */
struct HistoryItem
{
    juce::File diatonyFile;    // Fichier .diatony source (XML avec ValueTree)
    juce::File midiFile;       // Fichier .mid associé (même nom, extension différente)
    juce::String name;         // Nom affiché (nom du fichier sans extension)
    juce::Time timestamp;      // Date de modification du fichier
    juce::String startKey;     // Tonalité de départ (ex: "Cm", "Eb")
    int numSections;           // Nombre de sections/progressions
    int numModulations;        // Nombre de modulations
    int numChords;             // Nombre total d'accords
};

//==============================================================================
// Forward declaration
class HistoryPanel;

//==============================================================================
/**
 * @brief Composant pour une ligne de l'historique avec support drag and drop
 * 
 * Ce composant est créé pour chaque row visible de la ListBox.
 * Il gère le rendu et le drag du fichier MIDI vers le DAW.
 */
class HistoryRowComponent : public juce::Component
{
public:
    HistoryRowComponent(HistoryPanel& parentPanel);
    
    void setRowData(const HistoryItem& item, int rowNum, bool isSelected);
    void paint(juce::Graphics& g) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    
private:
    HistoryPanel& owner;
    HistoryItem currentItem;
    int rowNumber = 0;
    bool selected = false;
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HistoryRowComponent)
};

//==============================================================================
/**
 * @brief Panneau latéral droit affichant l'historique des solutions
 * 
 * Ce panneau se révèle/masque avec animation via SlidingPanelAnimator.
 * Il contient un header "History" et une ListBox scrollable affichant
 * les solutions précédemment générées avec support drag and drop vers le DAW.
 */
class HistoryPanel : public juce::Component, 
                     public juce::ValueTree::Listener,
                     public juce::ListBoxModel,
                     public juce::DragAndDropContainer
{
public:
    HistoryPanel();
    ~HistoryPanel() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === Chargement dynamique depuis le disque ===
    
    /** Scanne le dossier des solutions et charge les fichiers .diatony */
    void refreshFromDisk();
    
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
    
    /** Accès aux items pour les composants enfants */
    const std::vector<HistoryItem>& getItems() const { return items; }
    
    /** Formate la date pour l'affichage (heure si aujourd'hui, date sinon) */
    static juce::String formatTimestamp(const juce::Time& time);

    // === Callbacks pour que l'Animator puisse s'y abonner ===
    
    /** Callback déclenché quand historyPanelVisible change dans le ValueTree */
    std::function<void(bool)> onVisibilityChange;

    // =================================================================================
    // ListBoxModel interface
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                          int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForRow(int rowNumber, bool isRowSelected,
                                            juce::Component* existingComponentToUpdate) override;
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
    
    /** Parse un fichier .diatony et extrait les métadonnées */
    bool parseHistoryFile(const juce::File& file, HistoryItem& outItem);
    
    /** Convertit une note (0-11) et isMajor en label de tonalité (ex: "Cm", "Eb") */
    static juce::String noteToKeyLabel(int noteIndex, bool isMajor);
    
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
