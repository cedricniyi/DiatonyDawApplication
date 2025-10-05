#include "InfoColoredPanel.h"

//==============================================================================
InfoColoredPanel::InfoColoredPanel(juce::Colour color)
    : ColoredPanel(color)
{
    setupZones(color);
    setupGrid();
}

InfoColoredPanel::~InfoColoredPanel()
{
}

void InfoColoredPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Dessiner le fond avec la couleur du panel (comme ColoredPanel)
    juce::Path panelPath;
    panelPath.addRoundedRectangle(bounds, 10.0f);
    g.setColour(getColor());
    g.fillPath(panelPath);
    
    // Dessiner les lignes de séparation seulement si la partie droite est visible
    if (rightSideVisible)
    {
        // Appliquer le clipping arrondi pour les lignes de séparation
        g.saveState();
        g.reduceClipRegion(panelPath);
        
        // Dessiner les lignes de séparation
        g.setColour(getColor().contrasting(0.15f)); // Couleur légèrement contrastée
        
        // Ligne verticale au milieu (séparation gauche/droite)
        float midX = bounds.getWidth() * 0.5f;
        g.drawLine(midX, 0, midX, bounds.getHeight(), 1.0f);
        
        // Ligne horizontale au milieu droit (séparation top-right/bottom-right)
        float midY = bounds.getHeight() * 0.5f;
        g.drawLine(midX, midY, bounds.getWidth(), midY, 1.0f);
        
        g.restoreState();
    }
}

void InfoColoredPanel::resized()
{
    // Utilisation du système Grid pour positionner les zones
    grid.performLayout(getLocalBounds());
    
    // Positionner les composants à l'intérieur de chaque zone
    if (leftZone && topRightZone && bottomRightZone)
    {
        // ComboBox dans la zone gauche avec padding
        auto leftBounds = leftZone->getLocalBounds();
        auto comboHeight = 20;
        auto leftComboY = (leftBounds.getHeight() - comboHeight) / 2;
        leftCombo.setBounds(leftBounds.withTrimmedLeft(5)
                                      .withTrimmedRight(5)
                                      .withTop(leftComboY)
                                      .withHeight(comboHeight));
        
        // ComboBox dans la zone top-right avec padding
        auto topRightBounds = topRightZone->getLocalBounds();
        auto topComboY = (topRightBounds.getHeight() - comboHeight) / 2;
        topRightCombo.setBounds(topRightBounds.withTrimmedLeft(5)
                                              .withTrimmedRight(5)
                                              .withTop(topComboY)
                                              .withHeight(comboHeight));
        
        // ComboBox dans la zone bottom-right avec padding
        auto bottomRightBounds = bottomRightZone->getLocalBounds();
        auto bottomComboY = (bottomRightBounds.getHeight() - comboHeight) / 2;
        bottomRightCombo.setBounds(bottomRightBounds.withTrimmedLeft(5)
                                                    .withTrimmedRight(5)
                                                    .withTop(bottomComboY)
                                                    .withHeight(comboHeight));
    }
}

void InfoColoredPanel::setupGrid()
{
    // Configuration initiale du grid
    reconfigureGrid();
}

void InfoColoredPanel::reconfigureGrid()
{
    // Nettoyer les items existants
    grid.items.clear();
    
    if (rightSideVisible)
    {
        // Configuration normale : 2 colonnes, 2 lignes
        // Colonne 1: 50% (zone gauche)
        // Colonne 2: 50% (zones droites)
        // Ligne 1: 50% (zone top-right)
        // Ligne 2: 50% (zone bottom-right)
        
        grid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)), 
                                 juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)), 
                              juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        // Espacement entre les zones pour créer la séparation
        grid.setGap(juce::Grid::Px(2));
        
        // Zone gauche : occupe les 2 lignes de la première colonne
        grid.items.add(juce::GridItem(leftZone.get()).withArea(1, 1, 3, 2));
        
        // Zone top-right : première ligne, deuxième colonne
        grid.items.add(juce::GridItem(topRightZone.get()).withArea(1, 2, 2, 3));
        
        // Zone bottom-right : deuxième ligne, deuxième colonne
        grid.items.add(juce::GridItem(bottomRightZone.get()).withArea(2, 2, 3, 3));
    }
    else
    {
        // Configuration avec partie droite masquée : 1 colonne, 1 ligne
        // La zone gauche occupe tout l'espace
        
        grid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        // Pas d'espacement car une seule zone
        grid.setGap(juce::Grid::Px(0));
        
        // Zone gauche : occupe toute la grille
        grid.items.add(juce::GridItem(leftZone.get()).withArea(1, 1, 2, 2));
    }
}

