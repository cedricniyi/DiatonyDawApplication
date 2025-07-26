#include "ProgressionArea.h"
#include "utils/FontManager.h"

//==============================================================================
ProgressionArea::ProgressionArea() 
    : ColoredPanel(juce::Colours::white)
{
    // Ajouter la vue de bienvenue
    addAndMakeVisible(welcomeView);
}

void ProgressionArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void ProgressionArea::resized()
{
    // Appliquer le padding à toute la zone
    auto area = getLocalBounds().reduced(20, 10);
    
    // Positionner la vue de bienvenue dans toute la zone disponible
    welcomeView.setBounds(area);
} 