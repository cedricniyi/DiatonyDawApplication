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
}

MainContentComponent::~MainContentComponent()
{
    if (appState.isValid())
        appState.removeListener(this);
}

void MainContentComponent::setAppState(juce::ValueTree& state)
{
    // Déconnexion de l'ancien listener
    if (appState.isValid())
        appState.removeListener(this);
        
    // Connexion au nouveau ValueTree
    appState = state;
    appState.addListener(this);
    
    // Propagation de l'état aux panels enfants
    headerPanel.setAppState(appState);
    footerPanel.setAppState(appState);
    // sectionPanel.setAppState(appState);  // Si nécessaire plus tard
}

void MainContentComponent::paint(juce::Graphics& g)
{   
    // Dégradé linéaire à 135 degrés
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

    // Layout vertical avec FlexBox
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
    // Notification du RootAnimator pour les changements de layout
    if (property == UIStateIdentifiers::interactivePianoVisible)
    {
        if (onLayoutAnimationNeeded)
        {
            onLayoutAnimationNeeded();
        }
    }
}

void MainContentComponent::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) {}
void MainContentComponent::valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) {}
void MainContentComponent::valueTreeChildOrderChanged(juce::ValueTree&, int, int) {}
void MainContentComponent::valueTreeParentChanged(juce::ValueTree&) {} 