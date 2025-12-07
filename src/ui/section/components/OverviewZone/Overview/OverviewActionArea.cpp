#include "OverviewActionArea.h"
#include "utils/FontManager.h"

//==============================================================================
OverviewActionArea::OverviewActionArea() 
    : actionButton("+", 
                   juce::Colours::grey,
                   juce::Colours::darkgrey, 
                   18.0f, 
                   FontManager::FontWeight::Bold)
{
    setOpaque(false);
    
    // Ajouter les composants enfants
    addAndMakeVisible(contentArea);
    addAndMakeVisible(actionButton);
    
    // Configurer le callback du bouton pour ajouter une section
    actionButton.onClick = [this]() {
        contentArea.addSmallPanel();
    };
}

void OverviewActionArea::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Fond semi-transparent style BaseZone
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Contour léger
    g.setColour(juce::Colours::grey.withAlpha(0.4f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, static_cast<float>(borderThickness));
}

void OverviewActionArea::resized()
{
    auto bounds = getLocalBounds();
    
    // Padding interne
    auto area = bounds.reduced(contentPadding + 2, contentPadding);
    
    // Bouton "+" à droite avec largeur fixe
    constexpr int buttonWidth = 50;
    constexpr int spacing = 8;
    
    auto buttonArea = area.removeFromRight(buttonWidth);
    actionButton.setBounds(buttonArea);
    
    // ContentArea prend le reste de l'espace
    area.removeFromRight(spacing);
    contentArea.setBounds(area);
}

OverviewContentArea& OverviewActionArea::getContentArea()
{
    return contentArea;
}
