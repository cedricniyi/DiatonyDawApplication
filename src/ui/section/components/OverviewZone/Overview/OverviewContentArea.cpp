#include "OverviewContentArea.h"
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"
#include "model/ModelIdentifiers.h"

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
    if (scrollableContent)
    {
        scrollableContent->clearAllPanels();
    }
    
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
    if (!scrollableContent)
        return;
    
    // Récupérer l'ID de la section (identifiant unique permanent)
    int sectionId = sectionNode.getProperty(ModelIdentifiers::id, -1);
    
    // Création du panel avec couleur bleue
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    
    // Définir le type de contenu
    newPanel->setContentType(PanelContentType::Section);
    
    // Stocker l'ID pour l'identification (utilisé par onPanelClicked pour la conversion ID→Index)
    newPanel->setUserData(sectionId);
    
    // ✅ AFFICHAGE : Utiliser l'INDEX (position visuelle 1, 2, 3...) PAS l'ID
    newPanel->setDisplayText(juce::String(sectionIndex + 1));
    newPanel->setShowText(true);
    
    // Référence au panel avant transfert de propriété
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic gauche (sélection)
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Configuration du callback clic droit (suppression)
    // ✅ IMPORTANT : On utilise l'ID pour convertir en index au moment du clic
    // car l'index peut changer si d'autres sections sont supprimées avant celle-ci
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
    
    // Dimensions du panel
    constexpr int SECTION_WIDTH = 40;
    constexpr int SECTION_HEIGHT = 25;
    
    // Ajout au contenu scrollable
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component), SECTION_WIDTH, SECTION_HEIGHT);
    
    // Sélection automatique si demandée
    if (autoSelect && appController && sectionIndex >= 0)
    {
        appController->selectSection(sectionIndex);
    }
    
    updateVisibility();
    resized();
}

void OverviewContentArea::createPanelForModulation(const juce::ValueTree& modulationNode)
{
    if (!scrollableContent)
        return;
    
    // Récupérer l'ID de la modulation
    int modulationId = modulationNode.getProperty(ModelIdentifiers::id, -1);
    
    if (modulationId < 0)
        return;
    
    // Création du panel avec couleur verte pour les modulations
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::green);
    
    // Définir le type de contenu
    newPanel->setContentType(PanelContentType::Modulation);
    
    // Stocker l'ID de la modulation (plus besoin d'offset !)
    newPanel->setUserData(modulationId);
    
    // ✅ OPTIONNEL : Désactiver l'affichage du texte pour les modulations
    newPanel->setShowText(false);  // Les modulations ne sont pas numérotées par défaut
    
    // Si tu veux quand même afficher l'ID de la modulation, décommente la ligne suivante :
    // newPanel->setDisplayText(juce::String(modulationId+1));
    
    // Référence au panel avant transfert de propriété
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic (utilise onPanelClicked qui gère la conversion ID→Index)
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // ✅ DÉCOUPLÉ : OverviewContentArea décide des dimensions selon la logique métier
    constexpr int MODULATION_WIDTH = 30;  // 5px plus large que carré comme demandé
    constexpr int MODULATION_HEIGHT = 25;
    
    // Ajout au contenu scrollable avec dimensions explicites
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component), MODULATION_WIDTH, MODULATION_HEIGHT);
    
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
    int panelId = nextPanelId++;
    newPanel->setUserData(panelId);
    
    // ✅ CONFIGURABLE : Affichage de l'ID sur le panel (fallback)
    newPanel->setDisplayText(juce::String(panelId));
    newPanel->setShowText(true);  // Par défaut activé pour rétrocompatibilité
    
    // La sélection est maintenant gérée centralement - pas besoin de désélection manuelle
    
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // Configuration du callback de clic
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // Ajout et sélection du nouveau panel
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component));
    
    // La sélection visuelle sera gérée par updateSelectionHighlight()
    
    updateVisibility();
    resized();
}

void OverviewContentArea::clearAllPanels()
{
    if (scrollableContent)
    {
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

// Méthodes de sélection supprimées - maintenant pilotées par l'état central

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
    if (!selectionState.isValid() || !scrollableContent)
        return;
    
    // Obtenir l'ID de l'élément sélectionné depuis l'état central
    juce::String selectedElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String selectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    
    // Parcourir tous les panels pour mettre à jour leur état visuel
    for (int i = 0; i < scrollableContent->getNumChildComponents(); ++i)
    {
        if (auto* panel = dynamic_cast<ButtonColoredPanel*>(scrollableContent->getChildComponent(i)))
        {
            int userData = static_cast<int>(panel->getUserData());
            PanelContentType panelType = panel->getContentType();
            
            // Déterminer si ce panel doit être sélectionné
            bool shouldBeSelected = false;
            
            if (selectionType == "Section" && panelType == PanelContentType::Section)
            {
                // Panel de section
                juce::String expectedElementId = "Section_" + juce::String(userData);
                shouldBeSelected = (expectedElementId == selectedElementId);
            }
            else if (selectionType == "Modulation" && panelType == PanelContentType::Modulation)
            {
                // Panel de modulation
                juce::String expectedElementId = "Modulation_" + juce::String(userData);
                shouldBeSelected = (expectedElementId == selectedElementId);
            }
            
            // Mettre à jour l'état visuel du panel
            panel->setSelected(shouldBeSelected);
        }
    }
} 