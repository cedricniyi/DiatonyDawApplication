#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"
#include "ui/animation/AnimationManager.h"
#include "ui/animation/SlidingPanelAnimator.h"
#include "controller/AppController.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      appState(UIStateIdentifiers::APP_STATE)
{
    appController = std::make_unique<AppController>(juce::String::fromUTF8("PIECE"));

    appState.setProperty(UIStateIdentifiers::dockVisible, false, nullptr);
    appState.setProperty(UIStateIdentifiers::historyPanelVisible, false, nullptr);
    
    #if DEBUG
        // Attache un logger pour déboguer les changements du ValueTree.
        appState.addListener(&appStateLogger);
    
        // Attache un logger RÉCURSIF à l'état du modèle (la pièce) pour suivre ses changements à tous les niveaux.
        // Cela permet de tracker les accords ajoutés dans PIECE → SECTION → PROGRESSION → CHORD
        pieceStateLogger.attachTo(appController->getState());
        appController->getSelectionState().addListener(&selectionStateLogger);
    #endif

    mainContent = std::make_unique<MainContentComponent>();
    mainContent->setAppState(appState);
    mainContent->setSelectionState(appController->getSelectionState());
    addAndMakeVisible(*mainContent);

    // Configuration des contraintes de taille de la fenêtre.
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setMinimumWidth(1200);
    constrainer->setMaximumWidth(1750);
    constrainer->setMinimumHeight(500);
    constrainer->setMaximumHeight(500);
    setConstrainer(constrainer.get());
    
    setSize(1200, 500);
    
    // HistoryAnimator - anime le panneau History latéral
    historyAnimator = std::make_unique<SlidingPanelAnimator>(
        mainContent->getHistoryPanel().getWidthFractionRef(),
        mainContent->getHistoryPanel().getFadingComponent(),
        [this]() { mainContent->resized(); },
        *AnimationManager::getInstance()
    );
    
    mainContent->getHistoryPanel().onVisibilityChange = [this](bool visible) {
        historyAnimator->setVisible(visible);
    };
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    #if DEBUG
        // Détacher le logger récursif du modèle avant destruction
        if (appController)
            pieceStateLogger.detachFrom(appController->getState());
    #endif
    
    setLookAndFeel(nullptr);
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{   
    // Le rôle de la méthode paint est géré par le MainContentComponent
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Le MainContentComponent occupe toute la surface de l'éditeur.
    if (mainContent)
        mainContent->setBounds(getLocalBounds());
}

AppController& AudioPluginAudioProcessorEditor::getAppController()
{
    jassert(appController != nullptr);
    return *appController;
}
