#include "OverviewContentArea.h"
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"
#include "model/ModelIdentifiers.h"

OverviewContentArea::OverviewContentArea() 
    : overlayContainer(std::make_unique<juce::Component>())
{
    setOpaque(false);
    setupViewport();
    setupEmptyLabel();
    updateVisibility();
}

OverviewContentArea::~OverviewContentArea()
{
    if (modelState.isValid())
        modelState.removeListener(this);
    
    if (selectionState.isValid())
        selectionState.removeListener(this);
}

void OverviewContentArea::parentHierarchyChanged()
{
    juce::Component::parentHierarchyChanged();
    findAppController();
}

void OverviewContentArea::findAppController()
{
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        setModelState(appController->getState());
        
        selectionState = appController->getSelectionState();
        if (selectionState.isValid())
            selectionState.addListener(this);
    }
    else
    {
        appController = nullptr;
        if (selectionState.isValid())
        {
            selectionState.removeListener(this);
            selectionState = juce::ValueTree();
        }
    }
}

void OverviewContentArea::setModelState(juce::ValueTree& newModelState)
{
    if (modelState.isValid())
        modelState.removeListener(this);
        
    modelState = newModelState;
    modelState.addListener(this);
    refreshFromModel();
}

void OverviewContentArea::refreshFromModel()
{
    if (!modelState.isValid())
        return;
        
    sectionPanels.clear();
    modulationPanels.clear();
    
    int sectionIndex = 0;
    
    for (int i = 0; i < modelState.getNumChildren(); ++i)
    {
        auto child = modelState.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            createPanelForSection(child, sectionIndex, false);
            sectionIndex++;
        }
        else if (child.hasType(ModelIdentifiers::MODULATION))
        {
            createPanelForModulation(child);
        }
    }
    
    layoutPanels();
    updateVisibility();
    updateSelectionHighlight();
}

void OverviewContentArea::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                  const juce::Identifier& property)
{
    if (property == ModelIdentifiers::name && treeWhosePropertyHasChanged.hasType(ModelIdentifiers::SECTION))
        refreshFromModel();
    else if (property == ContextIdentifiers::selectedElementId && treeWhosePropertyHasChanged == selectionState)
        updateSelectionHighlight();
}

void OverviewContentArea::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree& childWhichHasBeenAdded)
{
    if (childWhichHasBeenAdded.hasType(ModelIdentifiers::SECTION))
        handleSectionAdded(childWhichHasBeenAdded);
    else if (childWhichHasBeenAdded.hasType(ModelIdentifiers::MODULATION))
        refreshFromModel();
}

void OverviewContentArea::valueTreeChildRemoved(juce::ValueTree&, 
                                               juce::ValueTree& childWhichHasBeenRemoved, 
                                               int)
{
    if (childWhichHasBeenRemoved.hasType(ModelIdentifiers::SECTION))
        handleSectionRemoved();
    else if (childWhichHasBeenRemoved.hasType(ModelIdentifiers::MODULATION))
        refreshFromModel();
}

void OverviewContentArea::valueTreeChildOrderChanged(juce::ValueTree&, int, int)
{
    refreshFromModel();
}

void OverviewContentArea::valueTreeParentChanged(juce::ValueTree&) {}

void OverviewContentArea::handleSectionAdded(const juce::ValueTree&) { refreshFromModel(); }

void OverviewContentArea::handleSectionRemoved() { refreshFromModel(); }

void OverviewContentArea::createPanelForSection(const juce::ValueTree& sectionNode, int sectionIndex, bool autoSelect)
{
    int sectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    newPanel->setContentType(PanelContentType::Section);
    newPanel->setUserData(sectionId);
    newPanel->setDisplayText("P" + juce::String(sectionIndex + 1));
    newPanel->setShowText(true);
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    newPanel->onRightClick = [this, sectionId]() {
        if (appController)
        {
            auto& piece = appController->getPiece();
            int currentIndex = piece.getSectionIndexById(sectionId);
            if (currentIndex >= 0)
                appController->removeSection(currentIndex);
        }
    };
    
    overlayContainer->addAndMakeVisible(newPanel.get());
    sectionPanels.push_back(std::move(newPanel));
    
    if (autoSelect && appController && sectionIndex >= 0)
        appController->selectSection(sectionIndex);
}

void OverviewContentArea::createPanelForModulation(const juce::ValueTree& modulationNode)
{
    int modulationId = modulationNode.getProperty(ModelIdentifiers::id, -1);
    if (modulationId < 0)
        return;
    
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::green);
    newPanel->setContentType(PanelContentType::Modulation);
    newPanel->setUserData(modulationId);
    newPanel->setShowText(false);
    newPanel->setAlpha(0.7f);
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    overlayContainer->addAndMakeVisible(newPanel.get());
    modulationPanels.push_back(std::move(newPanel));
}

