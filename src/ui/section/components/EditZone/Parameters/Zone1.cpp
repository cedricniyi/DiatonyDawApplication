#include "Zone1.h"

Zone1::Zone1()
{
}

Zone1::~Zone1()
{
}

void Zone1::paint(juce::Graphics& g)
{
    // Dessiner le fond du titre
    g.setColour(titleBackgroundColour);
    g.fillRect(titleArea);
    
    // Dessiner le fond du contenu (plus foncé)
    g.setColour(contentBackgroundColour);
    g.fillRect(contentArea);
    
    // Dessiner la bordure globale
    g.setColour(borderColour);
    g.drawRect(getLocalBounds(), 1);
    
    // Dessiner une ligne de séparation entre titre et contenu
    g.setColour(borderColour.withAlpha(0.3f));
    g.drawHorizontalLine(titleArea.getBottom(), static_cast<float>(titleArea.getX()), static_cast<float>(titleArea.getRight()));
    
    // Dessiner le titre avec FontManager
    g.setColour(juce::Colours::black);
    auto titleFont = juce::Font(fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Bold));
    g.setFont(titleFont);
    g.drawText("Zone 1", titleArea, juce::Justification::centred);
}

void Zone1::resized()
{
    auto bounds = getLocalBounds();
    
    // Diviser en 2 parties : 30% titre, 70% contenu
    int titleHeight = static_cast<int>(bounds.getHeight() * 0.3f);
    
    titleArea = bounds.removeFromTop(titleHeight);
    contentArea = bounds; // Le reste pour le contenu
    
    // Layout logic for Zone1 content components will go here
}