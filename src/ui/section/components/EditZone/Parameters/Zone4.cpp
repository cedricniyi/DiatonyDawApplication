#include "Zone4.h"

Zone4::Zone4()
    : ColoredPanel(juce::Colours::lightsteelblue.withAlpha(0.5f)),
      addButton("+ Ajouter", 
                juce::Colours::blue,
                juce::Colours::darkblue, 
                14.0f, 
                FontManager::FontWeight::Medium)
{
    // Configuration du titre
    titleLabel.setText("Editeur de section", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    
    // Application de la police via FontManager
    auto titleFont = juce::Font(fontManager->getSFProDisplay(16.0f, FontManager::FontWeight::Bold));
    titleLabel.setFont(titleFont);
    addAndMakeVisible(titleLabel);
    
    // Configuration du callback du bouton d'ajout
    addButton.onClick = [this]() {
        contentAreaComponent.addRectangle();
    };
    addAndMakeVisible(addButton);
    
    // Configuration de la zone de contenu scrollable
    addAndMakeVisible(contentAreaComponent);
    
    // Configuration du Grid
    setupGrid();
}

Zone4::~Zone4()
{
}

void Zone4::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
    
    // // Dessiner le fond du titre (un peu plus clair)
    // g.setColour(titleBackgroundColour);
    // g.fillRect(titleArea);
    
    // // Dessiner le fond du contenu (plus foncé)
    // g.setColour(contentBackgroundColour);
    // g.fillRect(contentArea);
    
    // // Dessiner la bordure globale
    // g.setColour(borderColour);
    // g.drawRect(getLocalBounds(), 1);
    
    // // Dessiner une ligne de séparation entre titre et contenu
    // g.setColour(borderColour.withAlpha(0.3f));
    // g.drawHorizontalLine(titleArea.getBottom(), static_cast<float>(titleArea.getX()), static_cast<float>(titleArea.getRight()));
    
    // Le titre est maintenant géré par le titleLabel, plus besoin de le dessiner ici
}

void Zone4::resized()
{
    // Appeler la méthode resized de ColoredPanel
    ColoredPanel::resized();
    
    // Zone principale avec padding interne
    auto mainBounds = getLocalBounds().reduced(INTERNAL_PADDING);
    
    if (mainBounds.isEmpty())
        return;
    
    // Layout avec Grid (2 rangées : titre + contenu)
    mainGrid.performLayout(mainBounds);
}

void Zone4::setupGrid()
{
    // Configuration du Grid : 2 rangées (titre + contenu), 2 colonnes (label + bouton)
    // 30% titre = 3 parts, 70% contenu = 7 parts (proportions entières)
    mainGrid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(3)), 
                              juce::Grid::TrackInfo(juce::Grid::Fr(7)) };
    
    mainGrid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)), 
                                 juce::Grid::TrackInfo(juce::Grid::Px(100)) };
    
    mainGrid.setGap(juce::Grid::Px(8));
    mainGrid.alignItems = juce::Grid::AlignItems::center;
    
    // Ajouter les éléments au Grid
    mainGrid.items.clear();
    
    // Rangée 1, Colonne 1 : Title Label
    mainGrid.items.add(juce::GridItem(titleLabel)
        .withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)));
    
    // Rangée 1, Colonne 2 : Add Button  
    mainGrid.items.add(juce::GridItem(addButton)
        .withArea(juce::GridItem::Span(1), juce::GridItem::Span(1)));
    
    // Rangée 2, Colonnes 1-2 : Content Area (span sur 2 colonnes)
    mainGrid.items.add(juce::GridItem(contentAreaComponent)
        .withArea(juce::GridItem::Span(1), juce::GridItem::Span(2)));
}