void OverviewContentArea::layoutPanels()
{
    if (!overlayContainer)
        return;
    
    int x = 0;
    int sectionIdx = 0;
    int modulationIdx = 0;
    
    for (int i = 0; i < modelState.getNumChildren(); ++i)
    {
        auto child = modelState.getChild(i);
        
        if (child.hasType(ModelIdentifiers::SECTION) && sectionIdx < (int)sectionPanels.size())
        {
            sectionPanels[sectionIdx]->setBounds(x, 10, SECTION_WIDTH, SECTION_HEIGHT);
            x += SECTION_WIDTH;
            sectionIdx++;
        }
        else if (child.hasType(ModelIdentifiers::MODULATION) && modulationIdx < (int)modulationPanels.size())
        {
            // Modulation superposée à la jonction des sections
            int modX = x - MODULATION_WIDTH / 2;
            int modY = (PREFERRED_HEIGHT - MODULATION_HEIGHT) / 2;
            modulationPanels[modulationIdx]->setBounds(modX, modY, MODULATION_WIDTH, MODULATION_HEIGHT);
            modulationPanels[modulationIdx]->toFront(false);
            modulationIdx++;
        }
    }
    
    int totalWidth = juce::jmax(100, x);
    overlayContainer->setSize(totalWidth, PREFERRED_HEIGHT);
}

void OverviewContentArea::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds, cornerRadius);
}

void OverviewContentArea::resized()
{
    auto contentArea = getLocalBounds().reduced(CONTENT_MARGIN);
    viewport.setBounds(contentArea);
    emptyLabel.setBounds(contentArea);
    layoutPanels();
}

void OverviewContentArea::addSmallPanel()
{
    if (appController != nullptr)
        appController->addNewSection("Nouvelle Section");
}

void OverviewContentArea::clearAllPanels()
{
    sectionPanels.clear();
    modulationPanels.clear();
    updateVisibility();
    resized();
}

bool OverviewContentArea::hasContent() const { return !sectionPanels.empty(); }

juce::Rectangle<int> OverviewContentArea::getPreferredSize() const
{
    return juce::Rectangle<int>(0, 0, PREFERRED_WIDTH, PREFERRED_HEIGHT);
}

void OverviewContentArea::setupViewport()
{
    overlayContainer->setSize(100, PREFERRED_HEIGHT);
    
    viewport.setViewedComponent(overlayContainer.get(), false);
    viewport.setScrollBarsShown(false, true, false, false);
    viewport.setScrollBarPosition(true, true);
    
    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::lightgrey.withAlpha(0.4f));
    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::trackColourId, juce::Colours::transparentBlack);
    
    addAndMakeVisible(viewport);
}

void OverviewContentArea::setupEmptyLabel()
{
    emptyLabel.setText(juce::String::fromUTF8("No progression has been added"), juce::dontSendNotification);
    emptyLabel.setJustificationType(juce::Justification::centred);
    emptyLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.7f));
    
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProText(14.0f, FontManager::FontWeight::Regular);
    emptyLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(emptyLabel);
}

void OverviewContentArea::updateVisibility()
{
    bool hasContentNow = hasContent();
    viewport.setVisible(hasContentNow);
    emptyLabel.setVisible(!hasContentNow);
}

void OverviewContentArea::onPanelClicked(ButtonColoredPanel* clickedPanel)
{
    if (!clickedPanel || !appController)
        return;
        
    int elementId = clickedPanel->getUserData();
    auto contentType = clickedPanel->getContentType();
    
    if (contentType == PanelContentType::Section)
    {
        auto& piece = appController->getPiece();
        int sectionIndex = piece.getSectionIndexById(elementId);
        if (sectionIndex >= 0)
            appController->selectSection(sectionIndex);
    }
    else if (contentType == PanelContentType::Modulation)
    {
        auto& piece = appController->getPiece();
        int modulationIndex = piece.getModulationIndexById(elementId);
        if (modulationIndex >= 0)
            appController->selectModulation(modulationIndex);
    }
}

void OverviewContentArea::updateSelectionHighlight()
{
    if (!selectionState.isValid())
        return;
    
    juce::String selectedElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String selectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    
    for (auto& panel : sectionPanels)
    {
        int userData = static_cast<int>(panel->getUserData());
        bool shouldBeSelected = false;
        
        if (selectionType == "Section")
        {
            juce::String expectedElementId = "Section_" + juce::String(userData);
            shouldBeSelected = (expectedElementId == selectedElementId);
        }
        
        panel->setSelected(shouldBeSelected);
    }
    
    for (auto& panel : modulationPanels)
    {
        int userData = static_cast<int>(panel->getUserData());
        bool shouldBeSelected = false;
        
        if (selectionType == "Modulation")
        {
            juce::String expectedElementId = "Modulation_" + juce::String(userData);
            shouldBeSelected = (expectedElementId == selectedElementId);
        }
        
        panel->setSelected(shouldBeSelected);
    }
}
