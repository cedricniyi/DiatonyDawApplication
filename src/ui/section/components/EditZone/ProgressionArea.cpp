#include "ProgressionArea.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"

//==============================================================================
ProgressionArea::ProgressionArea() 
    : ColoredPanel(juce::Colours::white)
{
    // Ajouter la vue de bienvenue (visible par défaut)
    addAndMakeVisible(welcomeView);
    
    // Créer le SectionEditor mais le garder caché au début
    sectionEditor = std::make_unique<SectionEditor>();
    addChildComponent(*sectionEditor); // addChildComponent = invisible par défaut
}

ProgressionArea::~ProgressionArea()
{
    // Nettoyer les listeners du ValueTree
    if (selectionState.isValid())
        selectionState.removeListener(this);
}

void ProgressionArea::paint(juce::Graphics& g)
{
    // Dessiner le fond coloré via ColoredPanel
    ColoredPanel::paint(g);
}

void ProgressionArea::resized()
{
    // Appliquer le padding à toute la zone
    auto area = getLocalBounds().reduced(20, 10);
    
    // Positionner les deux vues dans toute la zone disponible
    // Seule la vue visible sera affichée
    welcomeView.setBounds(area);
    if (sectionEditor)
        sectionEditor->setBounds(area);
}

// === DÉCOUVERTE DE SERVICE ===

void ProgressionArea::parentHierarchyChanged()
{
    ColoredPanel::parentHierarchyChanged();
    findAppController();
}

void ProgressionArea::findAppController()
{
    // Recherche de l'AppController via la hiérarchie des composants
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        
        // S'abonner à l'état de sélection centralisé
        selectionState = appController->getSelectionState();
        if (selectionState.isValid())
        {
            selectionState.addListener(this);
            // Mettre à jour immédiatement le contenu selon l'état actuel
            updateContentBasedOnSelection();
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

// === GESTION DE LA SÉLECTION RÉACTIVE ===

void ProgressionArea::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                              const juce::Identifier& property)
{
    // Réagir aux changements de sélection depuis l'état central
    if (property == ContextIdentifiers::selectedElementId && treeWhosePropertyHasChanged == selectionState)
    {
        updateContentBasedOnSelection();
    }
}

void ProgressionArea::updateContentBasedOnSelection()
{
    if (!selectionState.isValid() || !sectionEditor)
        return;
    
    juce::String selectedElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String selectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    
    // Si un ID de section est valide, cacher WelcomeView et montrer SectionEditor
    if (selectionType == "Section" && !selectedElementId.isEmpty())
    {
        welcomeView.setVisible(false);
        sectionEditor->setSectionToEdit(selectedElementId);
        sectionEditor->setVisible(true);
    }
    else
    {
        // Si l'ID est invalide (sélection vide), cacher SectionEditor et montrer WelcomeView
        if (sectionEditor->isVisible())
        {
            sectionEditor->setVisible(false);
            sectionEditor->setSectionToEdit(""); // Clear l'éditeur
        }
        welcomeView.setVisible(true);
    }
} 