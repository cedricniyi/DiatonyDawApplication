#include "OverviewContentArea.h"
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"
#include "model/ModelIdentifiers.h"

//==============================================================================
OverviewContentArea::OverviewContentArea() 
    : ColoredPanel(juce::Colour::fromString("#ffe3e3ff")),
      overlayContainer(std::make_unique<juce::Component>())
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
    
    if (selectionState.isValid())
        selectionState.removeListener(this);
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
        
    // ✅ TERRE BRÛLÉE : Toujours effacer d'abord, puis reconstruire
    sectionPanels.clear();
    modulationPanels.clear();
    if (overlayContainer)
        overlayContainer->removeAllChildren();
    
    // Compteur d'index pour les sections (position visuelle, pas ID)
    int sectionIndex = 0;
    
    // Création d'un panel pour chaque enfant du modèle
    for (int i = 0; i < modelState.getNumChildren(); ++i)
    {
        auto child = modelState.getChild(i);
        if (child.hasType(ModelIdentifiers::SECTION))
        {
            createPanelForSection(child, sectionIndex, false);
            sectionIndex++;  // Incrémenter l'index seulement pour les sections
        }
        else if (child.hasType(ModelIdentifiers::MODULATION))
        {
            createPanelForModulation(child);
        }
    }
    
    // Appliquer le layout superposé
    layoutPanels();
    
    updateVisibility();
    
    // ✅ IMPORTANT : Rétablir la surbrillance de sélection après reconstruction
    updateSelectionHighlight();
}

// =================================================================================
// ValueTree::Listener interface - Réactivité aux changements du modèle

void OverviewContentArea::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                  const juce::Identifier& property)
{
    // Mise à jour de l'affichage lors du changement de nom de SECTION uniquement
    // (Les modulations n'affichent pas leur nom dans l'overview, donc pas besoin de refresh)
    if (property == ModelIdentifiers::name && treeWhosePropertyHasChanged.hasType(ModelIdentifiers::SECTION))
    {
        refreshFromModel();
    }
    // Réagir aux changements de sélection depuis l'état central
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
        // Pour les modulations, on rafraîchit simplement l'affichage
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
        // Pour les modulations, on rafraîchit simplement l'affichage
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
    // REBUILD COMPLET pour maintenir l'ordre correct (Section-Modulation-Section-...)
    // La sélection automatique est déjà gérée par AppController::addNewSection()
    refreshFromModel();
}

void OverviewContentArea::handleSectionRemoved()
{
    // Reconstruction complète des panels après suppression
    refreshFromModel();
}

void OverviewContentArea::createPanelForSection(const juce::ValueTree& sectionNode, int sectionIndex, bool autoSelect)
{
    if (!overlayContainer)
        return;
    
    // Récupérer l'ID de la section (identifiant unique permanent)
    int sectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    // Création du panel avec couleur bleue
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    
    // Définir le type de contenu
    newPanel->setContentType(PanelContentType::Section);
    
    // Stocker l'ID pour l'identification (utilisé par onPanelClicked pour la conversion ID→Index)
    newPanel->setUserData(sectionId);
    
    // ✅ AFFICHAGE : Utiliser l'INDEX avec préfixe "P" (P1, P2, P3...)
    newPanel->setDisplayText("P" + juce::String(sectionIndex + 1));
    newPanel->setShowText(true);
    
    // Référence au panel avant transfert de propriété
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic gauche (sélection)
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Configuration du callback clic droit (suppression)
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
            else
            {
                DBG("[OverviewContentArea] Section ID=" << sectionId << " déjà supprimée");
            }
        }
    };
    
    // Ajouter au conteneur et stocker dans le vecteur
    overlayContainer->addAndMakeVisible(*newPanel);
    sectionPanels.push_back(std::move(newPanel));
    
    // Sélection automatique si demandée
    if (autoSelect && appController && sectionIndex >= 0)
    {
        appController->selectSection(sectionIndex);
    }
}

void OverviewContentArea::createPanelForModulation(const juce::ValueTree& modulationNode)
{
    if (!overlayContainer)
        return;
    
    // Récupérer l'ID de la modulation
    int modulationId = modulationNode.getProperty(ModelIdentifiers::id, -1);
    
    if (modulationId < 0)
        return;
    
    // Création du panel avec couleur verte pour les modulations
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::green);
    
    // Définir le type de contenu
    newPanel->setContentType(PanelContentType::Modulation);
    
    // Stocker l'ID de la modulation
    newPanel->setUserData(modulationId);
    
    // ✅ Pas de texte + transparence pour voir les sections en dessous
    newPanel->setShowText(false);
    newPanel->setAlpha(0.7f);
    
    // Référence au panel avant transfert de propriété
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Ajouter au conteneur et stocker dans le vecteur
    overlayContainer->addAndMakeVisible(*newPanel);
    modulationPanels.push_back(std::move(newPanel));
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
    
    // Relayout des panels avec les nouvelles dimensions
    layoutPanels();
}

