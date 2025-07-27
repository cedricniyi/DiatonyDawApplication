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
    // Configuration de base
    setAlpha(1.0f);
    
    // Configuration des composants
    setupViewport();
    setupEmptyLabel();
    
    // Mettre à jour la visibilité initiale
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
    // Chercher AudioPluginAudioProcessorEditor dans la hiérarchie des parents
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        DBG("OverviewContentArea: AppController découvert via la hiérarchie des composants");
        
        // Une fois AppController trouvé, connecter au ValueTree du modèle
        auto modelTree = appController->getPiece().getState();
        setModelState(modelTree);
    }
    else
    {
        appController = nullptr;
        DBG("OverviewContentArea: AppController non trouvé dans la hiérarchie");
    }
}

// === ARCHITECTURE RÉACTIVE ===

void OverviewContentArea::setModelState(juce::ValueTree& newModelState)
{
    // Déconnecter l'ancien listener si nécessaire
    if (modelState.isValid())
        modelState.removeListener(this);
        
    // Connecter au nouveau ValueTree
    modelState = newModelState;
    modelState.addListener(this);
    
    // Rafraîchir l'UI depuis le modèle
    refreshFromModel();
    
    DBG("OverviewContentArea: Connecté au ValueTree du modèle et UI rafraîchie");
}

void OverviewContentArea::refreshFromModel()
{
    if (!modelState.isValid())
        return;
        
    // Compter les sections dans le modèle
    int sectionCount = 0;
    for (int i = 0; i < modelState.getNumChildren(); ++i)
    {
        if (modelState.getChild(i).hasType(Identifiers::SECTION))
            sectionCount++;
    }
    
    DBG("OverviewContentArea: Modèle contient " << sectionCount << " sections");
    
    // === CRÉER LES PANELS POUR LES SECTIONS EXISTANTES ===
    if (scrollableContent && sectionCount > 0)
    {
        // Vider d'abord les panels existants
        selectedPanel = nullptr;
        scrollableContent->clearAllPanels();
        
        // Créer un panel pour chaque section existante
        for (int i = 0; i < modelState.getNumChildren(); ++i)
        {
            auto child = modelState.getChild(i);
            if (child.hasType(Identifiers::SECTION))
            {
                createPanelForSection(child, false); // false = ne pas sélectionner automatiquement
            }
        }
        
        DBG("OverviewContentArea: " << sectionCount << " panels créés pour les sections existantes");
    }
    
    // Mettre à jour la visibilité
    updateVisibility();
}

// =================================================================================
// ValueTree::Listener interface - Réactivité aux changements du modèle

void OverviewContentArea::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                  const juce::Identifier& property)
{
    DBG("OverviewContentArea: Propriété changée - " << property.toString() << 
        " dans " << treeWhosePropertyHasChanged.getType().toString());
    
    // Réagir aux changements de propriétés si nécessaire
    if (property == Identifiers::name)
    {
        // Une section a changé de nom, mettre à jour l'affichage
        refreshFromModel();
    }
}

void OverviewContentArea::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    DBG("OverviewContentArea: Enfant ajouté - " << childWhichHasBeenAdded.getType().toString() << 
        " dans " << parentTree.getType().toString());
    
    if (childWhichHasBeenAdded.hasType(Identifiers::SECTION))
    {
        handleSectionAdded(childWhichHasBeenAdded);
    }
}

void OverviewContentArea::valueTreeChildRemoved(juce::ValueTree& parentTree, 
                                               juce::ValueTree& childWhichHasBeenRemoved, 
                                               int indexFromWhichChildWasRemoved)
{
    DBG("OverviewContentArea: Enfant supprimé à l'index " << indexFromWhichChildWasRemoved << 
        " de type " << childWhichHasBeenRemoved.getType().toString());
    
    if (childWhichHasBeenRemoved.hasType(Identifiers::SECTION))
    {
        handleSectionRemoved();
    }
}

void OverviewContentArea::valueTreeChildOrderChanged(juce::ValueTree& parentTreeWhoseChildrenHaveMoved, 
                                                    int oldIndex, int newIndex)
{
    DBG("OverviewContentArea: Ordre des enfants changé - " << oldIndex << " -> " << newIndex);
    refreshFromModel();
}

