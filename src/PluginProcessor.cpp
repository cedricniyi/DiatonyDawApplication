#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SimpleVoice.h"
#include "SimpleSound.h"
#include "utils/DiatonyConstants.h"

#ifndef  JucePlugin_Name
 #define JucePlugin_Name    "DiatonyDawApplication"
#endif

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    synth.clearVoices();
    // Add custom voices (e.g. 4 voices)
    for (int i = 0; i < 4; ++i)
        synth.addVoice(new SimpleVoice());

    // Clear any existing sounds and add our custom sound
    synth.clearSounds();
    synth.addSound(new SimpleSound());
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    releaseResources();
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // juce::ignoreUnused (sampleRate, samplesPerBlock);
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    if (midiPlaying && midiSequence != nullptr) {
        // Buffer MIDI temporaire pour la prévisualisation interne
        juce::MidiBuffer previewMidiBuffer;
        int numSamples = buffer.getNumSamples();

        // Utiliser un tempo fixe (ici 120 BPM) - à adapter si vous récupérez le tempo dynamiquement
        double bpm = 120.0; // Assurez-vous d'utiliser le BPM correct
        double secondsPerQuarterNote = 60.0 / bpm;
        double samplesPerQuarterNote = getSampleRate() * secondsPerQuarterNote;
        double samplesPerTick = samplesPerQuarterNote / midiFile->getTimeFormat();

        double startTick = currentMidiPosition;
        double endTick = startTick + (numSamples / samplesPerTick);

        // DBG("Time Format: " << midiFile->getTimeFormat());
        // DBG("Sample Rate: " << getSampleRate());
        // DBG("Samples per Tick: " << samplesPerTick);
        // DBG("Current MIDI Position: " << currentMidiPosition);

        for (int i = 0; i < midiSequence->getNumEvents(); ++i) {
            auto* midiEvent = midiSequence->getEventPointer(i);
            double eventTick = midiEvent->message.getTimeStamp();
            
            if (eventTick >= startTick && eventTick < endTick) {
                int samplePosition = static_cast<int>((eventTick - startTick) * samplesPerTick);
                samplePosition = std::clamp(samplePosition, 0, numSamples - 1);

                previewMidiBuffer.addEvent(midiEvent->message, samplePosition);
                midiMessages.addEvent(midiEvent->message, samplePosition);
            }
        }

        synth.renderNextBlock(buffer, previewMidiBuffer, 0, numSamples);

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = std::clamp(channelData[sample], -0.95f, 0.95f);
        }
        // Mise à jour de la position
        currentMidiPosition = endTick;
        
        // Vérifier si nous avons atteint la fin
        if (currentMidiPosition >= midiSequence->getEndTime()) {
            stopMidiPlayback();
            // Notifier que la lecture est terminée
            if (auto* editor = dynamic_cast<AudioPluginAudioProcessorEditor*>(getActiveEditor()))
                editor->handlePlaybackFinished();
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

//==============================================================================
juce::String AudioPluginAudioProcessor::generateMidiSolution()
{
    // Vérifications défensives (redondantes avec les setters, mais conservées par sécurité)
    // Vérifier que nous avons les données requises
    if (currentChords.empty() || currentStates.empty()) {
        DBG("Erreur: La progression et les états sont obligatoires");
        return {};
    }

    if (currentStates.size() != currentChords.size()) {
        DBG("Erreur: Le nombre d'états ne correspond pas au nombre d'accords");
        return {};
    }

    // Si des qualités sont spécifiées, vérifier qu'elles sont complètes
    if (!currentChordQualities.empty() && currentChordQualities.size() != currentChords.size()) {
        DBG("Erreur: Le nombre de qualités ne correspond pas au nombre d'accords");
        return {};
    }

    // 1. Déterminer le chemin racine du projet
    // À partir du chemin actuel du fichier source (__FILE__)
    juce::File projectDir = juce::File(__FILE__) // Chemin de ce fichier .cpp
                          .getParentDirectory()  // Remonte à src/
                          .getParentDirectory()  // Remonte au dossier racine du projet
                          .getChildFile("out")
                          .getChildFile("MidiFiles");
    
    // 2. Créer l'arborescence de dossiers
    if (!projectDir.exists()) {
        projectDir.createDirectory(); // Crée out/MidiFiles
    }

    // 3. Chemin complet avec nom de fichier
    juce::String fullPath = projectDir.getFullPathName() 
                          + juce::File::getSeparatorString() 
                          + "solfromjuce";
    
    DBG("(PluginProcessor.cpp) Dossier MIDI du projet: " + projectDir.getFullPathName());
    
    // Au lieu d'utiliser un unique_ptr<Tonality>, utiliser un unique_ptr du type concret
    std::unique_ptr<MajorTonality> majorTonality;
    std::unique_ptr<MinorTonality> minorTonality;
    Tonality* tonality = nullptr;
    
    if (isMajorMode) {
        majorTonality = std::make_unique<MajorTonality>(currentTonality);
        tonality = majorTonality.get();
    } else {
        minorTonality = std::make_unique<MinorTonality>(currentTonality);
        tonality = minorTonality.get();
    }
    
    // * Default progression
    // {FIRST_DEGREE, SIXTH_DEGREE, FIVE_OF_FIVE, FIFTH_DEGREE_APPOGIATURA, FIFTH_DEGREE, FIRST_DEGREE};
    // * Default states
    // {FUNDAMENTAL_STATE, FUNDAMENTAL_STATE, FIRST_INVERSION, SECOND_INVERSION, FUNDAMENTAL_STATE, FUNDAMENTAL_STATE};
    // * Qualities
    // chords_qualities[1] = MINOR_SEVENTH_CHORD;
    // chords_qualities[2] = MINOR_NINTH_DOMINANT_CHORD;
    // chords_qualities[4] = DIMINISHED_SEVENTH_CHORD;
    vector<int> chords;
    vector<int> states;

    DBG("Utilisation de la progression utilisateur: " + 
        juce::String(currentChords.size()) + " accords");
    chords = currentChords;
    states = currentStates;

    vector<int> chords_qualities;
    for(size_t i = 0; i < chords.size(); ++i) {
        if (i < currentChordQualities.size() && currentChordQualities[i] != -1) {
            chords_qualities.push_back(currentChordQualities[i]);
        } else {
            chords_qualities.push_back(tonality->get_chord_quality(chords[i]));
        }
    }
    

    int size = chords.size();
    
    try {
        // Obtention de la solution
        auto newSolutions = solve_diatony_problem(size, tonality, chords, 
                                                chords_qualities, states, false);

        if (!newSolutions.empty()) {
            // Écriture du fichier MIDI
            juce::String finalPath = fullPath + "_0.mid";
            DBG(juce::String::fromUTF8("Écriture du fichier MIDI : ") + finalPath);
            writeSolToMIDIFile(size, finalPath.toStdString(), newSolutions.back());
            DBG(juce::String::fromUTF8("MIDI sauvegardé : ") + finalPath);
            
            currentMidiFilePath = finalPath;

            currentChords.clear();
            currentStates.clear();
            currentChordQualities.clear();

            return finalPath;
        } else {
            DBG("Aucune solution trouvée");
            return {};
        }
    }catch (const std::exception& e) {
        DBG("Exception pendant la génération : " + juce::String(e.what()));
        return {};
    }
}

//==============================================================================
// Ajout des nouvelles méthodes pour la prévisualisation MIDI
bool AudioPluginAudioProcessor::startMidiPlayback()
{
    stopMidiPlayback(); // Arrêter toute lecture en cours
    
    if (currentMidiFilePath.isEmpty())
        return false;
        
    juce::File midiFileToPlay(currentMidiFilePath);
    
    if (!midiFileToPlay.existsAsFile())
        return false;
        
    // Charger le fichier MIDI
    midiFile = std::make_unique<juce::MidiFile>();
    juce::FileInputStream inputStream(midiFileToPlay);
    
    if (inputStream.openedOk() && midiFile->readFrom(inputStream)) {
        // Préparer la séquence de lecture
        midiSequence = std::make_unique<juce::MidiMessageSequence>();
        
        // Combiner toutes les pistes en une seule séquence
        for (int track = 0; track < midiFile->getNumTracks(); ++track) {
            midiSequence->addSequence(*midiFile->getTrack(track), 0.0);
        }
        
        midiSequence->updateMatchedPairs();
        currentMidiPosition = 0;
        midiPlaying = true;
        
        return true;
    }
    
    return false;
}

void AudioPluginAudioProcessor::stopMidiPlayback()
{
    midiPlaying = false;
    currentMidiPosition = 0;
}

//==============================================================================
bool AudioPluginAudioProcessor::isPlayingMidi() const
{
    return midiPlaying;
}

//==============================================================================
void AudioPluginAudioProcessor::setTonality(int noteValue)
{
    currentTonality = noteValue;
    DBG("Tonality set to: " + juce::String(DiatonyConstants::getNoteName(currentTonality)));
}

//==============================================================================
void AudioPluginAudioProcessor::setMode(bool isMajor)
{
    isMajorMode = isMajor;
    DBG("Mode set to: " + juce::String(isMajor ? "Major" : "Minor"));
}

// ================================================
bool AudioPluginAudioProcessor::setProgressionFromString(const juce::String& progression)
{
    // Vérifier si l'input est vide
    if (progression.isEmpty()) {
        DBG("Erreur: La progression est obligatoire");
        return false;
    }
    
    std::vector<int> newChords;
    auto tokens = juce::StringArray::fromTokens(progression, " -", "");
    
    // Vérifier si on a au moins un token
    if (tokens.isEmpty()) {
        DBG("Erreur: La progression est vide après tokenization");
        return false;
    }

    // 
    for (const auto& token : tokens) {
        int degreeValue = DiatonyConstants::getDegreeValue(token.toStdString());
        if (degreeValue != -1) {
            newChords.push_back(degreeValue);
        } else {
            DBG(juce::String::fromUTF8("Degré invalide trouvé: ") + token); // Debug pour identifier le degré problématique
            return false; // Degré invalide trouvé
        }
    }
    
    currentChords = newChords;
    return true;
}

//==============================================================================
bool AudioPluginAudioProcessor::setStatesFromString(const juce::String& states)
{
    // Si l'input est vide
    if (states.isEmpty()) {
        DBG("Erreur: Les états sont obligatoires");
        return false;
    }

    // Vérifier que nous avons une progression valide
    if (currentChords.empty()) {
        DBG("Erreur: Définir la progression avant les états");
        return false;
    }

    std::vector<int> newStates;
    auto tokens = juce::StringArray::fromTokens(states, " -", "");

    // Vérifier que le nombre d'états correspond au nombre d'accords
    if (tokens.size() != currentChords.size()) {
        DBG("Nombre d'états différent du nombre d'accords");
        return false;
    }

    for (const auto& token : tokens) {
        // Essayer d'abord comme un nombre
        if (token.isNotEmpty() && token.containsOnly("0123")) {
            int stateValue = token.getIntValue();
            if (stateValue >= 0 && stateValue <= 3) {  // Valeurs valides : 0,1,2,3
                newStates.push_back(stateValue);
                continue;
            }
        }
        
        // Sinon essayer comme un nom d'état
        int stateValue = DiatonyConstants::getChordStateValue(token.toStdString());
        if (stateValue != -1) {
            newStates.push_back(stateValue);
        } else {
            DBG(juce::String::fromUTF8("État invalide trouvé: ") + token);
            return false;
        }
    }

    currentStates = newStates;
    return true;
}

bool AudioPluginAudioProcessor::setChordQualitiesFromString(const juce::String& qualities) {
    // Si l'input est vide ou pas d'accords, on utilise les qualités par défaut
    if (qualities.isEmpty()) {
        currentChordQualities.clear();
        return true; // Permet les qualités vides
    }

    // Vérifier que nous avons une progression valide
    if (currentChords.empty()) {
        DBG("Erreur: Définir la progression avant les qualités");
        return false;
    }

    auto tokens = juce::StringArray::fromTokens(qualities, " ", "");
    
    // Vérifier que nous n'avons pas plus de qualités que d'accords
    if (tokens.size() != currentChords.size()) {
        DBG("Nombre de qualités différent du nombre d'accords");
        return false;
    }

    std::vector<int> newQualities(currentChords.size(), -1); // -1 signifie "utiliser la qualité par défaut"

    int qualityIndex = 0;
    for (const auto& token : tokens) {
        if (token.isEmpty() || token == "-") {
            qualityIndex++;
            continue;
        }

        int qualityValue = DiatonyConstants::getChordQualityValue(token.toStdString());
        if (qualityValue == -1) {
            DBG(juce::String::fromUTF8("Qualité invalide trouvée: ") + token);
            return false;
        }
        newQualities[qualityIndex++] = qualityValue;
    }

    currentChordQualities = newQualities;
    return true;
}