#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Appliquer le look and feel personnalisé
    setLookAndFeel(&diatonyLookAndFeel);
    
    // Créer la fenêtre de tooltip
    tooltipWindow = std::make_unique<juce::TooltipWindow>(this, 700); // 700ms de délai avant l'apparition
    
    // Créer les panels
    tonalityPanel = std::make_unique<TonalityPanel>();
    progressionPanel = std::make_unique<ProgressionPanel>();
    generationPanel = std::make_unique<GenerationPanel>();
    
    // Ajouter les panels à l'interface
    addAndMakeVisible(*tonalityPanel);
    addAndMakeVisible(*progressionPanel);
    addAndMakeVisible(*generationPanel);
    
    // Configurer les callbacks et l'interactivité
    setupPanels();
    
    // Définir la taille de la fenêtre
    setSize (800, 600);
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
    
    // Dessiner le titre
    g.setColour(juce::Colours::white);
    // g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    g.drawText("Diatony DAW Application", getLocalBounds().removeFromTop(60), juce::Justification::centred, true);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    
    // Réserver de l'espace pour le titre
    area.removeFromTop(60);
    
    // Panel de tonalité
    tonalityPanel->setBounds(area.removeFromTop(120));
    area.removeFromTop(20); // Espace entre les panels
    
    // Panel de progression
    progressionPanel->setBounds(area.removeFromTop(160));
    area.removeFromTop(20); // Espace entre les panels
    
    // Panel de génération et contrôle
    generationPanel->setBounds(area);
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
        generationPanel->setGenerationStatus(juce::String("Progression invalide"));
        generationPanel->setGenerateButtonEnabled(true);
        return;
    }
    
    DBG("Progression valide, génération en cours...");
    generationPanel->setGenerationStatus(juce::String::fromUTF8("Génération en cours..."));
    
    // Générer la solution MIDI
    juce::String midiPath = processorRef.generateMidiSolution(currentProgression);
    
    if (!midiPath.isEmpty()) {
        generationPanel->setGenerationStatus(juce::String::fromUTF8("Génération réussie !"));
        generationPanel->setPlayButtonEnabled(true);
        progressionPanel->clearInputs();
        currentProgression.clear(); // Réinitialiser la progression
    } else {
        generationPanel->setGenerationStatus(juce::String::fromUTF8("Échec de la génération"));
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
        generationPanel->setPlaybackStatus(juce::String::fromUTF8("Lecture en pause"));
        generationPanel->setGenerateButtonEnabled(true);
    } else {
        if (processorRef.startMidiPlayback()) {
            generationPanel->setPlayButtonText(juce::String::fromUTF8("Arrêter"));
            generationPanel->setPlaybackStatus(juce::String::fromUTF8("Lecture en cours..."));
            generationPanel->setGenerateButtonEnabled(false);
        } else {
            generationPanel->setPlaybackStatus(juce::String::fromUTF8("Erreur de lecture"));
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
        generationPanel->setPlaybackStatus("");
        generationPanel->setGenerateButtonEnabled(true);
    });
} 