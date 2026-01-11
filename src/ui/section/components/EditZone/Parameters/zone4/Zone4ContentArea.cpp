#include "Zone4ContentArea.h"
#include "ui/extra/Component/Panel/InfoColoredPanel.h"

Zone4ContentArea::Zone4ContentArea() 
    : ColoredPanel(juce::Colours::black),
      scrollableContent(std::make_unique<Zone4ScrollablePanel>())
{
    setAlpha(1.0f);
    setupViewport();
    setupEmptyLabel();
    updateVisibility();
}

Zone4ContentArea::~Zone4ContentArea() {}

void Zone4ContentArea::paint(juce::Graphics& g)
{
    ColoredPanel::paint(g);
}

void Zone4ContentArea::resized()
{
    ColoredPanel::resized();
    
    auto bounds = getLocalBounds();
    auto contentArea = bounds.withTrimmedLeft(HORIZONTAL_PADDING)
                             .withTrimmedRight(HORIZONTAL_PADDING)
                             .withTrimmedTop(TOP_PADDING)
                             .withTrimmedBottom(BOTTOM_PADDING);
    
    viewport.setBounds(contentArea);
    emptyLabel.setBounds(contentArea);
    
    if (scrollableContent)
    {
        int availableHeight = contentArea.getHeight();
        scrollableContent->setSize(scrollableContent->getWidth(), availableHeight);
    }
}

void Zone4ContentArea::addRectangle()
{
    addRectangle(juce::ValueTree());
}

void Zone4ContentArea::addRectangle(juce::ValueTree chordState)
{
    if (!scrollableContent)
        return;
    
    auto newRectangle = std::make_unique<InfoColoredPanel>(getNextColour());
    newRectangle->setAlpha(0.8f);
    newRectangle->setNumber(nextRectangleId - 1);
    
    populateInfoColoredPanel(newRectangle.get());
    
    if (chordState.isValid())
    {
        int degree = chordState.getProperty(ModelIdentifiers::degree, 0);
        int quality = chordState.getProperty(ModelIdentifiers::quality, -1);
        int state = chordState.getProperty(ModelIdentifiers::state, 0);
        
        // Conversion quality : enumValue + 1 (car Auto=-1 est à l'index 0)
        newRectangle->getDegreeCombo().setSelectedItemIndex(degree, juce::dontSendNotification);
        newRectangle->getStateCombo().setSelectedItemIndex(state, juce::dontSendNotification);
        newRectangle->getQualityCombo().setSelectedItemIndex(quality + 1, juce::dontSendNotification);
        
        newRectangle->getDegreeCombo().refreshDisplayedText();
        newRectangle->getStateCombo().refreshDisplayedText();
        newRectangle->getQualityCombo().refreshDisplayedText();
        
        auto* degreeCombo = &newRectangle->getDegreeCombo();
        auto* stateCombo = &newRectangle->getStateCombo();
        auto* qualityCombo = &newRectangle->getQualityCombo();
        
        degreeCombo->onChange = [chordState, degreeCombo]() mutable {
            int newDegree = degreeCombo->getSelectedItemIndex();
            chordState.setProperty(ModelIdentifiers::degree, newDegree, nullptr);
        };
        
        stateCombo->onChange = [chordState, stateCombo]() mutable {
            int newState = stateCombo->getSelectedItemIndex();
            chordState.setProperty(ModelIdentifiers::state, newState, nullptr);
        };
        
        qualityCombo->onChange = [chordState, qualityCombo]() mutable {
            int comboboxIndex = qualityCombo->getSelectedItemIndex();
            int newQuality = comboboxIndex - 1;  // Conversion inverse
            chordState.setProperty(ModelIdentifiers::quality, newQuality, nullptr);
        };
        
        newRectangle->onDeleteRequested = [chordState]() mutable {
            if (chordState.isValid()) {
                auto parent = chordState.getParent();
                if (parent.isValid())
                    parent.removeChild(chordState, nullptr);
            }
        };
    }
    else
    {
        newRectangle->getDegreeCombo().setSelectedItemIndex(0, juce::dontSendNotification);
        newRectangle->getStateCombo().setSelectedItemIndex(0, juce::dontSendNotification);
        newRectangle->getQualityCombo().setSelectedItemIndex(0, juce::dontSendNotification);
        
        newRectangle->getDegreeCombo().refreshDisplayedText();
        newRectangle->getStateCombo().refreshDisplayedText();
        newRectangle->getQualityCombo().refreshDisplayedText();
    }
    
    std::unique_ptr<juce::Component> component(newRectangle.release());
    scrollableContent->addRectangle(std::move(component));
    
    updateVisibility();
    resized();
    
    // Auto-scroll vers le dernier élément ajouté
    juce::MessageManager::callAsync([this]() {
        if (scrollableContent)
        {
            int numRectangles = scrollableContent->getNumRectangles();
            if (numRectangles > 0)
            {
                int contentWidth = scrollableContent->getWidth();
                int viewWidth = viewport.getViewWidth();
                int scrollToX = juce::jmax(0, contentWidth - viewWidth);
                viewport.setViewPosition(scrollToX, 0);
            }
        }
    });
}