void OverviewContentArea::valueTreeParentChanged(juce::ValueTree& treeWhoseParentHasChanged)
{
    DBG("OverviewContentArea: Parent changé pour " << treeWhoseParentHasChanged.getType().toString());
}

// === MÉTHODES POUR L'ARCHITECTURE RÉACTIVE ===

void OverviewContentArea::handleSectionAdded(const juce::ValueTree& sectionNode)
{
    DBG("OverviewContentArea: Nouvelle section détectée - " << 
        sectionNode.getProperty(Identifiers::name, "Sans nom").toString());
    
    // Créer et sélectionner automatiquement le panel pour la nouvelle section
    createPanelForSection(sectionNode, true); // true = sélectionner automatiquement
}

void OverviewContentArea::handleSectionRemoved()
{
    DBG("OverviewContentArea: Section supprimée - reconstruction complète des panels");
    
    // Reconstruire tous les panels depuis le modèle
    refreshFromModel();
}

void OverviewContentArea::createPanelForSection(const juce::ValueTree& sectionNode, bool autoSelect)
{
    if (!scrollableContent)
        return;
    
    // Créer un nouveau ButtonColoredPanel avec une couleur bleue
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    
    // Associer l'ID de la section du modèle au panel
    auto sectionId = sectionNode.getProperty(Identifiers::id, nextPanelId++);
    newPanel->setUserData(sectionId);
    
    // Optionnel : afficher le nom de la section sur le panel
    auto sectionName = sectionNode.getProperty(Identifiers::name, "Section").toString();
    DBG("OverviewContentArea: Création du panel pour la section '" << sectionName << "' (ID: " << sectionId.toString() << ")");
    
    // Garder une référence au nouveau panel avant de le transférer
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // ✅ Utiliser le système onClick natif de JUCE !
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // L'ajouter au contenu scrollable (conversion vers juce::Component)
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component));
    
    // Gestion de la sélection si demandée
    if (autoSelect)
    {
        // Désélectionner l'ancien panel sélectionné
        if (selectedPanel)
        {
            selectedPanel->setSelected(false);
        }
        
        // Sélectionner le nouveau panel
        newPanelPtr->setSelected(true);
        selectedPanel = newPanelPtr;
    }
    
    // Mettre à jour la visibilité et forcer le redimensionnement
    updateVisibility();
    resized();
    
    DBG("OverviewContentArea: Panel visuel créé et ajouté pour la section '" << sectionName << 
        "' (sélectionné: " << (autoSelect ? "oui" : "non") << ")");
}

// === MÉTHODES UI EXISTANTES ===

void OverviewContentArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void OverviewContentArea::resized()
{
    ColoredPanel::resized();
    auto contentArea = getLocalBounds().reduced(CONTENT_MARGIN);
    
    // Configurer les bounds des composants
    viewport.setBounds(contentArea);
    emptyLabel.setBounds(contentArea);
    
    // Configurer le contenu scrollable
    if (scrollableContent)
    {
        scrollableContent->setSize(scrollableContent->getWidth(), PREFERRED_HEIGHT);
    }
}

void OverviewContentArea::addSmallPanel()
{
    if (!scrollableContent)
        return;
    
    // === UTILISATION D'APPCONTROLLER VIA ARCHITECTURE RÉACTIVE ===
    if (appController != nullptr)
    {
        // Au lieu de mettre à jour manuellement l'UI, on utilise AppController
        // L'UI se mettra à jour automatiquement via les ValueTree::Listener callbacks
        appController->addNewSection("Nouvelle Section");
        DBG("OverviewContentArea: Nouvelle section ajoutée via AppController - l'UI se mettra à jour automatiquement");
        
        // Pas besoin de créer manuellement le panel - le listener le fera !
        return;
    }
    else
    {
        DBG("OverviewContentArea: AppController non disponible, création manuelle du panel");
    }
    
    // === FALLBACK : CODE EXISTANT POUR CRÉER MANUELLEMENT UN PANEL ===
    // Ce code ne sera exécuté que si AppController n'est pas disponible
    
    // Créer un nouveau ButtonColoredPanel avec une couleur bleue
    auto newPanel = std::make_unique<ButtonColoredPanel>(juce::Colours::blue);
    
    // Associer des données utilisateur (ex: ID de progression)
    newPanel->setUserData(nextPanelId++);
    
    // Désélectionner l'ancien panel sélectionné
    if (selectedPanel)
    {
        selectedPanel->setSelected(false);
    }
    
    // Garder une référence au nouveau panel avant de le transférer
    ButtonColoredPanel* newPanelPtr = newPanel.get();
    
    // ✅ Utiliser le système onClick natif de JUCE !
    newPanel->onClick = [this, newPanelPtr]() {
        this->onPanelClicked(newPanelPtr);
    };
    
    // L'ajouter au contenu scrollable (conversion vers juce::Component)
    std::unique_ptr<juce::Component> component(newPanel.release());
    scrollableContent->addSmallPanel(std::move(component));
    
    // Sélectionner automatiquement le nouveau panel
    newPanelPtr->setSelected(true);
    selectedPanel = newPanelPtr;
    
    // Mettre à jour la visibilité
    updateVisibility();
    
    // Forcer le redimensionnement pour s'assurer du layout
    resized();
}

