#include "PluginEditor.h"
#include "components/DiatonyAlertWindow.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), titleBounds(0, 0, 0, 0)
{
    // Appliquer le look and feel personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Créer la fenêtre de tooltip
    tooltipWindow = std::make_unique<juce::TooltipWindow>(this, 700);
    
    // Créer les panels
    headerPanel = std::make_unique<HeaderPanel>();
    sidebarPanel = std::make_unique<SidebarPanel>();
    statusPanel = std::make_unique<StatusPanel>();
    
    diatonyPanel = std::make_unique<DiatonyContentPanel>();
    harmonizerPanel = std::make_unique<HarmonizerContentPanel>();
    
    // Créer le composant toast pour les notifications
    toastComponent = std::make_unique<ToastComponent>();
    toastComponent->setVisible(false);
    
    // Ajouter les panels à l'interface
    addAndMakeVisible(*headerPanel);
    addAndMakeVisible(*sidebarPanel);
    sidebarPanel->setVisible(false);  // Caché par défaut  
    addChildComponent(*diatonyPanel);
    addChildComponent(*harmonizerPanel);
    addChildComponent(*toastComponent);
    
    // Configurer les callbacks et l'interactivité
    setupPanels();
    
    // Initialiser la visibilité des panels
    updateContentPanelVisibility();
    
    // Définir la taille de la fenêtre
    setSize (1200, 800);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // Nettoyer le look and feel
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dessiner le fond
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Barre d'en-tête en haut
    auto headerHeight = 50;
    headerPanel->setBounds(bounds.removeFromTop(headerHeight));
    
    // Zone restante après le header
    auto remainingBounds = bounds;
    
    // Si la sidebar est visible, on ajuste les bounds en conséquence
    if (sidebarPanel->isVisible())
    {
        int sidebarWidth = 220;
        sidebarPanel->setBounds(remainingBounds.removeFromLeft(sidebarWidth));
    }

    // Positionner les panels de contenu dans tout l'espace restant
    diatonyPanel->setBounds(remainingBounds);
    harmonizerPanel->setBounds(remainingBounds);

    // Positionner le toast (pleine largeur)
    toastComponent->setBounds(getLocalBounds());
}

//==============================================================================
void AudioPluginAudioProcessorEditor::setupPanels()
{
    // Configurer les callbacks pour le panel d'en-tête
    headerPanel->onDiatonyClicked = [this]() {
        handleDiatonyModeClicked();
    };
    
    headerPanel->onHarmonizerClicked = [this]() {
        handleHarmonizerModeClicked();
    };
    
    headerPanel->onSettingsClicked = [this]() {
        handleSettingsClicked();
    };
    
    // Configurer les callbacks pour le DiatonyContentPanel
    diatonyPanel->onModelChanged = [this](const DiatonyModel& model) {
        handleModelChanged(model);
    };
    
    diatonyPanel->onGenerateRequested = [this]() {
        handleGenerateButtonClicked();
    };
    
    diatonyPanel->onPlayRequested = [this]() {
        handlePlayButtonClicked();
    };
    
    // Configurer les callbacks pour la sidebar
    sidebarPanel->onRefreshRequested = [this]() {
        handleRefreshSolutions();
    };
    
    sidebarPanel->onLoadRequested = [this](const SolutionHistoryItem& solution) {
        handleLoadSolution(solution);
    };
    
    sidebarPanel->onSolutionSelected = [this](const SolutionHistoryItem& solution) {
        handleSolutionSelected(solution);
    };

    // Ajouter le callback pour le bouton de toggle sidebar
    headerPanel->onToggleSidebarClicked = [this]() {
        toggleSidebar();
    };
}

