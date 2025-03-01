#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    juce::ignoreUnused (sampleRate, samplesPerBlock);
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    // clear any input
    buffer.clear();
    // midiMessages.clear();
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

void AudioPluginAudioProcessor::generateMidiSolution()
{
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
    
    // Configuration identique au main.cpp
    std::unique_ptr<MajorTonality> tonality(new MajorTonality(C));
    
    vector<int> chords = {FIRST_DEGREE, SIXTH_DEGREE, FIVE_OF_FIVE, 
                         FIFTH_DEGREE_APPOGIATURA, FIFTH_DEGREE, FIRST_DEGREE};
    
    vector<int> chords_qualities;
    for(int chord : chords)
        chords_qualities.push_back(tonality->get_chord_quality(chord));
    
    chords_qualities[1] = MINOR_SEVENTH_CHORD;
    chords_qualities[2] = MINOR_NINTH_DOMINANT_CHORD;
    chords_qualities[4] = DIMINISHED_SEVENTH_CHORD;
    
    vector<int> states = {FUNDAMENTAL_STATE, FUNDAMENTAL_STATE, FIRST_INVERSION, 
                          SECOND_INVERSION, FUNDAMENTAL_STATE, FUNDAMENTAL_STATE};
    
    int size = chords.size();
    
    // vector<FourVoiceTexture*> sols;
    try {
        // Obtention de la solution
        auto newSolutions = solve_diatony_problem(size, tonality.get(), chords, 
                                                chords_qualities, states, false);

        if (!newSolutions.empty()) {
            // Écriture du fichier MIDI
            juce::String finalPath = fullPath + "_0.mid";
            DBG(juce::String(juce::CharPointer_UTF8("Écriture du fichier MIDI : ")) + finalPath);
            writeSolToMIDIFile(size, finalPath.toStdString(), newSolutions.back());
            DBG(juce::String(juce::CharPointer_UTF8("MIDI sauvegardé : ")) + finalPath);
        } else {
            DBG("Aucune solution trouvée");
        }
    }catch (const std::exception& e) {
        DBG("Exception pendant la génération : " + juce::String(e.what()));
    }
}
