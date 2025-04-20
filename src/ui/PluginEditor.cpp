#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), titleBounds(0, 0, 0, 0)
{
    // Appliquer le look and feel personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Créer la fenêtre de tooltip
    tooltipWindow = std::make_unique<juce::TooltipWindow>(this, 700); // 700ms de délai avant l'apparition
    
    // Créer les panels
    sidebarPanel = std::make_unique<SidebarPanel>();
    statusPanel = std::make_unique<StatusPanel>();
    tonalityPanel = std::make_unique<TonalityPanel>();
    progressionPanel = std::make_unique<ProgressionPanel>();
    generationPanel = std::make_unique<GenerationPanel>();
    
    // Créer le composant toast pour les notifications
    toastComponent = std::make_unique<ToastComponent>();
    toastComponent->setVisible(false);
    
    // Ajouter les panels à l'interface
    addAndMakeVisible(*sidebarPanel);
    // addAndMakeVisible(*statusPanel);
    // addAndMakeVisible(*tonalityPanel);
    // addAndMakeVisible(*progressionPanel);
    // addAndMakeVisible(*generationPanel);
    addChildComponent(*toastComponent);
    
    // Configurer les callbacks et l'interactivité
    setupPanels();
    
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
    
    // Dessiner le titre aligné avec le StatusPanel
    if (titleBounds.getWidth() > 0) {
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
        g.drawText("Diatony DAW Application", titleBounds, juce::Justification::centred, false);
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Positionner le toast (pleine largeur)
    toastComponent->setBounds(bounds);
    
    // Largeur de la barre latérale augmentée
    int sidebarWidth = 220; // Élargissement de la barre latérale
    
    // Placer la barre latérale à gauche
    auto sidebarBounds = bounds.removeFromLeft(sidebarWidth);
    sidebarPanel->setBounds(sidebarBounds);
    
    // Espacement entre la barre latérale et le contenu principal
    bounds.removeFromLeft(10);
    
    // Réduire les marges du contenu principal
    auto area = bounds.reduced(20);
    
    // Réserver de l'espace pour le titre
    auto titleArea = area.removeFromTop(40);
    
    // Panel de statuts - hauteur augmentée pour afficher les deux zones
    auto statusBounds = area.removeFromTop(130);
    statusPanel->setBounds(statusBounds);
    area.removeFromTop(5); // Espace minimal entre les panels
    
    // Panel de tonalité
    tonalityPanel->setBounds(area.removeFromTop(120));
    area.removeFromTop(5); // Espace minimal entre les panels
    
    // Panel de progression
    progressionPanel->setBounds(area.removeFromTop(160));
    area.removeFromTop(5); // Espace minimal entre les panels
    
    // Panel de génération (boutons)
    generationPanel->setBounds(area.removeFromTop(50)); // Hauteur réduite pour les boutons
    
    // Stocker les coordonnées pour le titre aligné avec le StatusPanel
    titleBounds = juce::Rectangle<int>(statusBounds.getX(), titleArea.getY(), 
                                       statusBounds.getWidth(), titleArea.getHeight());
    
    // Force le redessin pour que le titre apparaisse
    repaint();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::setupPanels()
{
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
    // Implémentation des paramètres (vous pourriez utiliser un DialogWindow personnalisé)
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::InfoIcon,
        "Paramètres",
        "Fonctionnalité à venir dans une future mise à jour.",
        "OK"
    );
}

