#include "OverviewActionArea.h"
#include "utils/FontManager.h"

//==============================================================================
OverviewActionArea::OverviewActionArea() 
    : ColoredPanel(juce::Colour::fromString("#fffcfcff")),
      actionButton("+", juce::Colour::fromString("#FF4747b5"), 
                   juce::Colour::fromString("#FF4747b5"), 2.5f, 
                   16.0f, FontManager::FontWeight::Bold)
{
    // Fond légèrement transparent
    setAlpha(0.85f); 
    
    // Ajouter directement le ContentArea et le bouton (structure aplatie)
    addAndMakeVisible(contentArea);
    addAndMakeVisible(actionButton);
    
    // Connecter le bouton à la création de nouveaux panels
    actionButton.onClick = [this]() {
        contentArea.addSmallPanel();
    };
}

void OverviewActionArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void OverviewActionArea::resized()
{
    auto area = getLocalBounds().reduced(8, 6);
    
    // Bouton à droite avec la même hauteur que le contentArea
    constexpr int BUTTON_WIDTH = 50;
    constexpr int SPACING = 8;
    
    auto buttonArea = area.removeFromRight(BUTTON_WIDTH);
    area.removeFromRight(SPACING);
    
    // ContentArea prend tout l'espace restant
    contentArea.setBounds(area);
    
    // Bouton avec la même hauteur
    actionButton.setBounds(buttonArea);
}

OverviewContentArea& OverviewActionArea::getContentArea()
{
    return contentArea;
}
