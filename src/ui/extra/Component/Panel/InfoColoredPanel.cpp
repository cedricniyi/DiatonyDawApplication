#include "InfoColoredPanel.h"

InfoColoredPanel::InfoColoredPanel(juce::Colour color)
    : ColoredPanel(color)
{
    // Configuration des ComboBox
    degreeCombo.adaptToBackgroundColour(color);
    qualityCombo.adaptToBackgroundColour(color);
    stateCombo.adaptToBackgroundColour(color);
    
    // Cacher les flèches pour gagner de l'espace
    degreeCombo.setArrowVisible(false);
    qualityCombo.setArrowVisible(false);
    stateCombo.setArrowVisible(false);
    
    addAndMakeVisible(degreeCombo);
    addAndMakeVisible(qualityCombo);
    addAndMakeVisible(stateCombo);
    
    // Écouter les changements de degré pour mettre à jour la bande colorée (fonction tonale)
    degreeCombo.addListener(this);
    
    // Charger les icônes SVG pour le cadenas
    lockIcon = juce::Drawable::createFromImageData(IconData::lock1svgrepocom_svg, 
                                                    IconData::lock1svgrepocom_svgSize);
    unlockIcon = juce::Drawable::createFromImageData(IconData::unlocksvgrepocom_svg, 
                                                      IconData::unlocksvgrepocom_svgSize);
    
    setupLabels();
}

InfoColoredPanel::~InfoColoredPanel()
{
    degreeCombo.removeListener(this);
}

void InfoColoredPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    // Redessiner pour mettre à jour la bande colorée quand le degré change
    if (comboBoxThatHasChanged == &degreeCombo)
        repaint();
}

void InfoColoredPanel::paint(juce::Graphics& g)
{
    using namespace InfoPanelConfig;
    
    auto bounds = getLocalBounds().toFloat();
    
    // Fond arrondi
    juce::Path panelPath;
    panelPath.addRoundedRectangle(bounds, CORNER_RADIUS);
    g.setColour(getColor());
    g.fillPath(panelPath);
    
    // Bande de codage couleur harmonique en haut
    auto stripArea = bounds.removeFromTop(STRIP_HEIGHT);
    
    g.saveState();
    g.reduceClipRegion(panelPath);  // Respecter les coins arrondis
    g.setColour(getFunctionalStripColor());
    g.fillRect(stripArea);
    g.restoreState();
    
    // Calculer la hauteur totale de la zone du haut (bande + padding + carrés)
    int totalTopHeight = static_cast<int>(STRIP_HEIGHT) + TOP_PADDING + TOP_ROW_HEIGHT;
    float comboZoneHeight = (getLocalBounds().toFloat().getHeight() - totalTopHeight) / 3.0f;
    
    // Lignes de séparation
        g.saveState();
        g.reduceClipRegion(panelPath);
    g.setColour(getColor().contrasting(0.15f));
    
    // Ligne sous les carrés
    g.drawLine(0.0f, totalTopHeight, getLocalBounds().toFloat().getWidth(), totalTopHeight, 1.0f);
    
    // Ligne entre degré et qualité
    g.drawLine(0.0f, totalTopHeight + comboZoneHeight, getLocalBounds().toFloat().getWidth(), totalTopHeight + comboZoneHeight, 1.0f);
    
    // Ligne entre qualité et état
    g.drawLine(0.0f, totalTopHeight + comboZoneHeight * 2.0f, getLocalBounds().toFloat().getWidth(), totalTopHeight + comboZoneHeight * 2.0f, 1.0f);
        
        g.restoreState();
    
    // Dessiner les 3 petits carrés (après la bande + padding)
    auto topArea = getLocalBounds()
        .withTrimmedTop(static_cast<int>(STRIP_HEIGHT) + TOP_PADDING)
        .removeFromTop(TOP_ROW_HEIGHT);
    drawTopSquares(g, topArea);
}