void InfoColoredPanel::setColor(juce::Colour color)
{
    // Appeler la méthode parent pour maintenir la cohérence
    ColoredPanel::setColor(color);
    
    // Toutes les zones ont la même couleur maintenant
    leftZoneColor = color;
    topRightZoneColor = color;
    bottomRightZoneColor = color;
    
    // Adapter tous les ComboBox à la nouvelle couleur
    leftCombo.adaptToBackgroundColour(color);
    topRightCombo.adaptToBackgroundColour(color);
    bottomRightCombo.adaptToBackgroundColour(color);
    
    // Repaint pour mettre à jour les lignes de séparation
    repaint();
}

void InfoColoredPanel::setupZones(juce::Colour baseColor)
{
    // Toutes les zones ont la même couleur
    leftZoneColor = baseColor;
    topRightZoneColor = baseColor;
    bottomRightZoneColor = baseColor;
    
    // Création des zones simples sans couleur propre (transparentes)
    leftZone = std::make_unique<juce::Component>();
    topRightZone = std::make_unique<juce::Component>();
    bottomRightZone = std::make_unique<juce::Component>();
    
    // Les zones seront simplement des conteneurs, la couleur est gérée par le parent
    leftZone->setOpaque(false);
    topRightZone->setOpaque(false);
    bottomRightZone->setOpaque(false);
    
    // Configuration du ComboBox pour la zone gauche
    leftCombo.addItem("Accord 1", 1);
    leftCombo.addItem("Accord 2", 2);
    leftCombo.addItem("Accord 3", 3);
    leftCombo.addItem("Accord 4", 4);
    leftCombo.setSelectedId(1);
    leftCombo.adaptToBackgroundColour(baseColor);
    leftZone->addAndMakeVisible(leftCombo);
    
    // Configuration des combo boxes avec style personnalisé
    topRightCombo.addItem("Option 1", 1);
    topRightCombo.addItem("Option 2", 2);
    topRightCombo.addItem("Option 3", 3);
    topRightCombo.setSelectedId(1);
    topRightCombo.adaptToBackgroundColour(baseColor);
    topRightZone->addAndMakeVisible(topRightCombo);
    
    bottomRightCombo.addItem("Choix A", 1);
    bottomRightCombo.addItem("Choix B", 2);
    bottomRightCombo.addItem("Choix C", 3);
    bottomRightCombo.setSelectedId(1);
    bottomRightCombo.adaptToBackgroundColour(baseColor);
    bottomRightZone->addAndMakeVisible(bottomRightCombo);
    
    // Ajout des zones comme composants enfants
    addAndMakeVisible(leftZone.get());
    addAndMakeVisible(topRightZone.get());
    addAndMakeVisible(bottomRightZone.get());
}

void InfoColoredPanel::showRightSide(bool show)
{
    if (rightSideVisible != show)
    {
        rightSideVisible = show;
        
        // Contrôler la visibilité des zones de droite
        topRightZone->setVisible(show);
        bottomRightZone->setVisible(show);
        
        // Reconfigurer le grid avec la nouvelle disposition
        reconfigureGrid();
        
        // Forcer le repositionnement
        resized();
        
        // Repaint pour mettre à jour les lignes de séparation
        repaint();
    }
}