//==============================================================================
void AudioPluginAudioProcessorEditor::updateContentPanelVisibility()
{
    diatonyPanel->setVisible(isDiatonyMode);
    harmonizerPanel->setVisible(!isDiatonyMode);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handleDiatonyModeClicked()
{
    isDiatonyMode = true;
    updateContentPanelVisibility();
    toastComponent->showMessage(juce::String::fromUTF8("🟠 Switched to Diatony mode"));
}

void AudioPluginAudioProcessorEditor::handleHarmonizerModeClicked()
{
    isDiatonyMode = false;
    updateContentPanelVisibility();
    toastComponent->showMessage(juce::String::fromUTF8("🟢 Switched to Harmonizer mode"));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handleModelChanged(const DiatonyModel& model)
{
    // Mettre à jour le status avec les informations du modèle
    statusPanel->setGenerationStatus(model.toString());
    
    // TODO: Ajouter d'autres logiques selon les besoins
    // Par exemple, activer/désactiver des boutons selon l'état du modèle
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handleGenerateButtonClicked()
{
    DBG("handleGenerateButtonClicked: début");
    
    // Récupérer le modèle depuis DiatonyContentPanel
    const auto& model = diatonyPanel->getModel();
    
    // Vérifier si le modèle est complet
    if (!model.isComplete()) {
        DBG("Modèle incomplet, génération annulée");
        statusPanel->setGenerationStatus(juce::String("Modèle incomplet"));
        return;
    }
    
    DBG("Modèle complet, génération en cours...");
    statusPanel->setGenerationStatus(juce::String::fromUTF8("Génération en cours..."));
    
    // Générer la solution MIDI en utilisant le nouveau modèle
    juce::String midiPath = processorRef.generateMidiSolution(model);
    
    if (!midiPath.isEmpty()) {
        statusPanel->setGenerationStatus(juce::String::fromUTF8("Génération réussie !"));
        // TODO: Activer le bouton de lecture quand GenerationZone sera connectée
    } else {
        statusPanel->setGenerationStatus(juce::String::fromUTF8("Échec de la génération"));
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handlePlayButtonClicked()
{
    if (processorRef.isPlayingMidi()) {
        processorRef.stopMidiPlayback();
        statusPanel->setPlaybackStatus(juce::String::fromUTF8("Lecture en pause"));
    } else {
        if (processorRef.startMidiPlayback()) {
            statusPanel->setPlaybackStatus(juce::String::fromUTF8("Lecture en cours..."));
        } else {
            statusPanel->setPlaybackStatus(juce::String::fromUTF8("Erreur de lecture"));
        }
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handlePlaybackFinished()
{
    juce::MessageManager::callAsync([this]() {
        statusPanel->setPlaybackStatus("");
    });
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handleRefreshSolutions()
{
    // Charger les solutions de la base de données
    sidebarPanel->loadSolutionsFromDb();
    
    // Afficher un toast de confirmation
    toastComponent->showMessage(juce::String::fromUTF8("Historique mis à jour"));
}

void AudioPluginAudioProcessorEditor::handleLoadSolution(const SolutionHistoryItem& solution)
{
    // Récupérer le chemin vers le fichier MIDI
    juce::String midiPath = solution.getPath();
    
    if (juce::File(midiPath).existsAsFile()) {
        // Charger le fichier MIDI pour lecture
        processorRef.loadMidiFile(midiPath);
        
        // Afficher un toast de confirmation
        juce::String message = juce::String::fromUTF8("Loaded solution: ") + solution.getName();
        toastComponent->showMessage(message);
    } else {
        // Le fichier n'existe pas - afficher un toast d'erreur
        juce::String errorMessage = juce::String::fromUTF8("Error: MIDI file not found");
        toastComponent->showMessage(errorMessage);
    }
}

void AudioPluginAudioProcessorEditor::handleSolutionSelected(const SolutionHistoryItem& solution)
{
    // Afficher une notification toast
    juce::String message = juce::String::fromUTF8("Selected solution: ") + solution.getName();
    toastComponent->showMessage(message);
}

void AudioPluginAudioProcessorEditor::handleSettingsClicked()
{
    DiatonyAlertWindow::show(
        juce::String::fromUTF8("Settings"),
        juce::String::fromUTF8("Standalone and DAW plugin developed by C. Niyikiza and D. Sprockeels."),
        juce::String::fromUTF8("Close")
    );
}

void AudioPluginAudioProcessorEditor::toggleSidebar()
{
    isSidebarVisible = !isSidebarVisible;
    sidebarPanel->setVisible(isSidebarVisible);
    resized();  // Pour recalculer la disposition
}

