#pragma once

#include <JuceHeader.h>
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "history/HistoryPanel.h"
#include "extra/Component/DiatonyAlertWindow.h"

//==============================================================================
/**
 * @brief Composant principal qui contient le layout de l'interface
 * 
 * MainContentComponent gère le layout des 3 panels principaux (Header, Section, Footer)
 * et utilise le même pattern de callback que FooterPanel pour notifier les changements
 * de layout nécessaires à son RootAnimator.
 * 
 * Supporte le Drag & Drop de fichiers .diatony pour charger des solutions sauvegardées.
 */
class MainContentComponent : public juce::Component,
                             public juce::ValueTree::Listener,
                             public juce::FileDragAndDropTarget,
                             public juce::DragAndDropTarget
{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);
    
    /** Initialise l'écoute du selectionState pour les notifications de génération */
    void setSelectionState(juce::ValueTree& state);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === Accesseurs pour que RootAnimator puisse accéder aux valeurs flex ===
    
    float& getHeaderFlexRef();
    float& getSectionFlexRef();
    float& getFooterFlexRef();
    
    FooterPanel& getFooterPanel();
    HeaderPanel& getHeaderPanel();
    SectionPanel& getSectionPanel();
    HistoryPanel& getHistoryPanel();

    // === Callbacks pour que RootAnimator puisse s'y abonner (même pattern que FooterPanel) ===
    
    /** Callback déclenché quand une animation de layout est nécessaire */
    std::function<void()> onLayoutAnimationNeeded;

    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override;
    void valueTreeParentChanged(juce::ValueTree&) override;
    
    // =================================================================================
    // FileDragAndDropTarget interface (drag de fichiers externes depuis l'OS)
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    // =================================================================================
    // DragAndDropTarget interface (drag interne depuis HistoryPanel)
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    
private:
    juce::ValueTree appState;
    juce::ValueTree selectionState;  // Pour écouter les changements de génération

    // 3 Panels principaux + History Panel latéral
    HeaderPanel headerPanel;
    SectionPanel sectionPanel;
    FooterPanel footerPanel;
    HistoryPanel historyPanel;
    
    // Valeurs flex pour le layout
    float headerFlex;
    float sectionFlex;
    float footerFlex;
    
    // Overlay pour afficher les popups directement dans MainContentComponent
    std::unique_ptr<DiatonyAlertWindowWithOverlay> activePopup;
    
    // Méthodes helper pour gérer les popups
    void showPopup(DiatonyAlertWindow::AlertType type,
                   const juce::String& title,
                   const juce::String& message,
                   const juce::String& buttonText = "OK");
    void closePopup();
    
    // =================================================================================
    // Drag & Drop Overlay
    /**
     * @brief Overlay semi-transparent affiché lors du survol avec un fichier .diatony
     */
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