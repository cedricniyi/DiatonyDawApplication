#include "InfoColoredPanel.h"

//==============================================================================
InfoColoredPanel::InfoColoredPanel(juce::Colour color)
    : ColoredPanel(color), panelColor(color)
{
    setupZones(color);
    setupMainGrid();
    setupBottomGrid();
    setupNumberLabel();
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
    
    // Dessiner les lignes de séparation seulement si la partie basse est visible
    if (bottomZoneVisible && topZone && bottomZone)
    {
        // Appliquer le clipping arrondi pour les lignes de séparation
        g.saveState();
        g.reduceClipRegion(panelPath);
        
        // Dessiner les lignes de séparation
        g.setColour(getColor().contrasting(0.15f)); // Couleur légèrement contrastée
        
        // Ligne horizontale séparant haut et bas
        float separatorY = static_cast<float>(topZone->getBottom());
        g.drawLine(0.0f, separatorY, bounds.getWidth(), separatorY, 1.0f);
        
        // Ligne verticale au milieu de la zone basse (séparation état/qualité)
        if (bottomLeftZone && bottomRightZone)
        {
            // Position à 60% de la largeur (3/5 = 0.6)
            float separatorX = bounds.getWidth() * 0.6f;
            float startY = separatorY;
            g.drawLine(separatorX, startY, separatorX, bounds.getHeight(), 1.0f);
        }
        
        g.restoreState();
    }
    
    // Dessiner l'arrière-plan du numéro si visible
    if (panelNumber > 0 && numberLabel.isVisible())
    {
        auto numberBounds = numberLabel.getBounds().toFloat();
        juce::Path numberPath;
        numberPath.addRoundedRectangle(numberBounds.reduced(1.0f), 6.0f);
        
        // Fond semi-transparent contrastant
        g.setColour(getColor().contrasting(0.3f).withAlpha(0.8f));
        g.fillPath(numberPath);
    }
}

void InfoColoredPanel::resized()
{
    // Utilisation du système Grid principal pour positionner les zones haut/bas
    mainGrid.performLayout(getLocalBounds());
    
    // Utilisation du Grid pour la zone basse (2 ComboBox)
    if (bottomZone && bottomZoneVisible)
    {
        bottomGrid.performLayout(bottomZone->getLocalBounds());
    }
    
    // Positionner les composants à l'intérieur de chaque zone
    if (topZone)
    {
        // ComboBox degré dans la zone haute avec padding
        auto topBounds = topZone->getLocalBounds();
        auto comboHeight = 20;
        auto topComboY = (topBounds.getHeight() - comboHeight) / 2;
        
        // Si la partie basse est masquée et qu'il y a une numérotation, laisser plus d'espace à gauche
        int leftPadding = 5;
        if (!bottomZoneVisible && panelNumber > 0)
        {
            leftPadding = 32; // Espace pour le numéro (24px) + marge (8px)
        }
        else if (panelNumber > 0)
        {
            leftPadding = 32; // Toujours laisser de l'espace pour la numérotation
        }
        
        degreeCombo.setBounds(topBounds.withTrimmedLeft(leftPadding)
                                      .withTrimmedRight(5)
                                      .withTop(topComboY)
                                      .withHeight(comboHeight));
    }
    
    if (bottomLeftZone && bottomRightZone && bottomZoneVisible)
    {
        auto comboHeight = 20;
        
        // ComboBox état dans la zone basse gauche
        auto bottomLeftBounds = bottomLeftZone->getLocalBounds();
        auto leftComboY = (bottomLeftBounds.getHeight() - comboHeight) / 2;
        stateCombo.setBounds(bottomLeftBounds.withTrimmedLeft(5)
                                            .withTrimmedRight(5)
                                            .withTop(leftComboY)
                                            .withHeight(comboHeight));
        
        // ComboBox qualité dans la zone basse droite
        auto bottomRightBounds = bottomRightZone->getLocalBounds();
        auto rightComboY = (bottomRightBounds.getHeight() - comboHeight) / 2;
        qualityCombo.setBounds(bottomRightBounds.withTrimmedLeft(5)
                                               .withTrimmedRight(5)
                                               .withTop(rightComboY)
                                               .withHeight(comboHeight));
    }
    
    // Positionner le label de numérotation dans le coin supérieur gauche
    // Centré verticalement dans la zone haute
    if (panelNumber > 0 && topZone)
    {
        int numberSize = 24;
        int yPos = topZone->getY() + (topZone->getHeight() - numberSize) / 2;
        
        numberLabel.setBounds(6, yPos, numberSize, numberSize);
    }
}

void InfoColoredPanel::setupMainGrid()
{
    // Configuration initiale du grid principal
    reconfigureMainGrid();
}

void InfoColoredPanel::setupBottomGrid()
{
    // Configuration du grid pour la zone basse (2 ComboBox côte à côte)
    // Division 60% / 40% pour un meilleur équilibre visuel
    bottomGrid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(3)), 
                                   juce::Grid::TrackInfo(juce::Grid::Fr(2)) };
    
    bottomGrid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
    
    // Espacement entre les 2 ComboBox
    bottomGrid.setGap(juce::Grid::Px(2));
    
    // Zone basse gauche (état) - 60%
    bottomGrid.items.add(juce::GridItem(bottomLeftZone.get()).withArea(1, 1));
    
    // Zone basse droite (qualité) - 40%
    bottomGrid.items.add(juce::GridItem(bottomRightZone.get()).withArea(1, 2));
}

