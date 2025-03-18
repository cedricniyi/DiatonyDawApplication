#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "utils/DiatonyConstants.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Ajouter ceci au début du constructeur
    tooltipWindow = std::make_unique<juce::TooltipWindow>(this, 700); // 700ms de délai avant l'apparition

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
    
    progressionLabel = std::make_unique<juce::Label>();
    progressionInput = std::make_unique<juce::TextEditor>();

    modeLabel = std::make_unique<juce::Label>();

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

    addAndMakeVisible(*progressionLabel);
    addAndMakeVisible(*progressionInput);

    progressionLabel->setText("Progression:", juce::dontSendNotification);
    progressionLabel->setJustificationType(juce::Justification::right);
    progressionInput->setMultiLine(false);
    progressionInput->setTextToShowWhenEmpty(juce::String::fromUTF8("Entrez la progression (ex: I VI V/V Vda V I) [progression séparée par des espaces ou \"-\"]"), juce::Colours::grey);

    // Ajout du tooltip pour la progression
    juce::String progressionTooltip = juce::String::fromUTF8(
        "Les degrés possibles sont : \n"
        "\tI   : Premier degré\n"
        "\tII  : Deuxième degré\n"
        "\tIII : Troisième degré\n"
        "\tIV  : Quatrième degré\n"
        "\tV   : Cinquième degré\n"
        "\tVI  : Sixième degré\n"
        "\tVII : Septième degré\n"
        "\tVda : Quinte diminuée avec appoggiature\n"
        "\tV/II    : Dominante du deuxième degré\n"
        "\tV/III   : Dominante du troisième degré\n"
        "\tV/IV    : Dominante du quatrième degré\n"
        "\tV/V     : Dominante du cinquième degré\n"
        "\tV/VI    : Dominante du sixième degré\n"
        "\tV/VII   : Dominante du septième degré\n"
        "\tbII     : Deuxième degré abaissé\n"
        "\t6te_aug : Sixte augmentée\n"
        "\nExemple : I VI V/V Vda V I"
    );

    progressionLabel->setTooltip(progressionTooltip);
    progressionInput->setTooltip(progressionTooltip);

    // Configuration du label et input pour les états de progression
    progressionStateLabel = std::make_unique<juce::Label>();
    progressionStateInput = std::make_unique<juce::TextEditor>();
    addAndMakeVisible(*progressionStateLabel);
    addAndMakeVisible(*progressionStateInput);

    progressionStateLabel->setText(juce::String::fromUTF8("États:"), juce::dontSendNotification);
    progressionStateLabel->setJustificationType(juce::Justification::right);
    progressionStateInput->setMultiLine(false);
    progressionStateInput->setTextToShowWhenEmpty(juce::String::fromUTF8("Entrez les états (ex: Fund 1st 2nd Fund Fund Fund) [états séparés par des espaces]"), juce::Colours::grey);
    
       // Message commun pour les deux composants
    juce::String tooltipMessage = juce::String::fromUTF8("Les états possibles sont : \n\tFund / 0 (fondamental), \n\t1st / 1 (premier renversement), "
                                 "\n\t2nd / 2 (deuxième renversement), \n\t3rd / 3 (troisième renversement). \n\nExemple : Fund 1st 2nd Fund Fund Fund");
    
    progressionStateLabel->setTooltip(tooltipMessage);
    progressionStateInput->setTooltip(tooltipMessage);

    // Dans le constructeur, après la création du tonalityLabel
    addAndMakeVisible(*modeLabel);

    modeLabel->setText(juce::String::fromUTF8("Mode:"), juce::dontSendNotification);
    modeLabel->setJustificationType(juce::Justification::right);

    // Dans le constructeur
    chordQualitiesLabel = std::make_unique<juce::Label>();
    chordQualitiesInput = std::make_unique<juce::TextEditor>();
    addAndMakeVisible(*chordQualitiesLabel);
    addAndMakeVisible(*chordQualitiesInput);

    chordQualitiesLabel->setText(juce::String::fromUTF8("Qualités:"), juce::dontSendNotification);
    chordQualitiesLabel->setJustificationType(juce::Justification::right);
    chordQualitiesInput->setMultiLine(false);
    chordQualitiesInput->setTextToShowWhenEmpty(juce::String::fromUTF8("Entrez les qualités (ex: - m7 9b - dim7 -) [qualités séparées par des espaces]"), juce::Colours::grey);

    // Tooltip pour les qualités d'accords
    juce::String qualityTooltip = juce::String::fromUTF8(
        "Les qualités possibles sont : \n\n"
        "- : Qualité par défaut\n"
        "maj : Majeur\n"
        "min : Mineur\n"
        "dim : Diminué\n"
        "aug : Augmenté\n"
        "6+ : Sixte augmentée\n"
        "7 : Septième de dominante\n"
        "maj7 : Septième majeure\n"
        "min7 : Septième mineure\n"
        "dim7 : Septième diminuée\n"
        "ø : Semi-diminué\n"
        "minmaj7 : Mineur-majeur septième\n"
        "9 : Neuvième majeure dominante\n"
        "9b (9♭) : Neuvième mineure dominante\n"
        "\nExemple : - m7 9 - dim7 -"
    );

    chordQualitiesLabel->setTooltip(qualityTooltip);
    chordQualitiesInput->setTooltip(qualityTooltip);

    setSize (1000, 1000);
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
    // Fond principal
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    auto area = getLocalBounds().reduced(20);
    auto statusArea = area.removeFromTop(100);

    // Cadre pour le statut de génération
    auto generationArea = statusArea.removeFromTop(45);
    // auto generationFrame = generationArea.reduced(10, 5);  // Padding horizontal de 10, vertical de 5
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRoundedRectangle(generationArea.toFloat(), 5.0f, 2.0f);
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.fillRoundedRectangle(generationArea.toFloat(), 5.0f);
    
    // Label "Statut de génération"
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(12.0f);
    g.drawText(juce::String::fromUTF8("Statut de génération"), generationArea.removeFromTop(15), juce::Justification::centredTop, false);

    statusArea.removeFromTop(10);  // Espace entre les cadres

    // Cadre pour le statut de lecture
    auto playbackArea = statusArea.removeFromTop(45);
    // auto playbackFrame = playbackArea.reduced(10, 5);  // Padding horizontal de 10, vertical de 5
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRoundedRectangle(playbackArea.toFloat(), 5.0f, 2.0f);
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.fillRoundedRectangle(playbackArea.toFloat(), 5.0f);
    
    // Label "Statut de lecture"
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(12.0f);
    g.drawText(juce::String::fromUTF8("Statut de lecture"), playbackArea.removeFromTop(15), juce::Justification::centredTop, false);
}