void OverviewContentArea::addSmallPanel()
{
    // Utilisation d'AppController pour l'architecture réactive
    if (appController != nullptr)
    {
        // L'UI se mettra à jour automatiquement via les listeners ValueTree
        appController->addNewSection("Nouvelle Section");
        return;
    }
    
    // Fallback : création manuelle si AppController indisponible
    if (!overlayContainer)
        return;
        
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    int panelId = nextPanelId++;
    newPanel->setUserData(panelId);
    newPanel->setContentType(PanelContentType::Section);
    newPanel->setDisplayText("P" + juce::String(panelId));
    newPanel->setShowText(true);
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    overlayContainer->addAndMakeVisible(*newPanel);
    sectionPanels.push_back(std::move(newPanel));
    
    layoutPanels();
    updateVisibility();
}

void OverviewContentArea::layoutPanels()
{
    if (!overlayContainer) return;
    
    // Calculer la hauteur totale nécessaire (modulations plus hautes que sections)
    int totalHeight = juce::jmax(SECTION_HEIGHT, MODULATION_HEIGHT) + 10;
    int centerY = totalHeight / 2;
    
    int x = 0;
    int ySection = centerY - (SECTION_HEIGHT / 2);  // Sections centrées verticalement
    int yModulation = centerY - (MODULATION_HEIGHT / 2);  // Modulations centrées (dépassent en haut/bas)
    
    // 1) Positionner les sections côte à côte (sans espacement)
    for (size_t i = 0; i < sectionPanels.size(); ++i)
    {
        sectionPanels[i]->setBounds(x, ySection, SECTION_WIDTH, SECTION_HEIGHT);
        x += SECTION_WIDTH;
    }
    
    // 2) Positionner les modulations à cheval sur les frontières entre sections
    x = SECTION_WIDTH - (MODULATION_WIDTH / 2);  // Commence au milieu de la frontière S1-S2
    for (size_t i = 0; i < modulationPanels.size(); ++i)
    {
        modulationPanels[i]->setBounds(x, yModulation, MODULATION_WIDTH, MODULATION_HEIGHT);
        modulationPanels[i]->toFront(false);  // Les modulations au premier plan
        x += SECTION_WIDTH;
    }
    
    // Mettre à jour la taille du conteneur
    int totalWidth = static_cast<int>(sectionPanels.size()) * SECTION_WIDTH;
    overlayContainer->setSize(juce::jmax(totalWidth, 100), totalHeight);
}

void OverviewContentArea::clearAllPanels()
{
    sectionPanels.clear();
    modulationPanels.clear();
    if (overlayContainer)
        overlayContainer->removeAllChildren();
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

// Méthodes de sélection supprimées - maintenant pilotées par l'état central

void OverviewContentArea::setupViewport()
{
    overlayContainer->setSize(100, PREFERRED_HEIGHT);
    
    // Configuration du viewport avec scrollbar horizontale uniquement
    viewport.setViewedComponent(overlayContainer.get(), false);
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
    if (!clickedPanel || !appController)
        return;
        
    // ⚠️ IMPORTANT : getUserData() retourne l'ID, pas l'index !
    int elementId = clickedPanel->getUserData();
    auto contentType = clickedPanel->getContentType();
    
    if (contentType == PanelContentType::Section)
    {
        // Convertir l'ID en index pour selectSection()
        auto& piece = appController->getPiece();
        int sectionIndex = piece.getSectionIndexById(elementId);
        
        if (sectionIndex >= 0)
        {
            appController->selectSection(sectionIndex);
        }
        else
        {
            DBG("[OverviewContentArea] Section avec ID " << elementId << " introuvable");
        }
    }
    else if (contentType == PanelContentType::Modulation)
    {
        // Convertir l'ID en index pour selectModulation()
        auto& piece = appController->getPiece();
        int modulationIndex = piece.getModulationIndexById(elementId);
        
        if (modulationIndex >= 0)
        {
            appController->selectModulation(modulationIndex);
        }
        else
        {
            DBG("[OverviewContentArea] Modulation avec ID " << elementId << " introuvable");
        }
    }
}

void OverviewContentArea::updateSelectionHighlight()
{
    if (!selectionState.isValid())
        return;
    
    // Obtenir l'ID de l'élément sélectionné depuis l'état central
    juce::String selectedElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String selectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    
    // Parcourir les panels de section
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
    
    // Parcourir les panels de modulation
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