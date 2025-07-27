#include "MainContentComponent.h"
#include "header/HeaderPanel.h"
#include "section/SectionPanel.h"
#include "footer/FooterPanel.h"
#include "UIStateIdentifiers.h"

//==============================================================================
MainContentComponent::MainContentComponent() 
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

MainContentComponent::~MainContentComponent()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void MainContentComponent::setAppState(juce::ValueTree& state)
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

void MainContentComponent::paint(juce::Graphics& g)
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

void MainContentComponent::resized()
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

float& MainContentComponent::getHeaderFlexRef()
{
    return headerFlex;
}

float& MainContentComponent::getSectionFlexRef()
{
    return sectionFlex;
}

float& MainContentComponent::getFooterFlexRef()
{
    return footerFlex;
}

FooterPanel& MainContentComponent::getFooterPanel()
{
    return footerPanel;
}

HeaderPanel& MainContentComponent::getHeaderPanel()
{
    return headerPanel;
}

SectionPanel& MainContentComponent::getSectionPanel()
{
    return sectionPanel;
}

void MainContentComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                   const juce::Identifier& property)
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

void MainContentComponent::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void MainContentComponent::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void MainContentComponent::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void MainContentComponent::valueTreeParentChanged(juce::ValueTree&) {} 