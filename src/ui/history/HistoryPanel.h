#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"
#include "ui/extra/Button/StyledButton.h"
#include <vector>
#include <memory>

/** @brief Métadonnées d'une solution générée (pour drag & drop). */
struct HistoryItem
{
    juce::File diatonyFile;    // Fichier .diatony source (XML avec ValueTree)
    juce::File midiFile;       // Fichier .mid associé (même nom, extension différente)
    juce::String name;
    juce::Time timestamp;
    juce::String startKey;
    int numSections;
    int numModulations;
    int numChords;
};

class HistoryPanel;

/** @brief Composant pour une ligne de l'historique avec drag & drop. */
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

/**
 * @brief Panneau latéral affichant l'historique des solutions générées.
 *
 * Animation via SlidingPanelAnimator, ListBox scrollable avec drag & drop.
 */
class HistoryPanel : public juce::Component, 
                     public juce::ValueTree::Listener,
                     public juce::ListBoxModel,
                     public juce::DragAndDropContainer
{
public:
    HistoryPanel();
    ~HistoryPanel() override;

    void setAppState(juce::ValueTree& state);
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /** @brief Scanne le dossier des solutions et charge les fichiers .diatony. */
    void refreshFromDisk();
    
    void setExpanded(bool expanded);
    bool getExpanded() const;
    juce::Component& getFadingComponent();
    float& getWidthFractionRef();
    float getWidthFraction() const;
    const std::vector<HistoryItem>& getItems() const { return items; }
    
    /** @brief Formate le timestamp (heure si aujourd'hui, date sinon). */
    static juce::String formatTimestamp(const juce::Time& time);

    std::function<void(bool)> onVisibilityChange;

    // ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                          int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForRow(int rowNumber, bool isRowSelected,
                                            juce::Component* existingComponentToUpdate) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent& e) override;

    // ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;
    
private:
    void updateVisibilityState();
    bool parseHistoryFile(const juce::File& file, HistoryItem& outItem);
    static juce::String noteToKeyLabel(int noteIndex, bool isMajor);
    
    juce::Label headerLabel;
    
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
    
    bool isPanelVisible;
    float widthTransitionFraction;
    juce::ValueTree appState;
    
    static constexpr int HEADER_HEIGHT = 60;
    static constexpr int PANEL_WIDTH = 250;
    static constexpr int ROW_HEIGHT = 54;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HistoryPanel)
};