void Zone4ContentArea::clearAllRectangles()
{
    if (scrollableContent)
    {
        scrollableContent->clearAllRectangles();
        nextRectangleId = 1; // Reset du compteur
        updateVisibility();
        resized();
    }
}

bool Zone4ContentArea::hasContent() const
{
    return scrollableContent && scrollableContent->getNumRectangles() > 0;
}

juce::Rectangle<int> Zone4ContentArea::getPreferredSize() const
{
    return juce::Rectangle<int>(0, 0, PREFERRED_WIDTH, PREFERRED_HEIGHT);
}

void Zone4ContentArea::setupViewport()
{
    int initialHeight = PREFERRED_HEIGHT - TOP_PADDING - BOTTOM_PADDING;
    scrollableContent->setSize(100, initialHeight);
    
    viewport.setViewedComponent(scrollableContent.get(), false);
    viewport.setScrollBarsShown(false, true, false, false);
    viewport.setScrollBarPosition(true, true);
    
    // Couleur de la scrollbar : gris clair transparent
    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::lightgrey.withAlpha(0.4f));
    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::trackColourId, juce::Colours::transparentBlack);
    
    addAndMakeVisible(viewport);
}

void Zone4ContentArea::setupEmptyLabel()
{
    emptyLabel.setText(juce::String::fromUTF8("No chord added"), juce::dontSendNotification);
    emptyLabel.setJustificationType(juce::Justification::centred);
    emptyLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.7f));
    
    // Application de la police via FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProText(14.0f, FontManager::FontWeight::Regular);
    emptyLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(emptyLabel);
}

void Zone4ContentArea::updateVisibility()
{
    bool hasContentNow = hasContent();
    viewport.setVisible(hasContentNow);
    emptyLabel.setVisible(!hasContentNow);
}

juce::Colour Zone4ContentArea::getNextColour()
{
    // Gris uniforme pour tous les accords 
    nextRectangleId++;
    return juce::Colours::darkgrey;
}

void Zone4ContentArea::populateInfoColoredPanel(InfoColoredPanel* panel)
{
    if (!panel)
        return;
    
    // ChordDegree
    juce::StringArray degreeNames;
    for (const auto& degree : chordDegrees)
        degreeNames.add(DiatonyText::getChordDegreeName(degree));
    panel->populateDegreeCombo(degreeNames);
    
    // ChordQuality (noms complets + abréviations)
    juce::StringArray qualityNames, qualityShortNames;
    for (const auto& quality : chordQualities)
    {
        qualityNames.add(DiatonyText::getChordQualityName(quality));
        qualityShortNames.add(DiatonyText::getShortQualityName(quality));
    }
    panel->populateQualityCombo(qualityNames, qualityShortNames);
    
    // ChordState (état d'inversion)
    // Noms complets dans le dropdown, abréviations une fois sélectionné
    juce::StringArray stateNames;
    juce::StringArray stateShortNames;
    for (const auto& state : chordStates)
    {
        stateNames.add(DiatonyText::getChordStateName(state));
        stateShortNames.add(DiatonyText::getShortStateName(state));
    }
    panel->populateStateCombo(stateNames, stateShortNames);
}
