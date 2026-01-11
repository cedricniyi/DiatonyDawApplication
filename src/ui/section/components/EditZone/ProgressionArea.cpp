#include "ProgressionArea.h"
#include "utils/FontManager.h"
#include "controller/AppController.h"
#include "ui/PluginEditor.h"

ProgressionArea::ProgressionArea()
{
    setOpaque(false);
    
    addAndMakeVisible(welcomeView);
    
    // SectionEditor créé mais non visible initialement
    sectionEditor = std::make_unique<SectionEditor>();
    addChildComponent(*sectionEditor);
    
    // ModulationEditor créé mais non visible initialement
    modulationEditor = std::make_unique<ModulationEditor>();
    addChildComponent(*modulationEditor);
}

ProgressionArea::~ProgressionArea()
{
    if (selectionState.isValid())
        selectionState.removeListener(this);
    
    if (modelState.isValid())
        modelState.removeListener(this);
}

void ProgressionArea::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    juce::Path panelPath;
    panelPath.addRoundedRectangle(bounds, cornerRadius);
    
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillPath(panelPath);
    
    g.setColour(juce::Colours::grey.withAlpha(0.4f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, static_cast<float>(borderThickness));
}

void ProgressionArea::resized()
{
    // Les enfants remplissent tout l'espace (pas de padding)
    auto area = getLocalBounds();
    
    // Positionner les composants enfants, seule la vue visible sera affichée
    welcomeView.setBounds(area);
    if (sectionEditor)
        sectionEditor->setBounds(area);
    if (modulationEditor)
        modulationEditor->setBounds(area);
}

void ProgressionArea::parentHierarchyChanged()
{
    juce::Component::parentHierarchyChanged();
    findAppController();
}

void ProgressionArea::findAppController()
{
    auto* pluginEditor = findParentComponentOfClass<AudioPluginAudioProcessorEditor>();
    
    if (pluginEditor != nullptr)
    {
        appController = &pluginEditor->getAppController();
        
        // S'abonner à l'état de sélection centralisé
        selectionState = appController->getSelectionState();
        if (selectionState.isValid())
        {
            selectionState.addListener(this);
            updateContentBasedOnSelection();
        }
        
        // S'abonner au modèle pour détecter les suppressions de sections
        modelState = appController->getState();
        if (modelState.isValid())
            modelState.addListener(this);
    }
    else
    {
        appController = nullptr;
        
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
    if (property == ContextIdentifiers::selectedElementId && treeWhosePropertyHasChanged == selectionState)
        updateContentBasedOnSelection();
}

void ProgressionArea::valueTreeChildRemoved(juce::ValueTree& parentTree, 
                                           juce::ValueTree&, 
                                           int)
{
    // Rafraîchir le titre quand une section est supprimée (l'index peut changer)
    if (parentTree == modelState)
    {
        updateContentBasedOnSelection();
        
        if (sectionEditor && sectionEditor->isVisible())
            sectionEditor->refreshTitle();
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
        sectionEditor->setSectionToEdit("");
        sectionEditor->setSectionState(juce::ValueTree());
        
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
        sectionEditor->setSectionToEdit("");
        sectionEditor->setSectionState(juce::ValueTree());
        modulationEditor->setVisible(false);
        modulationEditor->setModulationToEdit("");
        modulationEditor->setModulationState(juce::ValueTree());
        welcomeView.setVisible(true);
    }
}
