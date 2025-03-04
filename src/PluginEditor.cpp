#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Initialisation des composants avec std::make_unique
    generateButton = std::make_unique<juce::TextButton>();
    playButton = std::make_unique<juce::TextButton>();
    statusLabel = std::make_unique<juce::Label>();

    // Configuration du bouton générer
    addAndMakeVisible(*generateButton);
    generateButton->setButtonText(juce::String::fromUTF8("Générer"));
    generateButton->addListener(this);
    generateButton->setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    generateButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    generateButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // Configuration du bouton lecture
    addAndMakeVisible(*playButton);
    playButton->setButtonText(juce::String::fromUTF8("Écouter"));
    playButton->addListener(this);
    playButton->setEnabled(false);
    playButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    playButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    // Configuration du label
    addAndMakeVisible(*statusLabel);
    statusLabel->setText(juce::String::fromUTF8("Prêt à générer"), juce::dontSendNotification);
    statusLabel->setJustificationType(juce::Justification::centred);

    setSize (400, 200);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // It's good practice to remove listeners in the destructor
    generateButton->removeListener(this);
    playButton->removeListener(this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // generateButton.setBounds(getLocalBounds().reduced(20)); // Bouton centré avec marge

    auto area = getLocalBounds().reduced(20);
    statusLabel->setBounds(area.removeFromTop(30));
    
    // Placer le label d'état en haut
    // statusLabel.setBounds(area.removeFromTop(30));
    
    // Diviser l'espace restant pour les boutons
    auto buttonArea = area.removeFromTop(50);
    generateButton->setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5));
    playButton->setBounds(buttonArea.reduced(5));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == generateButton.get()) {
        // Désactiver le bouton pendant la génération
        generateButton->setEnabled(false);
        statusLabel->setText(juce::String::fromUTF8("Génération en cours..."), juce::dontSendNotification);
        
        if (!processorRef.generateMidiSolution().isEmpty()) {
            statusLabel->setText(juce::String::fromUTF8("Génération réussie !"), juce::dontSendNotification);
            playButton->setEnabled(true);
        } else {
            statusLabel->setText(juce::String::fromUTF8("Échec de la génération"), juce::dontSendNotification);
            playButton->setEnabled(false);
        }
        
        generateButton->setEnabled(true);
        startTimer(2000); // Le timer réinitialisera le message après 2 secondes
    }
    else if (button == playButton.get()) {
        if (processorRef.isPlayingMidi()) {
            processorRef.stopMidiPlayback();
            playButton->setButtonText(juce::String::fromUTF8("Écouter"));
            statusLabel->setText(juce::String::fromUTF8("Prêt à jouer"), juce::dontSendNotification);
        } else {
            if (processorRef.startMidiPlayback()) {
                playButton->setButtonText(juce::String::fromUTF8("Arrêter"));
                statusLabel->setText(juce::String::fromUTF8("Lecture en cours..."), juce::dontSendNotification);
            } else {
                statusLabel->setText(juce::String::fromUTF8("Erreur de lecture"), juce::dontSendNotification);
                playButton->setEnabled(false);
            }
        }
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback(){
    statusLabel->setText(juce::String::fromUTF8("Prêt à générer"), juce::dontSendNotification);
    stopTimer();
}