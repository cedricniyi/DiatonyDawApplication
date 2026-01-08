#include "OverviewContentArea.h"
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"
#include "model/ModelIdentifiers.h"

//==============================================================================
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
    // Nettoyer les listeners du ValueTree
    if (modelState.isValid())
        modelState.removeListener(this);
    
    if (selectionState.isValid())
        selectionState.removeListener(this);
}

// === DÉCOUVERTE DE SERVICE ===

void OverviewContentArea::parentHierarchyChanged()
{
    juce::Component::parentHierarchyChanged();
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
        setModelState(appController->getState());
        
        // S'abonner à l'état de sélection centralisé
        selectionState = appController->getSelectionState();
        if (selectionState.isValid())
        {
            selectionState.addListener(this);
        }
    }
    else
    {
        appController = nullptr;
        
        // Nettoyer les listeners si on perd AppController
        if (selectionState.isValid())
        {
            selectionState.removeListener(this);
            selectionState = juce::ValueTree();
        }
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
        
    // Effacer tous les panels existants
    sectionPanels.clear();
    modulationPanels.clear();
    
    // Compteur d'index pour les sections (position visuelle, pas ID)
    int sectionIndex = 0;
    
    // Création d'un panel pour chaque enfant du modèle
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
    
    // Positionner les panels avec modulations superposées
    layoutPanels();
    
    updateVisibility();
    
    // Rétablir la surbrillance de sélection après reconstruction
    updateSelectionHighlight();
}

// =================================================================================
// ValueTree::Listener interface - Réactivité aux changements du modèle

void OverviewContentArea::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                  const juce::Identifier& property)
{
    if (property == ModelIdentifiers::name && treeWhosePropertyHasChanged.hasType(ModelIdentifiers::SECTION))
    {
        refreshFromModel();
    }
    else if (property == ContextIdentifiers::selectedElementId && treeWhosePropertyHasChanged == selectionState)
    {
        updateSelectionHighlight();
    }
}

void OverviewContentArea::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (childWhichHasBeenAdded.hasType(ModelIdentifiers::SECTION))
    {
        handleSectionAdded(childWhichHasBeenAdded);
    }
    else if (childWhichHasBeenAdded.hasType(ModelIdentifiers::MODULATION))
    {
        refreshFromModel();
    }
}

