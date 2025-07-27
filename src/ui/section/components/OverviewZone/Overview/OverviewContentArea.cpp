#include "OverviewContentArea.h"
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"
#include "model/Identifiers.h"

//==============================================================================
OverviewContentArea::OverviewContentArea() 
    : ColoredPanel(juce::Colour::fromString("#ffe3e3ff")),
      scrollableContent(std::make_unique<ScrollableContentPanel>())
{
    setAlpha(1.0f);
    
    setupViewport();
    setupEmptyLabel();
    updateVisibility();
}

OverviewContentArea::~OverviewContentArea()
{
    // Nettoyer les listeners du ValueTree
    if (modelState.isValid())
        modelState.removeListener(this);
}

// === DÉCOUVERTE DE SERVICE ===

void OverviewContentArea::parentHierarchyChanged()
{
    ColoredPanel::parentHierarchyChanged();
    findAppController();
}

void OverviewContentArea::findAppController()
{
    // Recherche de l'AppController via la hiérarchie des composants
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        
        // Connexion au ValueTree du modèle une fois AppController trouvé
        auto modelTree = appController->getPiece().getState();
        setModelState(modelTree);
    }
    else
    {
        appController = nullptr;
    }
}

// === ARCHITECTURE RÉACTIVE ===

void OverviewContentArea::setModelState(juce::ValueTree& newModelState)
{
    // Déconnexion de l'ancien listener
    if (modelState.isValid())
        modelState.removeListener(this);
        
    // Connexion au nouveau ValueTree
    modelState = newModelState;
    modelState.addListener(this);
    
    // Rafraîchissement de l'UI depuis le modèle
    refreshFromModel();
}

void OverviewContentArea::refreshFromModel()
{
    if (!modelState.isValid())
        return;
        
    // Comptage des sections dans le modèle
    int sectionCount = 0;
    for (int i = 0; i < modelState.getNumChildren(); ++i)
    {
        if (modelState.getChild(i).hasType(Identifiers::SECTION))
            sectionCount++;
    }
    
    // Création des panels pour les sections existantes
    if (scrollableContent && sectionCount > 0)
    {
        selectedPanel = nullptr;
        scrollableContent->clearAllPanels();
        
        // Création d'un panel pour chaque section
        for (int i = 0; i < modelState.getNumChildren(); ++i)
        {
            auto child = modelState.getChild(i);
            if (child.hasType(Identifiers::SECTION))
            {
                createPanelForSection(child, false);
            }
        }
    }
    
    updateVisibility();
}

// =================================================================================
// ValueTree::Listener interface - Réactivité aux changements du modèle

void OverviewContentArea::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                  const juce::Identifier& property)
{
    // Mise à jour de l'affichage lors du changement de nom de section
    if (property == Identifiers::name)
    {
        refreshFromModel();
    }
}

void OverviewContentArea::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (childWhichHasBeenAdded.hasType(Identifiers::SECTION))
    {
        handleSectionAdded(childWhichHasBeenAdded);
    }
}

void OverviewContentArea::valueTreeChildRemoved(juce::ValueTree& parentTree, 
                                               juce::ValueTree& childWhichHasBeenRemoved, 
                                               int indexFromWhichChildWasRemoved)
{
    if (childWhichHasBeenRemoved.hasType(Identifiers::SECTION))
    {
        handleSectionRemoved();
    }
}

void OverviewContentArea::valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, 
                                                    int oldIndex, int newIndex)
{
    refreshFromModel();
}

void OverviewContentArea::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    // Gestion des changements de parent si nécessaire
}

// === MÉTHODES POUR L'ARCHITECTURE RÉACTIVE ===

void OverviewContentArea::handleSectionAdded(const juce::ValueTree& sectionNode)
{
    // Création et sélection automatique du panel pour la nouvelle section
    createPanelForSection(sectionNode, true);
}

void OverviewContentArea::handleSectionRemoved()
{
    // Reconstruction complète des panels après suppression
    refreshFromModel();
}