void OverviewContentArea::clearAllPanels()
{
    if (scrollableContent)
    {
        selectedPanel = nullptr; // Réinitialiser la sélection
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
    // Donner une taille initiale au contenu scrollable
    scrollableContent->setSize(100, PREFERRED_HEIGHT);
    
    // Configuration du viewport
    viewport.setViewedComponent(scrollableContent.get(), false); // false = on gère nous-mêmes la lifetime
    
    // Configurer les scrollbars : seulement horizontale, et seulement quand nécessaire
    viewport.setScrollBarsShown(false, true, false, false);
    
    // Positionner la scrollbar en bas
    viewport.setScrollBarPosition(true, true);
    
    // Ajouter le viewport à notre interface
    addAndMakeVisible(viewport);
}

void OverviewContentArea::setupEmptyLabel()
{
    // Configuration du label pour le texte par défaut
    emptyLabel.setText(juce::String::fromUTF8("Aucun progression n'a été ajouté à la pièce"), juce::dontSendNotification);
    emptyLabel.setJustificationType(juce::Justification::centred);
    emptyLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withAlpha(0.7f));
    
    // Utiliser FontManager pour la font
    juce::SharedResourcePointer<FontManager> fontManager;
    auto fontOptions = fontManager->getSFProText(18.0f, FontManager::FontWeight::Regular);
    emptyLabel.setFont(juce::Font(fontOptions));
    
    // Ajouter le label
    addAndMakeVisible(emptyLabel);
}

void OverviewContentArea::updateVisibility()
{
    bool hasContentNow = hasContent();
    
    // Debug temporaire
    // DBG("updateVisibility: hasContent=" + juce::String(hasContentNow ? "true" : "false"));
    // DBG("  - numPanels=" + juce::String(scrollableContent ? scrollableContent->getNumPanels() : -1));
    
    viewport.setVisible(hasContentNow);
    emptyLabel.setVisible(!hasContentNow);
    
    // Debug temporaire
    // DBG("  - viewport visible=" + juce::String(viewport.isVisible() ? "true" : "false"));
    // DBG("  - label visible=" + juce::String(emptyLabel.isVisible() ? "true" : "false"));
}

void OverviewContentArea::onPanelClicked(ButtonColoredPanel* clickedPanel)
{
    if (!clickedPanel)
        return;
        
    auto panelId = clickedPanel->getUserData().toString();
    
    // Debug temporaire pour voir les clics
    DBG(juce::String::fromUTF8("Panel cliqué - ID: ") + panelId);
    
    // Gestion de la sélection
    if (selectedPanel != clickedPanel)
    {
        // Désélectionner l'ancien panel
        if (selectedPanel)
        {
            selectedPanel->setSelected(false);
        }
        
        // Sélectionner le nouveau panel
        clickedPanel->setSelected(true);
        selectedPanel = clickedPanel;
        
        DBG(juce::String::fromUTF8("Panel sélectionné - ID: ") + panelId);
    }
    
    // TODO: Autres actions possibles
    // - Déclencher la lecture de la progression
    // - Ouvrir un éditeur de progression
    // - Envoyer un event à un listener parent
} 