#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"
#include "ui/animation/AnimationManager.h"
#include "ui/animation/SlidingPanelAnimator.h"
#include "ui/RootAnimator.h"
#include "controller/AppController.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      appState(UIStateIdentifiers::APP_STATE)
{
    // Création du contrôleur principal de l'application.
    appController = std::make_unique<AppController>(juce::String::fromUTF8("Ma Pièce"));

    // Initialisation de l'état de l'interface (UI State).
    appState.setProperty(UIStateIdentifiers::footerExpanded, false, nullptr);
    appState.setProperty(UIStateIdentifiers::dockVisible, false, nullptr);
    appState.setProperty(UIStateIdentifiers::interactivePianoVisible, false, nullptr);
    appState.setProperty(UIStateIdentifiers::historyPanelVisible, false, nullptr);
    
    #if DEBUG
        // Attache un logger pour déboguer les changements du ValueTree (mode non-récursif pour l'UI State).
        appState.addListener(&appStateLogger);
    
        // Attache un logger RÉCURSIF à l'état du modèle (la pièce) pour suivre ses changements à tous les niveaux.
        // Cela permet de tracker les accords ajoutés dans PIECE → SECTION → PROGRESSION → CHORD
        pieceStateLogger.attachTo(appController->getState());
        
        // Attache un logger à l'état de sélection (contexte applicatif) pour suivre ses changements (mode non-récursif).
        appController->getSelectionState().addListener(&selectionStateLogger);

        // Test: Modifions une propriété de la pièce pour vérifier que le logger fonctionne.
        appController->getState().setProperty("testProperty", "Hello from Piece!", nullptr);
    #endif

    // Création et configuration du composant de contenu principal.
    mainContent = std::make_unique<MainContentComponent>();
    mainContent->setAppState(appState); // Lie l'état de l'UI au contenu.
    mainContent->setSelectionState(appController->getSelectionState()); // Pour les notifications de génération
    // TODO: Connecter au Model State (données métier).
    // mainContent->setModelState(appController->getPiece().getState());
    addAndMakeVisible(*mainContent);

    // Configuration des contraintes de taille de la fenêtre.
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setMinimumWidth(1200);
    constrainer->setMaximumWidth(1750);
    constrainer->setMinimumHeight(500);  // Hauteur fixe
    constrainer->setMaximumHeight(500);  // Hauteur fixe (min == max)
    // Pas d'aspect ratio → la hauteur reste à 500 peu importe la largeur
    setConstrainer(constrainer.get());
    
    setSize(1200, 500);
    
    #if DEBUG
    // Permettre la réception des événements clavier pour les raccourcis de dev
    setWantsKeyboardFocus(true);
    #endif

    // Initialisation des animateurs.
    // RootAnimator gère le layout flexible principal.
    rootAnimator = std::make_unique<RootAnimator>(*mainContent, *AnimationManager::getInstance());
    
    // FooterAnimator (générique) - anime le panneau footer
    footerAnimator = std::make_unique<SlidingPanelAnimator>(
        mainContent->getFooterPanel().getGridFractionRef(),
        mainContent->getFooterPanel().getFadingComponent(),
        [this]() { mainContent->getFooterPanel().resized(); },
        *AnimationManager::getInstance()
    );
    
    // Connecter le callback du FooterPanel au SlidingPanelAnimator
    mainContent->getFooterPanel().onInteractivePianoVisibilityChange = [this](bool visible) {
        footerAnimator->setVisible(visible);
    };
    
    // HistoryAnimator (générique) - anime le panneau History latéral
    historyAnimator = std::make_unique<SlidingPanelAnimator>(
        mainContent->getHistoryPanel().getWidthFractionRef(),
        mainContent->getHistoryPanel().getFadingComponent(),
        [this]() { mainContent->resized(); },
        *AnimationManager::getInstance()
    );
    
    // Connecter le callback du HistoryPanel au SlidingPanelAnimator
    mainContent->getHistoryPanel().onVisibilityChange = [this](bool visible) {
        historyAnimator->setVisible(visible);
    };

    // L'architecture est réactive : l'UI écoute les changements du modèle (ValueTree)
    // et se met à jour automatiquement.

    #if DEBUG
        // Bloc de test pour le débogage du ValueTree avec LLDB.
        // Placez un point d'arrêt ici pour inspecter 'appState'.
        appState.setProperty("test_debug", "LLDB_test_value", nullptr);
        auto testChild = juce::ValueTree("TestChild");
        testChild.setProperty("child_prop", 42, nullptr);
        appState.appendChild(testChild, nullptr);
    #endif
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

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{   
    // La peinture est déléguée au MainContentComponent qui remplit toute la zone.
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Le MainContentComponent occupe toute la surface de l'éditeur.
    if (mainContent)
        mainContent->setBounds(getLocalBounds());
}

// === DÉCOUVERTE DE SERVICE ===

AppController& AudioPluginAudioProcessorEditor::getAppController()
{
    jassert(appController != nullptr);
    return *appController;
}