void InfoColoredPanel::drawTopSquares(juce::Graphics& g, const juce::Rectangle<int>& topArea)
{
    using namespace InfoPanelConfig;
    
    int totalWidth = SQUARE_SIZE * 3 + SQUARE_SPACING * 2;
    int startX = (topArea.getWidth() - totalWidth) / 2;
    int y = topArea.getY() + (topArea.getHeight() - SQUARE_SIZE) / 2;
    
    // Couleur des carrés
    auto squareColor = getColor().contrasting(0.25f);
    auto textColor = getColor().contrasting(0.8f);
    
    // Carré 1 : Numéro
    juce::Rectangle<int> square1(startX, y, SQUARE_SIZE, SQUARE_SIZE);
    g.setColour(squareColor);
    g.fillRoundedRectangle(square1.toFloat(), 4.0f);
    
    if (panelNumber > 0)
    {
        g.setColour(textColor);
        auto font = juce::Font(fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Bold));
        g.setFont(font);
        g.drawText(juce::String(panelNumber), square1, juce::Justification::centred);
    }
    
    // Carré 2 : Cadenas (cliquable)
    lockSquareArea = juce::Rectangle<int>(startX + SQUARE_SIZE + SQUARE_SPACING, y, SQUARE_SIZE, SQUARE_SIZE);
    // Fond jaunâtre si verrouillé, gris semi-transparent sinon
    if (locked)
        g.setColour(juce::Colour(0xFFD4A017).withAlpha(0.7f));  // Or/jaune
    else
        g.setColour(squareColor.withAlpha(0.5f));
    g.fillRoundedRectangle(lockSquareArea.toFloat(), 4.0f);
    drawLockIcon(g, lockSquareArea, locked);
    
    // Carré 3 : Suppression (long press)
    deleteSquareArea = juce::Rectangle<int>(startX + (SQUARE_SIZE + SQUARE_SPACING) * 2, y, SQUARE_SIZE, SQUARE_SIZE);
    g.setColour(squareColor.withAlpha(0.5f));
    g.fillRoundedRectangle(deleteSquareArea.toFloat(), 4.0f);
    drawDeleteIcon(g, deleteSquareArea);
}

void InfoColoredPanel::drawLockIcon(juce::Graphics& g, const juce::Rectangle<int>& area, bool isLocked)
{
    // Sélectionner l'icône appropriée
    auto* iconToDraw = isLocked ? lockIcon.get() : unlockIcon.get();
    
    if (iconToDraw == nullptr)
        return;
    
    // Créer une copie pour recolorier sans modifier l'original
    auto iconCopy = iconToDraw->createCopy();
    
    // Couleur de l'icône : même couleur que le texte existant
    auto iconColor = getColor().contrasting(0.8f);
    iconCopy->replaceColour(juce::Colours::black, iconColor);
    
    // Zone de dessin avec padding pour que l'icône ne touche pas les bords
    auto iconBounds = area.toFloat().reduced(3.0f);
    
    // Dessiner l'icône centrée dans la zone
    iconCopy->drawWithin(g, iconBounds, juce::RectanglePlacement::centred, 1.0f);
}

void InfoColoredPanel::drawDeleteIcon(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    auto areaFloat = area.toFloat();
    
    // Fond avec progression rouge
    if (deleteProgress > 0.0f)
    {
        // Interpoler du gris vers le rouge selon la progression
        auto progressColor = juce::Colours::darkgrey.interpolatedWith(
            juce::Colours::darkred, deleteProgress
        ).withAlpha(0.5f + 0.5f * deleteProgress);
        
        g.setColour(progressColor);
        g.fillRoundedRectangle(areaFloat, 4.0f);
    }
    
    // Croix (X)
    auto textColor = getColor().contrasting(0.8f);
    g.setColour(textColor);
    
    auto center = area.getCentre().toFloat();
    float crossSize = area.getWidth() * 0.25f;
    
    g.drawLine(center.x - crossSize, center.y - crossSize,
               center.x + crossSize, center.y + crossSize, 1.5f);
    g.drawLine(center.x + crossSize, center.y - crossSize,
               center.x - crossSize, center.y + crossSize, 1.5f);
}

void InfoColoredPanel::setLocked(bool newLocked)
{
    if (locked != newLocked)
    {
        locked = newLocked;
        repaint();
    }
}

