#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "../Diatony/c++/headers/aux/Tonality.hpp"
#include "../Diatony/c++/headers/aux/Utilities.hpp"
#include "../Diatony/c++/headers/aux/MidiFileGeneration.hpp"
#include "../Diatony/c++/headers/diatony/SolveDiatony.hpp"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::String generateMidiSolution();
    
    // Méthodes pour la prévisualisation MIDI
    bool startMidiPlayback();
    void stopMidiPlayback();
    bool isPlayingMidi() const;
    void setTonality(int noteValue);
    void setMode(bool isMajor);

    // ================================================
    bool setProgressionFromString(const juce::String& progression);
    bool setStatesFromString(const juce::String& states);
    bool setChordQualitiesFromString(const juce::String& qualities);
    
private:

    // Pour la prévisualisation MIDI
    juce::String currentMidiFilePath;
    std::unique_ptr<juce::MidiFile> midiFile;
    std::unique_ptr<juce::MidiMessageSequence> midiSequence;
    
    int currentMidiPosition = 0;
    bool midiPlaying = false;
    int currentTonality = C; // Valeur par défaut
    bool isMajorMode = true;  

    std::vector<int> currentChords;
    std::vector<int> currentStates;
    std::vector<int> currentChordQualities;

    juce::Synthesiser synth;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
