#include "InfoColoredPanel.h"

InfoColoredPanel::InfoColoredPanel(juce::Colour color)
    : ColoredPanel(color)
{
    degreeCombo.adaptToBackgroundColour(color);
    qualityCombo.adaptToBackgroundColour(color);
    stateCombo.adaptToBackgroundColour(color);
    
    degreeCombo.setArrowVisible(false);
    qualityCombo.setArrowVisible(false);
    stateCombo.setArrowVisible(false);
    
    addAndMakeVisible(degreeCombo);
    addAndMakeVisible(qualityCombo);
    addAndMakeVisible(stateCombo);
    
    degreeCombo.addListener(this);
    
    lockIcon = juce::Drawable::createFromImageData(IconData::lock1svgrepocom_svg, 
                                                    IconData::lock1svgrepocom_svgSize);
    unlockIcon = juce::Drawable::createFromImageData(IconData::unlocksvgrepocom_svg, 
                                                      IconData::unlocksvgrepocom_svgSize);
    
    setupLabels();
}

InfoColoredPanel::~InfoColoredPanel()
{
    // Arrêter le timer pour éviter use-after-free si un long press est en cours
    stopTimer();
    degreeCombo.removeListener(this);
}

void InfoColoredPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &degreeCombo)
        repaint();
}

void InfoColoredPanel::paint(juce::Graphics& g)
{
    using namespace InfoPanelConfig;
    
    auto bounds = getLocalBounds().toFloat();
    
    juce::Path panelPath;
    panelPath.addRoundedRectangle(bounds, CORNER_RADIUS);
    g.setColour(getColor());
    g.fillPath(panelPath);
    
    // Bande de codage couleur harmonique en haut
    auto stripArea = bounds.removeFromTop(STRIP_HEIGHT);
    g.saveState();
    g.reduceClipRegion(panelPath);
    g.setColour(getFunctionalStripColor());
    g.fillRect(stripArea);
    g.restoreState();
    
    int totalTopHeight = static_cast<int>(STRIP_HEIGHT) + TOP_PADDING + TOP_ROW_HEIGHT;
    float comboZoneHeight = (getLocalBounds().toFloat().getHeight() - totalTopHeight) / 3.0f;
    
    // Lignes de séparation
    g.saveState();
    g.reduceClipRegion(panelPath);
    g.setColour(getColor().contrasting(0.15f));
    
    g.drawLine(0.0f, totalTopHeight, getLocalBounds().toFloat().getWidth(), totalTopHeight, 1.0f);
    g.drawLine(0.0f, totalTopHeight + comboZoneHeight, getLocalBounds().toFloat().getWidth(), totalTopHeight + comboZoneHeight, 1.0f);
    g.drawLine(0.0f, totalTopHeight + comboZoneHeight * 2.0f, getLocalBounds().toFloat().getWidth(), totalTopHeight + comboZoneHeight * 2.0f, 1.0f);
    g.restoreState();
    
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
    
    // Carré 2 : Cadenas
    lockSquareArea = juce::Rectangle<int>(startX + SQUARE_SIZE + SQUARE_SPACING, y, SQUARE_SIZE, SQUARE_SIZE);
    g.setColour(locked 
        ? juce::Colour(0xFFD4A017).withAlpha(0.7f)  // Or si verrouillé
        : squareColor.withAlpha(0.5f));
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
    auto* iconToDraw = isLocked ? lockIcon.get() : unlockIcon.get();
    
    if (iconToDraw == nullptr)
        return;
    
    auto iconCopy = iconToDraw->createCopy();
    auto iconColor = getColor().contrasting(0.8f);
    iconCopy->replaceColour(juce::Colours::black, iconColor);
    
    auto iconBounds = area.toFloat().reduced(3.0f);
    iconCopy->drawWithin(g, iconBounds, juce::RectanglePlacement::centred, 1.0f);
}

void InfoColoredPanel::drawDeleteIcon(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    auto areaFloat = area.toFloat();
    
    // Fond avec progression rouge
    if (deleteProgress > 0.0f)
    {
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
    if (deleteSquareArea.contains(event.getPosition()))
    {
        isDeleteHeldDown = true;
        deleteProgress = 0.0f;
        deleteStartTime = juce::Time::getMillisecondCounter();
        startTimerHz(60);
        repaint();
    }
    
    ColoredPanel::mouseDown(event);
}

void InfoColoredPanel::mouseUp(const juce::MouseEvent& event)
{
    if (lockSquareArea.contains(event.getPosition()))
    {
        locked = !locked;
        repaint();
        
        if (onLockToggled)
            onLockToggled(locked);
    }
    
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
        stopTimer();
        isDeleteHeldDown = false;
        deleteProgress = 0.0f;
        
        if (onDeleteRequested)
            onDeleteRequested();
        
        // IMPORTANT: Retourner immédiatement !
        // onDeleteRequested() peut détruire ce panel (via syncWithProgression)
        // Tout code après ce point serait un use-after-free
        return;
    }
    
    repaint();
}

void InfoColoredPanel::resized()
{
    using namespace InfoPanelConfig;
    
    auto bounds = getLocalBounds();
    
    int totalTopHeight = static_cast<int>(STRIP_HEIGHT) + TOP_PADDING + TOP_ROW_HEIGHT;
    bounds.removeFromTop(totalTopHeight);
    
    int comboZoneHeight = bounds.getHeight() / 3;
    
    auto zone1 = bounds.removeFromTop(comboZoneHeight);
    int y1 = (zone1.getHeight() - COMBO_HEIGHT) / 2;
    degreeCombo.setBounds(zone1.withTrimmedLeft(HORIZONTAL_PADDING)
                              .withTrimmedRight(HORIZONTAL_PADDING)
                              .withTop(zone1.getY() + y1)
                              .withHeight(COMBO_HEIGHT));
    
    auto zone2 = bounds.removeFromTop(comboZoneHeight);
    int y2 = (zone2.getHeight() - COMBO_HEIGHT) / 2;
    qualityCombo.setBounds(zone2.withTrimmedLeft(HORIZONTAL_PADDING)
                               .withTrimmedRight(HORIZONTAL_PADDING)
                               .withTop(zone2.getY() + y2)
                               .withHeight(COMBO_HEIGHT));
    
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
}

void InfoColoredPanel::setNumber(int number)
{
    panelNumber = number;
    repaint();
}

juce::Colour InfoColoredPanel::getFunctionalStripColor() const
{
    int selectedId = degreeCombo.getSelectedId();
    
    // Couleur selon fonction tonale : T (I/III/VI), SD (II/IV), D (V/VII)
    switch (selectedId)
    {
        case 1:  // I
        case 3:  // III
        case 6:  // VI
            return juce::Colour(0xFF4A90A4);  // Bleu - Tonique
            
        case 2:  // II
        case 4:  // IV
            return juce::Colour(0xFFF1C40F);  // Or - Sous-Dominante
            
        case 5:  // V
        case 7:  // VII
            return juce::Colour(0xFFE74C3C);  // Rouge - Dominante
            
        default:
            return juce::Colours::lightgrey;
    }
}
