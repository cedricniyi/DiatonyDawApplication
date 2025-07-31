#include "Zone4ContentArea.h"
#include "ui/extra/Component/Panel/InfoColoredPanel.h"

//==============================================================================
Zone4ContentArea::Zone4ContentArea() 
    : ColoredPanel(juce::Colours::lightsteelblue.withAlpha(0.75f)),
      scrollableContent(std::make_unique<Zone4ScrollablePanel>())
{
    setAlpha(1.0f);
    
    setupViewport();
    setupEmptyLabel();
    updateVisibility();
}

Zone4ContentArea::~Zone4ContentArea()
{
}

void Zone4ContentArea::paint(juce::Graphics& g)
{
    ColoredPanel::paint(g);
}

void Zone4ContentArea::resized()
{
    ColoredPanel::resized();
    
    // Padding asymétrique : côtés + haut, mais pas en bas pour garder la scrollbar accessible
    auto bounds = getLocalBounds();
    auto contentArea = bounds.withTrimmedLeft(HORIZONTAL_PADDING)
                             .withTrimmedRight(HORIZONTAL_PADDING)
                             .withTrimmedTop(TOP_PADDING)
                             .withTrimmedBottom(BOTTOM_PADDING);
    
    viewport.setBounds(contentArea);
    emptyLabel.setBounds(contentArea);
    
    if (scrollableContent)
    {
        // Plus de hauteur disponible pour les rectangles
        int availableHeight = contentArea.getHeight();
        scrollableContent->setSize(scrollableContent->getWidth(), availableHeight);
    }
}

void Zone4ContentArea::addRectangle()
{
    if (!scrollableContent)
        return;
    
    // Créer un rectangle coloré avec zones d'information
    auto newRectangle = std::make_unique<InfoColoredPanel>(getNextColour());
    
    // Configuration du rectangle
    newRectangle->setAlpha(0.8f);
    
    // Ajout au contenu scrollable
    std::unique_ptr<juce::Component> component(newRectangle.release());
    scrollableContent->addRectangle(std::move(component));
    
    updateVisibility();
    resized();
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
    // Hauteur initiale calculée avec le nouveau padding
    int initialHeight = PREFERRED_HEIGHT - TOP_PADDING - BOTTOM_PADDING;
    scrollableContent->setSize(100, initialHeight);
    
    // Configuration du viewport avec scrollbar horizontale uniquement
    viewport.setViewedComponent(scrollableContent.get(), false);
    viewport.setScrollBarsShown(false, true, false, false);
    viewport.setScrollBarPosition(true, true);
    
    addAndMakeVisible(viewport);
}

void Zone4ContentArea::setupEmptyLabel()
{
    // Configuration du label d'état vide
    emptyLabel.setText("Aucun élément ajouté", juce::dontSendNotification);
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
    
    // Affichage conditionnel : viewport si contenu, label si vide
    viewport.setVisible(hasContentNow);
    emptyLabel.setVisible(!hasContentNow);
}

juce::Colour Zone4ContentArea::getNextColour()
{
    // Rotation des couleurs disponibles
    auto colour = availableColours[(nextRectangleId - 1) % availableColours.size()];
    nextRectangleId++;
    return colour;
}