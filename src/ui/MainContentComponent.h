#pragma once

#include <JuceHeader.h>
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "history/HistoryPanel.h"
#include "extra/Component/DiatonyAlertWindow.h"

/**
 * @brief Composant principal qui contient le layout de l'interface.
 *
 * Gère le layout des panels (Header, Section) et supporte le Drag & Drop de fichiers .diatony.
 */
class MainContentComponent : public juce::Component,
                             public juce::ValueTree::Listener,
                             public juce::FileDragAndDropTarget,
                             public juce::DragAndDropTarget
{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    void setAppState(juce::ValueTree& state);
    void setSelectionState(juce::ValueTree& state);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    float& getHeaderFlexRef();
    float& getSectionFlexRef();
    
    HeaderPanel& getHeaderPanel();
    SectionPanel& getSectionPanel();
    HistoryPanel& getHistoryPanel();

    std::function<void()> onLayoutAnimationNeeded;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    
private:
    juce::ValueTree appState;
    juce::ValueTree selectionState;

    HeaderPanel headerPanel;
    SectionPanel sectionPanel;
    HistoryPanel historyPanel;
    
    float headerFlex;
    float sectionFlex;
    
    std::unique_ptr<DiatonyAlertWindowWithOverlay> activePopup;
    
    void showPopup(DiatonyAlertWindow::AlertType type,
                   const juce::String& title,
                   const juce::String& message,
                   const juce::String& buttonText = "OK");
    void closePopup();
    
    class DragOverlay : public juce::Component
    {
    public:
        DragOverlay();
        void paint(juce::Graphics& g) override;
    private:
        juce::SharedResourcePointer<FontManager> fontManager;
    };
    
    DragOverlay dragOverlay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
