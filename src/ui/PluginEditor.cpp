#include "PluginEditor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "melatonin_inspector/melatonin_inspector.h"
#include "utils/FontManager.h"
#include "ui/animation/AnimationManager.h"
#include "ui/RootAnimator.h"
#include "ui/footer/animator/FooterAnimator.h"
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
    // TODO: Connecter au Model State (données métier).
    // mainContent->setModelState(appController->getPiece().getState());
    addAndMakeVisible(*mainContent);

    // Configuration des contraintes de taille de la fenêtre.
    constrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
    constrainer->setSizeLimits(1300, 569, 1694, 847);
    constrainer->setFixedAspectRatio(1500.0 / 750.0);
    setConstrainer(constrainer.get());
    
    setSize(1500, 750);

    // Initialisation des animateurs.
    // RootAnimator gère le layout flexible principal.
    rootAnimator = std::make_unique<RootAnimator>(*mainContent, *AnimationManager::getInstance());
    
    // FooterAnimator gère les animations spécifiques au panneau de pied de page.
    footerAnimator = std::make_unique<FooterAnimator>(mainContent->getFooterPanel(), *AnimationManager::getInstance());

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