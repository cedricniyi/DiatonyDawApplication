#pragma once

#include <JuceHeader.h>
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"

//==============================================================================
/**
 * @brief Composant principal qui contient le layout de l'interface
 * 
 * MainContentComponent gère le layout des 3 panels principaux (Header, Section, Footer)
 * et utilise le même pattern de callback que FooterPanel pour notifier les changements
 * de layout nécessaires à son RootAnimator.
 */
class MainContentComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === Accesseurs pour que RootAnimator puisse accéder aux valeurs flex ===
    
    float& getHeaderFlexRef();
    float& getSectionFlexRef();
    float& getFooterFlexRef();
    
    FooterPanel& getFooterPanel();
    HeaderPanel& getHeaderPanel();
    SectionPanel& getSectionPanel();

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
    
private:
    juce::ValueTree appState;

    // 3 Panels principaux
    HeaderPanel headerPanel;
    SectionPanel sectionPanel;
    FooterPanel footerPanel;
    
    // Valeurs flex pour le layout
    float headerFlex;
    float sectionFlex;
    float footerFlex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
}; 