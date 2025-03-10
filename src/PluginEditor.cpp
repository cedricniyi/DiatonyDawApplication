#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "utils/DiatonyConstants.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Création et configuration du label de tonalité
    tonalityLabel = std::make_unique<juce::Label>();
    addAndMakeVisible(*tonalityLabel);
    tonalityLabel->setText(juce::String::fromUTF8("Tonalité:"), juce::dontSendNotification);
    tonalityLabel->setJustificationType(juce::Justification::right);
    
    tonalityComboBox = std::make_unique<juce::ComboBox>();
    addAndMakeVisible(*tonalityComboBox);
    setupTonalityComboBox();

    // Initialisation des composants avec std::make_unique
    generateButton = std::make_unique<juce::TextButton>();
    playButton = std::make_unique<juce::TextButton>();

    // Configuration des labels
    generationStatusLabel = std::make_unique<juce::Label>();
    playbackStatusLabel = std::make_unique<juce::Label>();
    
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
    addAndMakeVisible(*generationStatusLabel);
    addAndMakeVisible(*playbackStatusLabel);

    generationStatusLabel->setText(juce::String::fromUTF8("Prêt à générer"), juce::dontSendNotification);
    generationStatusLabel->setJustificationType(juce::Justification::centred);
    playbackStatusLabel->setJustificationType(juce::Justification::centred);

    setSize (400, 300);
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

//==============================================================================

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    auto labelArea = area.removeFromTop(60);
    generationStatusLabel->setBounds(labelArea.removeFromTop(30));
    playbackStatusLabel->setBounds(labelArea);

    // Configuration de la zone tonalité
    auto tonalityArea = area.removeFromTop(30);
    tonalityLabel->setBounds(tonalityArea.removeFromLeft(70));  // Label à gauche
    tonalityComboBox->setBounds(tonalityArea);  // ComboBox prend le reste
    
    // Diviser l'espace restant entre les deux ComboBox
    auto comboBoxArea = tonalityArea;
    tonalityComboBox->setBounds(comboBoxArea.removeFromLeft(comboBoxArea.getWidth() / 2).reduced(5, 0));
    modeComboBox->setBounds(comboBoxArea.reduced(5, 0));

    // Diviser l'espace restant pour les boutons
    auto buttonArea = area.removeFromTop(50);
    generateButton->setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5));
    playButton->setBounds(buttonArea.reduced(5));
}

//==============================================================================
void AudioPluginAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == generateButton.get()) {
        // Désactiver les deux boutons pendant la génération
        generateButton->setEnabled(false);
        playButton->setEnabled(false);
        generationStatusLabel->setText(juce::String::fromUTF8("Génération en cours..."), juce::dontSendNotification);
        
        if (!processorRef.generateMidiSolution().isEmpty()) {
            generationStatusLabel->setText(juce::String::fromUTF8("Génération réussie !"), juce::dontSendNotification);
            playButton->setEnabled(true);
            startTimer(2000); // Le timer effacera le message de génération
        } else {
            generationStatusLabel->setText(juce::String::fromUTF8("Échec de la génération"), juce::dontSendNotification);
            playButton->setEnabled(false);
        }
        generateButton->setEnabled(true);
    }
    else if (button == playButton.get()) {
        if (processorRef.isPlayingMidi()) {
            processorRef.stopMidiPlayback();
            playButton->setButtonText(juce::String::fromUTF8("Écouter"));
            playbackStatusLabel->setText(juce::String::fromUTF8("Lecture en pause"), juce::dontSendNotification);
            generateButton->setEnabled(true);
        } else {
            if (processorRef.startMidiPlayback()) {
                playButton->setButtonText(juce::String::fromUTF8("Arrêter"));
                playbackStatusLabel->setText(juce::String::fromUTF8("Lecture en cours..."), juce::dontSendNotification);
                generateButton->setEnabled(false);
            } else {
                playbackStatusLabel->setText(juce::String::fromUTF8("Erreur de lecture"), juce::dontSendNotification);
                playButton->setEnabled(false);
                generateButton->setEnabled(true);
            }
        }
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback(){
    generationStatusLabel->setText("", juce::dontSendNotification);
    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::handlePlaybackFinished() {
        juce::MessageManager::callAsync([this]() {
        playButton->setButtonText(juce::String::fromUTF8("Écouter"));
        playbackStatusLabel->setText("", juce::dontSendNotification);
        generateButton->setEnabled(true);
    });
}

//==============================================================================
void AudioPluginAudioProcessorEditor::setupTonalityComboBox() {
    // Utiliser les constantes de Diatony
    for (size_t i = 0; i < DiatonyConstants::NOTES.size(); ++i) {
        const auto& note = DiatonyConstants::NOTES[i];
        tonalityComboBox->addItem(note.name, i + 1);
    }
    
    tonalityComboBox->setSelectedId(1); // C par défaut
    
    tonalityComboBox->onChange = [this]() {
        int selectedIndex = tonalityComboBox->getSelectedId() - 1;
        int noteValue = DiatonyConstants::NOTES[selectedIndex].value;
        processorRef.setTonality(noteValue);
    };

    modeComboBox = std::make_unique<juce::ComboBox>();
    addAndMakeVisible(*modeComboBox);
    
    modeComboBox->addItem("Majeur", 1);
    modeComboBox->addItem("Mineur", 2);
    modeComboBox->setSelectedId(1); // Majeur par défaut
    
    modeComboBox->onChange = [this]() {
        bool isMajor = modeComboBox->getSelectedId() == 1;
        processorRef.setMode(isMajor);
    };

}