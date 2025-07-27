#include "OverviewContentArea.h"
#include "ui/extra/Button/ButtonColoredPanel.h"
#include "utils/FontManager.h"

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