void InfoColoredPanel::mouseDown(const juce::MouseEvent& event)
{
    // Long press sur le carré de suppression
    if (deleteSquareArea.contains(event.getPosition()))
    {
        isDeleteHeldDown = true;
        deleteProgress = 0.0f;
        deleteStartTime = juce::Time::getMillisecondCounter();
        startTimerHz(60);  // ~60 FPS pour animation fluide
        repaint();
    }
    
    ColoredPanel::mouseDown(event);
}

void InfoColoredPanel::mouseUp(const juce::MouseEvent& event)
{
    // Clic sur le cadenas
    if (lockSquareArea.contains(event.getPosition()))
    {
        locked = !locked;
        repaint();
        
        if (onLockToggled)
            onLockToggled(locked);
    }
    
    // Relâchement du bouton de suppression (annule si pas fini)
    if (isDeleteHeldDown)
    {
        isDeleteHeldDown = false;
        stopTimer();
        deleteProgress = 0.0f;
        repaint();
    }
    
    ColoredPanel::mouseUp(event);
}

void InfoColoredPanel::timerCallback()
{
    if (!isDeleteHeldDown)
    {
        stopTimer();
        deleteProgress = 0.0f;
        repaint();
        return;
    }
    
    auto now = juce::Time::getMillisecondCounter();
    auto elapsed = static_cast<int>(now - deleteStartTime);
    deleteProgress = juce::jlimit(0.0f, 1.0f, 
                                   static_cast<float>(elapsed) / static_cast<float>(LONG_PRESS_DURATION_MS));
    
    if (deleteProgress >= 1.0f)
    {
        // Long press complété !
        stopTimer();
        isDeleteHeldDown = false;
        deleteProgress = 0.0f;  // Reset avant destruction
        
        DBG("Long press completed - Delete requested!");
        
        // Différer la suppression pour permettre à timerCallback() de se terminer
        // avant que le composant ne soit détruit (évite use-after-free)
        if (onDeleteRequested)
        {
            auto callback = onDeleteRequested;  // Copier le std::function
            juce::MessageManager::callAsync([callback]() {
                if (callback)
                    callback();
            });
        }
        
        return;  // IMPORTANT: sortir immédiatement, ne plus toucher à this
    }
    
    repaint();
}

void InfoColoredPanel::resized()
{
    using namespace InfoPanelConfig;
    
    auto bounds = getLocalBounds();
    
    // Zone du haut : bande + padding + carrés
    int totalTopHeight = static_cast<int>(STRIP_HEIGHT) + TOP_PADDING + TOP_ROW_HEIGHT;
    bounds.removeFromTop(totalTopHeight);
    
    // 3 zones égales pour les ComboBox
    int comboZoneHeight = bounds.getHeight() / 3;
    
    // Zone 1 : Degré
    auto zone1 = bounds.removeFromTop(comboZoneHeight);
    int y1 = (zone1.getHeight() - COMBO_HEIGHT) / 2;
    degreeCombo.setBounds(zone1.withTrimmedLeft(HORIZONTAL_PADDING)
                              .withTrimmedRight(HORIZONTAL_PADDING)
                              .withTop(zone1.getY() + y1)
                              .withHeight(COMBO_HEIGHT));
    
    // Zone 2 : Qualité
    auto zone2 = bounds.removeFromTop(comboZoneHeight);
    int y2 = (zone2.getHeight() - COMBO_HEIGHT) / 2;
    qualityCombo.setBounds(zone2.withTrimmedLeft(HORIZONTAL_PADDING)
                               .withTrimmedRight(HORIZONTAL_PADDING)
                               .withTop(zone2.getY() + y2)
                               .withHeight(COMBO_HEIGHT));
    
    // Zone 3 : État
    auto zone3 = bounds;
    int y3 = (zone3.getHeight() - COMBO_HEIGHT) / 2;
    stateCombo.setBounds(zone3.withTrimmedLeft(HORIZONTAL_PADDING)
                              .withTrimmedRight(HORIZONTAL_PADDING)
                              .withTop(zone3.getY() + y3)
                              .withHeight(COMBO_HEIGHT));
}