void OverviewContentArea::valueTreeChildRemoved(juce::ValueTree& parentTree, 
                                               juce::ValueTree& childWhichHasBeenRemoved, 
                                               int indexFromWhichChildWasRemoved)
{
    if (childWhichHasBeenRemoved.hasType(ModelIdentifiers::SECTION))
    {
        handleSectionRemoved();
    }
    else if (childWhichHasBeenRemoved.hasType(ModelIdentifiers::MODULATION))
    {
        refreshFromModel();
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
    refreshFromModel();
}

void OverviewContentArea::handleSectionRemoved()
{
    refreshFromModel();
}

void OverviewContentArea::createPanelForSection(const juce::ValueTree& sectionNode, int sectionIndex, bool autoSelect)
{
    // Récupérer l'ID de la section
    int sectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    // Création du panel avec couleur bleue
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    
    newPanel->setContentType(PanelContentType::Section);
    newPanel->setUserData(sectionId);
    
    // Affichage avec préfixe "P" pour les sections
    newPanel->setDisplayText("P" + juce::String(sectionIndex + 1));
    newPanel->setShowText(true);
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Callback de clic gauche (sélection)
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Callback clic droit (suppression)
    newPanel->onRightClick = [this, sectionId]() {
        if (appController)
        {
            auto& piece = appController->getPiece();
            int currentIndex = piece.getSectionIndexById(sectionId);
            
            if (currentIndex >= 0)
            {
                DBG("[OverviewContentArea] Clic droit - Suppression section ID=" << sectionId << " Index=" << currentIndex);
                appController->removeSection(currentIndex);
            }
        }
    };
    
    // Ajouter au conteneur et stocker
    overlayContainer->addAndMakeVisible(newPanel.get());
    sectionPanels.push_back(std::move(newPanel));
    
    // Sélection automatique si demandée
    if (autoSelect && appController && sectionIndex >= 0)
    {
        appController->selectSection(sectionIndex);
    }
}

void OverviewContentArea::createPanelForModulation(const juce::ValueTree& modulationNode)
{
    int modulationId = modulationNode.getProperty(ModelIdentifiers::id, -1);
    
    if (modulationId < 0)
        return;
    
    // Création du panel avec couleur verte pour les modulations
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::green);
    
    newPanel->setContentType(PanelContentType::Modulation);
    newPanel->setUserData(modulationId);
    newPanel->setShowText(false);
    newPanel->setAlpha(0.7f);  // Légèrement transparent
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Ajouter au conteneur et stocker
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
    
    // Parcourir le modèle pour garder l'ordre correct
    for (int i = 0; i < modelState.getNumChildren(); ++i)
    {
        auto child = modelState.getChild(i);
        
        if (child.hasType(ModelIdentifiers::SECTION) && sectionIdx < (int)sectionPanels.size())
        {
            // Positionner la section (pas d'arrondis, collées)
            sectionPanels[sectionIdx]->setBounds(x, 10, SECTION_WIDTH, SECTION_HEIGHT);
            x += SECTION_WIDTH;  // Pas d'espacement entre sections
            sectionIdx++;
        }
        else if (child.hasType(ModelIdentifiers::MODULATION) && modulationIdx < (int)modulationPanels.size())
        {
            // Positionner la modulation superposée à la jonction des sections
            int modX = x - MODULATION_WIDTH / 2;
            int modY = (PREFERRED_HEIGHT - MODULATION_HEIGHT) / 2;
            
            modulationPanels[modulationIdx]->setBounds(modX, modY, MODULATION_WIDTH, MODULATION_HEIGHT);
            modulationPanels[modulationIdx]->toFront(false);
            modulationIdx++;
        }
    }
    
    // Mettre à jour la taille du conteneur
    int totalWidth = juce::jmax(100, x);
    overlayContainer->setSize(totalWidth, PREFERRED_HEIGHT);
}

// === MÉTHODES UI ===

void OverviewContentArea::paint(juce::Graphics& g)
{
    // Fond noir arrondi
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
    // Utilisation d'AppController pour l'architecture réactive
    if (appController != nullptr)
    {
        appController->addNewSection("Nouvelle Section");
        return;
    }
    
    // Fallback si AppController indisponible - ne fait rien
    DBG("[OverviewContentArea] addSmallPanel appelé sans AppController");
}

void OverviewContentArea::clearAllPanels()
{
    sectionPanels.clear();
    modulationPanels.clear();
    updateVisibility();
    resized();
}

bool OverviewContentArea::hasContent() const
{
    return !sectionPanels.empty();
}

juce::Rectangle<int> OverviewContentArea::getPreferredSize() const
{
    return juce::Rectangle<int>(0, 0, PREFERRED_WIDTH, PREFERRED_HEIGHT);
}

void OverviewContentArea::setupViewport()
{
    overlayContainer->setSize(100, PREFERRED_HEIGHT);
    
    // Configuration du viewport avec scrollbar horizontale uniquement
    viewport.setViewedComponent(overlayContainer.get(), false);
    viewport.setScrollBarsShown(false, true, false, false);
    viewport.setScrollBarPosition(true, true);
    
    // Couleur de la scrollbar
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
        {
            appController->selectSection(sectionIndex);
        }
    }
    else if (contentType == PanelContentType::Modulation)
    {
        auto& piece = appController->getPiece();
        int modulationIndex = piece.getModulationIndexById(elementId);
        
        if (modulationIndex >= 0)
        {
            appController->selectModulation(modulationIndex);
        }
    }
}

void OverviewContentArea::updateSelectionHighlight()
{
    if (!selectionState.isValid())
        return;
    
    juce::String selectedElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String selectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    
    // Mettre à jour les sections
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
    
    // Mettre à jour les modulations
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
