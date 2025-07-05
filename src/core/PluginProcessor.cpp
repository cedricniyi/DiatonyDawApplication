#include "PluginProcessor.h"
#include "../ui/PluginEditor.h"
#include "../audio/SimpleVoice.h"
#include "../audio/SimpleSound.h"

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
    // Ajouter les voix de synthé (par exemple 4 voix)
    for (int i = 0; i < 4; ++i)
        synth.addVoice(new SimpleVoice());

    // Effacer tous les sons existants et ajouter notre son personnalisé
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
    // Initialisation avant la lecture
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // Libérer les ressources lorsque la lecture s'arrête
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Vérifier si la configuration est supportée
    // Dans ce code, on ne supporte que mono ou stéréo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Vérifier si l'entrée correspond à la sortie
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

    if (chordSequence.isPlayingMidi() && chordSequence.getMidiSequence() != nullptr) {
        // Buffer MIDI temporaire pour la prévisualisation interne
        juce::MidiBuffer previewMidiBuffer;
        int numSamples = buffer.getNumSamples();

        // Utiliser un tempo fixe (ici 120 BPM) - à adapter si besoin
        double bpm = 120.0;
        double secondsPerQuarterNote = 60.0 / bpm;
        double samplesPerQuarterNote = getSampleRate() * secondsPerQuarterNote;
        double samplesPerTick = samplesPerQuarterNote / chordSequence.getMidiFile()->getTimeFormat();

        double startTick = chordSequence.getCurrentPosition();
        double endTick = startTick + (numSamples / samplesPerTick);

        // Parcourir les événements MIDI
        auto midiSeq = chordSequence.getMidiSequence();
        for (int i = 0; i < midiSeq->getNumEvents(); ++i) {
            auto* midiEvent = midiSeq->getEventPointer(i);
            double eventTick = midiEvent->message.getTimeStamp();
            
            if (eventTick >= startTick && eventTick < endTick) {
                int samplePosition = static_cast<int>((eventTick - startTick) * samplesPerTick);
                samplePosition = std::clamp(samplePosition, 0, numSamples - 1);

                previewMidiBuffer.addEvent(midiEvent->message, samplePosition);
                midiMessages.addEvent(midiEvent->message, samplePosition);
            }
        }

        // Rendre l'audio
        synth.renderNextBlock(buffer, previewMidiBuffer, 0, numSamples);

        // Limiter l'amplitude pour éviter les distorsions
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = std::clamp(channelData[sample], -0.95f, 0.95f);
        }
        
        // Mise à jour de la position
        chordSequence.setCurrentPosition(endTick);
        
        // Vérifier si nous avons atteint la fin
        if (chordSequence.getCurrentPosition() >= midiSeq->getEndTime()) {
            stopMidiPlayback();
            handlePlaybackFinished();
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // TODO: Sauvegarder les paramètres dans le bloc de mémoire
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // TODO: Restaurer vos paramètres depuis ce bloc de mémoire
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// API simplifiée pour l'interface utilisateur
juce::String AudioPluginAudioProcessor::generateMidiSolution(const DiatonyModel& model)
{
    return chordSequence.generateMidiSolution(model);
}

bool AudioPluginAudioProcessor::startMidiPlayback()
{
    return chordSequence.startMidiPlayback();
}

void AudioPluginAudioProcessor::stopMidiPlayback()
{
    chordSequence.stopMidiPlayback();
}

bool AudioPluginAudioProcessor::isPlayingMidi() const
{
    return chordSequence.isPlayingMidi();
}

bool AudioPluginAudioProcessor::loadMidiFile(const juce::String& midiFilePath)
{
    // Nous utilisons directement la méthode existante dans ChordSequence
    return chordSequence.loadSolutionByPath(midiFilePath);
}

void AudioPluginAudioProcessor::handlePlaybackFinished()
{
    if (auto* editor = dynamic_cast<AudioPluginAudioProcessorEditor*>(getActiveEditor()))
        editor->handlePlaybackFinished();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
} 