void InfoColoredPanel::setColor(juce::Colour color)
{
    ColoredPanel::setColor(color);
    
    degreeCombo.adaptToBackgroundColour(color);
    qualityCombo.adaptToBackgroundColour(color);
    stateCombo.adaptToBackgroundColour(color);
    
    repaint();
}

void InfoColoredPanel::populateDegreeCombo(const juce::StringArray& items)
{
    degreeCombo.clear();
    for (int i = 0; i < items.size(); ++i)
        degreeCombo.addItem(items[i], i + 1);
    if (items.size() > 0)
        degreeCombo.setSelectedId(1, juce::dontSendNotification);
}

void InfoColoredPanel::populateQualityCombo(const juce::StringArray& items)
{
    qualityCombo.clear();
    for (int i = 0; i < items.size(); ++i)
        qualityCombo.addItem(items[i], i + 1);
    if (items.size() > 0)
        qualityCombo.setSelectedId(1, juce::dontSendNotification);
}

void InfoColoredPanel::populateStateCombo(const juce::StringArray& items)
{
    stateCombo.clear();
    for (int i = 0; i < items.size(); ++i)
        stateCombo.addItem(items[i], i + 1);
    if (items.size() > 0)
        stateCombo.setSelectedId(1, juce::dontSendNotification);
}

void InfoColoredPanel::populateDegreeCombo(const juce::StringArray& items, const juce::StringArray& shortItems)
{
    degreeCombo.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        degreeCombo.addItem(items[i], i + 1);
        if (i < shortItems.size())
            degreeCombo.setShortTextForItem(i + 1, shortItems[i]);
    }
    degreeCombo.enableShortDisplayMode(true);
    if (items.size() > 0)
        degreeCombo.setSelectedId(1, juce::dontSendNotification);
}

void InfoColoredPanel::populateQualityCombo(const juce::StringArray& items, const juce::StringArray& shortItems)
{
    qualityCombo.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        qualityCombo.addItem(items[i], i + 1);
        if (i < shortItems.size())
            qualityCombo.setShortTextForItem(i + 1, shortItems[i]);
    }
    qualityCombo.enableShortDisplayMode(true);
    if (items.size() > 0)
        qualityCombo.setSelectedId(1, juce::dontSendNotification);
}

void InfoColoredPanel::populateStateCombo(const juce::StringArray& items, const juce::StringArray& shortItems)
{
    stateCombo.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        stateCombo.addItem(items[i], i + 1);
        if (i < shortItems.size())
            stateCombo.setShortTextForItem(i + 1, shortItems[i]);
    }
    stateCombo.enableShortDisplayMode(true);
    if (items.size() > 0)
        stateCombo.setSelectedId(1, juce::dontSendNotification);
}

void InfoColoredPanel::setupLabels()
{
    numberLabel.setJustificationType(juce::Justification::centred);
    auto numberFont = juce::Font(fontManager->getSFProDisplay(11.0f, FontManager::FontWeight::Bold));
    numberLabel.setFont(numberFont);
    // Le numéro est dessiné directement dans paint(), pas besoin d'ajouter le label
}

void InfoColoredPanel::setNumber(int number)
{
    panelNumber = number;
    repaint();
}

juce::Colour InfoColoredPanel::getFunctionalStripColor() const
{
    int selectedId = degreeCombo.getSelectedId();
    
    // IDs dans la combo basés sur la fonction tonale :
    // 1=I, 2=II, 3=III, 4=IV, 5=V, 6=VI, 7=VII, 8+=autres (V/II, V/III, etc.)
    
    switch (selectedId)
    {
        case 1:  // I
        case 3:  // III
        case 6:  // VI
            // Fonction Tonique : Bleu
            return juce::Colour(0xFF4A90A4);
            
        case 2:  // II
        case 4:  // IV
            // Fonction Sous-Dominante : Or
            return juce::Colour(0xFFF1C40F);
            
        case 5:  // V
        case 7:  // VII
            // Fonction Dominante : Rouge
            return juce::Colour(0xFFE74C3C);
            
        default: // Autres degrés (V/II, V/III, bII, etc.)
            return juce::Colours::lightgrey;
    }
}
