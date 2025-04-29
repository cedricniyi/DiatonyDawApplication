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
    tonalityPanel = std::make_unique<TonalityPanel>();
    progressionPanel = std::make_unique<ProgressionPanel>();
    generationPanel = std::make_unique<GenerationPanel>();
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

    // Le reste du code...
    auto area = remainingBounds.reduced(20);
    
    // Panel de statuts - hauteur augmentée pour afficher les deux zones
    auto statusBounds = area.removeFromTop(130);
    statusPanel->setBounds(statusBounds);
    area.removeFromTop(5);
    
    // Panel de tonalité
    tonalityPanel->setBounds(area.removeFromTop(120));
    area.removeFromTop(5);
    
    // Panel de progression
    progressionPanel->setBounds(area.removeFromTop(160));
    area.removeFromTop(5);
    
    // Panel de génération (boutons)
    generationPanel->setBounds(area.removeFromTop(50));
    
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
    
    // Configurer les callbacks pour le panel de tonalité
    tonalityPanel->onTonalityChanged = [this](int noteValue) {
        processorRef.setTonality(noteValue);
    };
    
    tonalityPanel->onModeChanged = [this](bool isMajor) {
        processorRef.setMode(isMajor);
    };
    
    // Configurer les callbacks pour le panel de progression
    progressionPanel->onProgressionChanged = [this](const Progression& progression) {
        currentProgression = progression;
    };
    
    // Configurer les callbacks pour le panel de génération
    generationPanel->onGenerateClicked = [this]() {
        handleGenerateButtonClicked();
    };
    
    generationPanel->onPlayClicked = [this]() {
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
    toastComponent->showMessage(juce::String::fromUTF8("🟠 Mode Diatony activé"));
}

void AudioPluginAudioProcessorEditor::handleHarmonizerModeClicked()
{
    isDiatonyMode = false;
    updateContentPanelVisibility();
    toastComponent->showMessage(juce::String::fromUTF8("🟢 Mode Harmonizer activé"));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handleGenerateButtonClicked()
{
    DBG("handleGenerateButtonClicked: début");
    
    // Désactiver les contrôles pendant la génération
    generationPanel->setGenerateButtonEnabled(false);
    generationPanel->setPlayButtonEnabled(false);
    
    // Vérifier si la progression est valide
    DBG(juce::String::fromUTF8("Vérification validité: chords=") + juce::String(currentProgression.getChords().size()) + 
        juce::String::fromUTF8(", states=") + juce::String(currentProgression.getStates().size()));
    
    if (!currentProgression.isValid()) {
        DBG("Progression invalide, génération annulée");
        statusPanel->setGenerationStatus(juce::String("Progression invalide"));
        generationPanel->setGenerateButtonEnabled(true);
        return;
    }
    
    DBG("Progression valide, génération en cours...");
    statusPanel->setGenerationStatus(juce::String::fromUTF8("Génération en cours..."));
    
    // Générer la solution MIDI
    juce::String midiPath = processorRef.generateMidiSolution(currentProgression);
    
    if (!midiPath.isEmpty()) {
        statusPanel->setGenerationStatus(juce::String::fromUTF8("Génération réussie !"));
        generationPanel->setPlayButtonEnabled(true);
        progressionPanel->clearInputs();
        currentProgression.clear(); // Réinitialiser la progression
    } else {
        statusPanel->setGenerationStatus(juce::String::fromUTF8("Échec de la génération"));
        generationPanel->setPlayButtonEnabled(false);
    }
    
    generationPanel->setGenerateButtonEnabled(true);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handlePlayButtonClicked()
{
    if (processorRef.isPlayingMidi()) {
        processorRef.stopMidiPlayback();
        generationPanel->setPlayButtonText(juce::String::fromUTF8("Écouter"));
        statusPanel->setPlaybackStatus(juce::String::fromUTF8("Lecture en pause"));
        generationPanel->setGenerateButtonEnabled(true);
    } else {
        if (processorRef.startMidiPlayback()) {
            generationPanel->setPlayButtonText(juce::String::fromUTF8("Arrêter"));
            statusPanel->setPlaybackStatus(juce::String::fromUTF8("Lecture en cours..."));
            generationPanel->setGenerateButtonEnabled(false);
        } else {
            statusPanel->setPlaybackStatus(juce::String::fromUTF8("Erreur de lecture"));
            generationPanel->setPlayButtonEnabled(false);
            generationPanel->setGenerateButtonEnabled(true);
        }
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handlePlaybackFinished()
{
    juce::MessageManager::callAsync([this]() {
        generationPanel->setPlayButtonText(juce::String::fromUTF8("Écouter"));
        statusPanel->setPlaybackStatus("");
        generationPanel->setGenerateButtonEnabled(true);
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
        generationPanel->setPlayButtonEnabled(true);
        
        // Afficher un toast de confirmation au lieu d'utiliser la zone de statut
        juce::String message = juce::String::fromUTF8("Solution chargée: ") + solution.getName();
        toastComponent->showMessage(message);
    } else {
        // Le fichier n'existe pas - afficher un toast d'erreur
        generationPanel->setPlayButtonEnabled(false);
        
        juce::String errorMessage = juce::String::fromUTF8("Erreur: Fichier MIDI non trouvé");
        toastComponent->showMessage(errorMessage);
    }
}

void AudioPluginAudioProcessorEditor::handleSolutionSelected(const SolutionHistoryItem& solution)
{
    // Afficher une notification toast au lieu d'utiliser la zone de statut
    juce::String message = juce::String::fromUTF8("Solution sélectionnée: ") + solution.getName();
    toastComponent->showMessage(message);
}

void AudioPluginAudioProcessorEditor::handleSettingsClicked()
{
    DiatonyAlertWindow::show(
        juce::String::fromUTF8("Paramètres"),
        juce::String::fromUTF8("Standalone et plugin DAW développé par C. Niyikiza et D. Sprockeels."),
        juce::String::fromUTF8("Quitter")
    );
}

void AudioPluginAudioProcessorEditor::toggleSidebar()
{
    isSidebarVisible = !isSidebarVisible;
    sidebarPanel->setVisible(isSidebarVisible);
    resized();  // Pour recalculer la disposition
}

