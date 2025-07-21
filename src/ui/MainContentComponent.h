#pragma once

#include <JuceHeader.h>
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "UIStateIdentifiers.h"

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
    MainContentComponent() 
        : appState(),
          headerPanel(),
          sectionPanel(),
          footerPanel(),
          headerFlex(7.5f),
          sectionFlex(57.5f),
          footerFlex(15.0f)
    {        
        addAndMakeVisible(headerPanel);
        addAndMakeVisible(sectionPanel);
        addAndMakeVisible(footerPanel);
        
        DBG("MainContentComponent: Composant principal initialisé");
    }
    
    ~MainContentComponent() override
    {
        if (appState.isValid())
            appState.removeListener(this);
    }

    /** Initialise le ValueTree et commence l'écoute des changements */
    void setAppState(juce::ValueTree& state)
    {
        if (appState.isValid())
            appState.removeListener(this);
            
        appState = state;
        appState.addListener(this);
        
        // Connecter les panels auto-gérés au ValueTree
        headerPanel.setAppState(appState);
        footerPanel.setAppState(appState);
        // sectionPanel.setAppState(appState);  // Si nécessaire plus tard
        
        DBG("MainContentComponent: ValueTree connecté et panels configurés");
    }
    
    void paint(juce::Graphics& g) override
    {   
        // Dégradé linéaire à 135 degrés (même que dans l'ancien PluginEditor)
        auto bounds = getLocalBounds().toFloat();
        
        auto center = bounds.getCentre();
        auto diagonal = std::sqrt(bounds.getWidth() * bounds.getWidth() + bounds.getHeight() * bounds.getHeight()) * 0.5f;
        
        auto angleRad = juce::MathConstants<float>::pi * 135.0f / 180.0f;
        auto startPoint = center - juce::Point<float>(std::cos(angleRad), std::sin(angleRad)) * diagonal;
        auto endPoint = center + juce::Point<float>(std::cos(angleRad), std::sin(angleRad)) * diagonal;
        
        juce::ColourGradient gradient(
            juce::Colour::fromString("fff5f7fa"), startPoint,
            juce::Colour::fromString("ffc3cfe2"), endPoint,
            false
        );
        
        g.setGradientFill(gradient);
        g.fillRect(bounds);
    }

    void resized() override
    {
        int padding = 8;
        auto content = getLocalBounds().reduced(padding);

        juce::FlexBox fb;
        fb.flexDirection = juce::FlexBox::Direction::column;

        fb.items = {
            juce::FlexItem(headerPanel).withFlex(headerFlex).withMargin({ 0, 0, 4, 0 }),
            juce::FlexItem(sectionPanel).withFlex(sectionFlex).withMargin({ 4, 0, 4, 0 }),
            juce::FlexItem(footerPanel).withFlex(footerFlex).withMargin({ 4, 0, 0, 0 })
        };

        fb.performLayout(content);
    }
    
    // === Accesseurs pour que RootAnimator puisse accéder aux valeurs flex ===
    
    float& getHeaderFlexRef() { return headerFlex; }
    float& getSectionFlexRef() { return sectionFlex; }
    float& getFooterFlexRef() { return footerFlex; }
    
    FooterPanel& getFooterPanel() { return footerPanel; }
    HeaderPanel& getHeaderPanel() { return headerPanel; }
    SectionPanel& getSectionPanel() { return sectionPanel; }

    // === Callbacks pour que RootAnimator puisse s'y abonner (même pattern que FooterPanel) ===
    
    /** Callback déclenché quand une animation de layout est nécessaire */
    std::function<void()> onLayoutAnimationNeeded;

    // =================================================================================
    // ValueTree::Listener interface
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        // MainContentComponent détecte les changements et notifie RootAnimator via callback
        if (property == UIStateIdentifiers::interactivePianoVisible)
        {
            // ✅ Notifier via callback si RootAnimator s'est abonné (même pattern que FooterPanel)
            if (onLayoutAnimationNeeded)
            {
                onLayoutAnimationNeeded();
                DBG("MainContentComponent: Notifie RootAnimator via callback pour propriété: " << property.toString());
            }
        }
    }

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    
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