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
    
    // Créer le ModulationEditor mais le garder caché au début
    modulationEditor = std::make_unique<ModulationEditor>();
    addChildComponent(*modulationEditor); // addChildComponent = invisible par défaut
}

ProgressionArea::~ProgressionArea()
{
    // Nettoyer les listeners du ValueTree
    if (selectionState.isValid())
        selectionState.removeListener(this);
    
    if (modelState.isValid())
        modelState.removeListener(this);
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
    
    // Positionner les trois vues dans toute la zone disponible
    // Seule la vue visible sera affichée
    welcomeView.setBounds(area);
    if (sectionEditor)
        sectionEditor->setBounds(area);
    if (modulationEditor)
        modulationEditor->setBounds(area);
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
        
        // S'abonner au modèle pour détecter les suppressions de sections
        modelState = appController->getState();
        if (modelState.isValid())
        {
            modelState.addListener(this);
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
        
        if (modelState.isValid())
        {
            modelState.removeListener(this);
            modelState = juce::ValueTree();
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

void ProgressionArea::valueTreeChildRemoved(juce::ValueTree& parentTree, 
                                           juce::ValueTree& childWhichHasBeenRemoved, 
                                           int indexFromWhichChildWasRemoved)
{
    // Quand une section ou modulation est supprimée du modèle, rafraîchir l'affichage
    // Cela permet de mettre à jour le titre (ex: "Section 3" → "Section 2" si l'index change)
    if (parentTree == modelState)
    {
        // Rafraîchir l'affichage pour recalculer les titres avec les nouveaux indices
        updateContentBasedOnSelection();
        
        // Forcer le rafraîchissement du titre même si la sélection n'a pas changé
        // (l'index de la section peut avoir changé sans que son ID change)
        if (sectionEditor && sectionEditor->isVisible())
        {
            sectionEditor->refreshTitle();
        }
    }
}

void ProgressionArea::updateContentBasedOnSelection()
{
    if (!selectionState.isValid() || !sectionEditor || !modulationEditor)
        return;
    
    juce::String selectedElementId = selectionState.getProperty(ContextIdentifiers::selectedElementId, "");
    juce::String selectionType = selectionState.getProperty(ContextIdentifiers::selectionType, "None");
    
    // Gérer les trois cas de sélection
    if (selectionType == "Section" && !selectedElementId.isEmpty())
    {
        // Afficher SectionEditor et cacher les autres
        welcomeView.setVisible(false);
        modulationEditor->setVisible(false);
        sectionEditor->setSectionToEdit(selectedElementId);

        // ✅ CORRIGÉ: Extraire l'ID et utiliser getSectionById() au lieu de getSection(index)
        int sectionId = selectedElementId.getTrailingIntValue();
        if (appController != nullptr && sectionId >= 0)
        {
            auto& piece = appController->getPiece();
            // Vérifier que la section existe avant de la récupérer
            int sectionIndex = piece.getSectionIndexById(sectionId);
            if (sectionIndex >= 0)
            {
                auto section = piece.getSectionById(sectionId);
                sectionEditor->setSectionState(section.getState());
            }
            else
            {
                sectionEditor->setSectionState(juce::ValueTree());
            }
        }
        else
        {
            sectionEditor->setSectionState(juce::ValueTree());
        }
        sectionEditor->setVisible(true);
    }
    else if (selectionType == "Modulation" && !selectedElementId.isEmpty())
    {
        // Afficher ModulationEditor et cacher les autres
        welcomeView.setVisible(false);
        sectionEditor->setVisible(false);
        sectionEditor->setSectionToEdit(""); // Clear l'éditeur de section
        sectionEditor->setSectionState(juce::ValueTree());
        
        // ✅ CORRIGÉ: Extraire l'ID et utiliser getModulationById() au lieu de getModulation(index)
        modulationEditor->setModulationToEdit(selectedElementId);
        int modulationId = selectedElementId.getTrailingIntValue();
        if (appController != nullptr && modulationId >= 0)
        {
            auto& piece = appController->getPiece();
            // Vérifier que la modulation existe avant de la récupérer
            int modulationIndex = piece.getModulationIndexById(modulationId);
            if (modulationIndex >= 0)
            {
                auto modulation = piece.getModulationById(modulationId);
                modulationEditor->setModulationState(modulation.getState());
            }
            else
            {
                modulationEditor->setModulationState(juce::ValueTree());
            }
        }
        else
        {
            modulationEditor->setModulationState(juce::ValueTree());
        }
        
        modulationEditor->setVisible(true);
    }
    else
    {
        // Cas par défaut : afficher WelcomeView et cacher les éditeurs
        sectionEditor->setVisible(false);
        sectionEditor->setSectionToEdit(""); // Clear l'éditeur de section
        sectionEditor->setSectionState(juce::ValueTree());
        modulationEditor->setVisible(false);
        modulationEditor->setModulationToEdit(""); // Clear l'éditeur de modulation
        modulationEditor->setModulationState(juce::ValueTree());
        welcomeView.setVisible(true);
    }
} 