void OverviewContentArea::createPanelForSection(const juce::ValueTree& sectionNode, bool autoSelect)
{
    if (!scrollableContent)
        return;
    
    // Création du panel avec couleur bleue
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    
    // Association de l'ID de section au panel
    auto sectionId = sectionNode.getProperty(Identifiers::id, nextPanelId++);
    newPanel->setUserData(sectionId);
    
    // Référence au panel avant transfert de propriété
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Ajout au contenu scrollable
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component));
    
    // Gestion de la sélection automatique
    if (autoSelect)
    {
        if (selectedPanel)
        {
            selectedPanel->setSelected(false);
        }
        
        newPanelPtr->setSelected(true);
        selectedPanel = newPanelPtr;
    }
    
    updateVisibility();
    resized();
}

// === MÉTHODES UI EXISTANTES ===

void OverviewContentArea::paint(juce::Graphics& g)
{
    ColoredPanel::paint(g);
}

void OverviewContentArea::resized()
{
    ColoredPanel::resized();
    auto contentArea = getLocalBounds().reduced(CONTENT_MARGIN);
    
    viewport.setBounds(contentArea);
    emptyLabel.setBounds(contentArea);
    
    if (scrollableContent)
    {
        scrollableContent->setSize(scrollableContent->getWidth(), PREFERRED_HEIGHT);
    }
}

void OverviewContentArea::addSmallPanel()
{
    if (!scrollableContent)
        return;
    
    // Utilisation d'AppController pour l'architecture réactive
    if (appController != nullptr)
    {
        // L'UI se mettra à jour automatiquement via les listeners ValueTree
        appController->addNewSection("Nouvelle Section");
        return;
    }
    
    // Fallback : création manuelle si AppController indisponible
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    newPanel->setUserData(nextPanelId++);
    
    // Désélection de l'ancien panel
    if (selectedPanel)
    {
        selectedPanel->setSelected(false);
    }
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Ajout et sélection du nouveau panel
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component));
    
    newPanelPtr->setSelected(true);
    selectedPanel = newPanelPtr;
    
    updateVisibility();
    resized();
}

void OverviewContentArea::clearAllPanels()
{
    if (scrollableContent)
    {
        selectedPanel = nullptr;
        scrollableContent->clearAllPanels();
        updateVisibility();
        resized();
    }
}

bool OverviewContentArea::hasContent() const
{
    return scrollableContent && scrollableContent->getNumPanels() > 0;
}

juce::Rectangle<int> OverviewContentArea::getPreferredSize() const
{
    return juce::Rectangle<int>(0, 0, PREFERRED_WIDTH, PREFERRED_HEIGHT);
}

ButtonColoredPanel* OverviewContentArea::getSelectedPanel() const
{
    return selectedPanel;
}

bool OverviewContentArea::hasSelectedPanel() const
{
    return selectedPanel != nullptr;
}

void OverviewContentArea::setupViewport()
{
    scrollableContent->setSize(100, PREFERRED_HEIGHT);
    
    // Configuration du viewport avec scrollbar horizontale uniquement
    viewport.setViewedComponent(scrollableContent.get(), false);
    viewport.setScrollBarsShown(false, true, false, false);
    viewport.setScrollBarPosition(true, true);
    
    addAndMakeVisible(viewport);
}

void OverviewContentArea::setupEmptyLabel()
{
    // Configuration du label d'état vide
    emptyLabel.setText(juce::String::fromUTF8("Aucun progression n'a été ajouté à la pièce"), juce::dontSendNotification);
    emptyLabel.setJustificationType(juce::Justification::centred);
    emptyLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.7f));
    
    // Application de la police via FontManager
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProText(18.0f, FontManager::FontWeight::Regular);
    emptyLabel.setFont(juce::Font(fontOptions));
    
    addAndMakeVisible(emptyLabel);
}

void OverviewContentArea::updateVisibility()
{
    bool hasContentNow = hasContent();
    
    // Affichage conditionnel : viewport si contenu, label si vide
    viewport.setVisible(hasContentNow);
    emptyLabel.setVisible(!hasContentNow);
}

void OverviewContentArea::onPanelClicked(ButtonColoredPanel* clickedPanel)
{
    if (!clickedPanel)
        return;
        
    // Gestion de la sélection de panel
    if (selectedPanel != clickedPanel)
    {
        if (selectedPanel)
        {
            selectedPanel->setSelected(false);
        }
        
        clickedPanel->setSelected(true);
        selectedPanel = clickedPanel;
    }
} 