void InfoColoredPanel::reconfigureMainGrid()
{
    // Nettoyer les items existants
    mainGrid.items.clear();
    
    if (bottomZoneVisible)
    {
        // Configuration normale : 2 rangées (haut + bas)
        // Division 50% / 50% en hauteur pour un équilibre visuel
        
        mainGrid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)), 
                                  juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        mainGrid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        // Espacement entre les zones pour créer la séparation
        mainGrid.setGap(juce::Grid::Px(2));
        
        // Zone haute : première rangée - 50%
        mainGrid.items.add(juce::GridItem(topZone.get()).withArea(1, 1));
        
        // Zone basse : deuxième rangée - 50%
        mainGrid.items.add(juce::GridItem(bottomZone.get()).withArea(2, 1));
    }
    else
    {
        // Configuration avec partie basse masquée : 1 rangée
        // La zone haute occupe tout l'espace
        
        mainGrid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        mainGrid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
        
        // Pas d'espacement car une seule zone
        mainGrid.setGap(juce::Grid::Px(0));
        
        // Zone haute : occupe toute la grille
        mainGrid.items.add(juce::GridItem(topZone.get()).withArea(1, 1));
    }
}

void InfoColoredPanel::setColor(juce::Colour color)
{
    // Appeler la méthode parent pour maintenir la cohérence
    ColoredPanel::setColor(color);
    
    // Sauvegarder la couleur
    panelColor = color;
    
    // Adapter tous les ComboBox à la nouvelle couleur
    degreeCombo.adaptToBackgroundColour(color);
    stateCombo.adaptToBackgroundColour(color);
    qualityCombo.adaptToBackgroundColour(color);
    
    // Repaint pour mettre à jour les lignes de séparation
    repaint();
}

void InfoColoredPanel::setupZones(juce::Colour baseColor)
{
    // Création des zones principales
    topZone = std::make_unique<juce::Component>();
    bottomZone = std::make_unique<juce::Component>();
    
    // Création des sous-zones pour la partie basse
    bottomLeftZone = std::make_unique<juce::Component>();
    bottomRightZone = std::make_unique<juce::Component>();
    
    // Les zones seront simplement des conteneurs, la couleur est gérée par le parent
    topZone->setOpaque(false);
    bottomZone->setOpaque(false);
    bottomLeftZone->setOpaque(false);
    bottomRightZone->setOpaque(false);
    
    // Configuration des ComboBox (vides au départ, à peupler depuis l'extérieur)
    degreeCombo.adaptToBackgroundColour(baseColor);
    topZone->addAndMakeVisible(degreeCombo);
    
    stateCombo.adaptToBackgroundColour(baseColor);
    bottomLeftZone->addAndMakeVisible(stateCombo);
    
    qualityCombo.adaptToBackgroundColour(baseColor);
    bottomRightZone->addAndMakeVisible(qualityCombo);
    
    // Ajout des sous-zones à la zone basse
    bottomZone->addAndMakeVisible(bottomLeftZone.get());
    bottomZone->addAndMakeVisible(bottomRightZone.get());
    
    // Ajout des zones principales comme composants enfants
    addAndMakeVisible(topZone.get());
    addAndMakeVisible(bottomZone.get());
}

void InfoColoredPanel::populateDegreeCombo(const juce::StringArray& items)
{
    degreeCombo.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        degreeCombo.addItem(items[i], i + 1);
    }
    if (items.size() > 0)
        degreeCombo.setSelectedId(1);
}

void InfoColoredPanel::populateStateCombo(const juce::StringArray& items)
{
    stateCombo.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        stateCombo.addItem(items[i], i + 1);
    }
    if (items.size() > 0)
        stateCombo.setSelectedId(1);
}

void InfoColoredPanel::populateQualityCombo(const juce::StringArray& items)
{
    qualityCombo.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        qualityCombo.addItem(items[i], i + 1);
    }
    if (items.size() > 0)
        qualityCombo.setSelectedId(1);
}

void InfoColoredPanel::showBottomZone(bool show)
{
    if (bottomZoneVisible != show)
    {
        bottomZoneVisible = show;
        
        // Contrôler la visibilité de la zone basse
        bottomZone->setVisible(show);
        
        // Reconfigurer le grid avec la nouvelle disposition
        reconfigureMainGrid();
        
        // Forcer le repositionnement
        resized();
        
        // Repaint pour mettre à jour les lignes de séparation
        repaint();
    }
}

void InfoColoredPanel::setupNumberLabel()
{
    // Configuration du label de numérotation
    numberLabel.setJustificationType(juce::Justification::centred);
    
    // Application de la police via FontManager
    auto numberFont = juce::Font(fontManager->getSFProDisplay(12.0f, FontManager::FontWeight::Bold));
    numberLabel.setFont(numberFont);
    
    numberLabel.setVisible(false); // Invisible par défaut jusqu'à ce qu'un numéro soit défini
    addAndMakeVisible(numberLabel);
}

void InfoColoredPanel::setNumber(int number)
{
    panelNumber = number;
    
    if (number > 0)
    {
        numberLabel.setText(juce::String(number), juce::dontSendNotification);
        numberLabel.setVisible(true);
        
        // Adapter la couleur du texte en fonction du fond
        auto bgColor = getColor();
        auto textColor = bgColor.contrasting(0.8f);
        numberLabel.setColour(juce::Label::textColourId, textColor);
    }
    else
    {
        numberLabel.setVisible(false);
    }
    
    resized();
    repaint();
}