//==============================================================================

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    // Ajuster la position des labels de statut avec padding
    auto labelArea = area.removeFromTop(100);

    // Generation label
    auto generationLabelArea = labelArea.removeFromTop(45);
    generationStatusLabel->setBounds(generationLabelArea.reduced(10, 5).withTrimmedTop(10));
    
    labelArea.removeFromTop(10);
    
    auto playbackLabelArea = labelArea.removeFromTop(45);
    playbackStatusLabel->setBounds(playbackLabelArea.reduced(10, 5).withTrimmedTop(10));

    area.removeFromTop(20);

    // Configuration de la zone tonalité
    auto tonalityArea = area.removeFromTop(35);
    tonalityLabel->setBounds(tonalityArea.removeFromLeft(90));
    tonalityComboBox->setBounds(tonalityArea);

    area.removeFromTop(20);

    // Zone du mode (nouveau)
    auto modeArea = area.removeFromTop(35);
    modeLabel->setBounds(modeArea.removeFromLeft(90));  // Utiliser le membre modeLabel au lieu d'une nouvelle variable
    modeComboBox->setBounds(modeArea);

    area.removeFromTop(20);

    // Zone de progression
    auto progressionArea = area.removeFromTop(35);
    progressionLabel->setBounds(progressionArea.removeFromLeft(90));
    progressionInput->setBounds(progressionArea);

    area.removeFromTop(20);

    // Zone des états de progression
    auto progressionStateArea = area.removeFromTop(35);
    progressionStateLabel->setBounds(progressionStateArea.removeFromLeft(90));
    progressionStateInput->setBounds(progressionStateArea);

    area.removeFromTop(20);

    // Zone des qualités d'accords
    auto qualitiesArea = area.removeFromTop(35);
    chordQualitiesLabel->setBounds(qualitiesArea.removeFromLeft(90));
    chordQualitiesInput->setBounds(qualitiesArea);

    area.removeFromTop(20);

    // Zone des boutons
    auto buttonArea = area.removeFromTop(40);
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
        
        // Vérifier d'abord la progression
        if (!processorRef.setProgressionFromString(progressionInput->getText())) {
            generationStatusLabel->setText(juce::String::fromUTF8("Progression invalide"), juce::dontSendNotification);
            generateButton->setEnabled(true);
            return;
        }

        // Vérifier ensuite les états
        if (!processorRef.setStatesFromString(progressionStateInput->getText())) {
            generationStatusLabel->setText(juce::String::fromUTF8("États de progression invalides"), juce::dontSendNotification);
            generateButton->setEnabled(true);
            return;
        }

        if (!processorRef.setChordQualitiesFromString(chordQualitiesInput->getText())) {
            generationStatusLabel->setText(juce::String::fromUTF8("Qualités d'accords invalides"), juce::dontSendNotification);
            generateButton->setEnabled(true);
            return;
        }

        generationStatusLabel->setText(juce::String::fromUTF8("Génération en cours..."), juce::dontSendNotification);

        if (!processorRef.generateMidiSolution().isEmpty()) {
            generationStatusLabel->setText(juce::String::fromUTF8("Génération réussie !"), juce::dontSendNotification);
            playButton->setEnabled(true);
            progressionInput->setText("", false);
            progressionStateInput->setText("", false);
            chordQualitiesInput->setText("", false);
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