#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../../Diatony/c++/headers/aux/MidiFileGeneration.hpp"
#include "../../Diatony/c++/headers/diatony/SolveDiatony.hpp"
#include "../../Diatony/c++/headers/aux/Tonality.hpp"
#include "../../Diatony/c++/headers/aux/MajorTonality.hpp"
#include "../../Diatony/c++/headers/aux/MinorTonality.hpp"
#include "Progression.h"

class ChordSequence {
public:
    ChordSequence() : currentTonality(C), isMajorMode(true), midiPlaying(false), currentMidiPosition(0) {}
    
    // Méthodes de configuration
    void setTonality(int noteValue) { 
        currentTonality = noteValue;
    }
    
    void setMode(bool isMajor) {
        isMajorMode = isMajor;
    }
    
    // Méthode de génération
    juce::String generateMidiSolution(const Progression& progression) {
        // Vérifier que la progression est valide
        if (!progression.isValid()) {
            return {};
        }
        
        // Récupérer les données de progression
        auto chords = progression.getChords();
        auto states = progression.getStates();
        auto customQualities = progression.getQualities();
        
        // Déterminer le chemin pour le fichier MIDI
        juce::File projectDir = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                               .getParentDirectory()
                               .getChildFile("out")
                               .getChildFile("MidiFiles");
    
        // Créer l'arborescence de dossiers si nécessaire
        if (!projectDir.exists()) {
            projectDir.createDirectory();
        }

        // Chemin complet avec nom de fichier
        juce::String fullPath = projectDir.getFullPathName() 
                              + juce::File::getSeparatorString() 
                              + "solfromjuce";
                              
        // Créer la tonalité appropriée
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
        
        // Préparer les qualités d'accords
        std::vector<int> chordQualities;
        for(size_t i = 0; i < chords.size(); ++i) {
            if (i < customQualities.size() && customQualities[i] != -1) {
                chordQualities.push_back(customQualities[i]);
            } else {
                chordQualities.push_back(tonality->get_chord_quality(chords[i]));
            }
        }
        
        int size = chords.size();
        
        try {
            // Obtention de la solution
            auto newSolutions = solve_diatony_problem(size, tonality, chords, 
                                                    chordQualities, states, false);

            if (!newSolutions.empty()) {
                // Écriture du fichier MIDI
                juce::String finalPath = fullPath + "_0.mid";
                writeSolToMIDIFile(size, finalPath.toStdString(), newSolutions.back());
                
                currentMidiFilePath = finalPath;
                return finalPath;
            } else {
                return {};
            }
        } catch (const std::exception& e) {
            return {};
        }
    }
    
    // Méthodes de lecture MIDI
    bool startMidiPlayback() {
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
    
    void stopMidiPlayback() {
        midiPlaying = false;
        currentMidiPosition = 0;
    }
    
    bool isPlayingMidi() const {
        return midiPlaying;
    }
    
    // Accesseurs
    juce::MidiMessageSequence* getMidiSequence() const {
        return midiSequence.get();
    }
    
    double getCurrentPosition() const {
        return currentMidiPosition;
    }
    
    void setCurrentPosition(double position) {
        currentMidiPosition = position;
    }
    
    juce::MidiFile* getMidiFile() const {
        return midiFile.get();
    }
    
private:
    int currentTonality;
    bool isMajorMode;
    juce::String currentMidiFilePath;
    std::unique_ptr<juce::MidiFile> midiFile;
    std::unique_ptr<juce::MidiMessageSequence> midiSequence;
    bool midiPlaying;
    double currentMidiPosition;
}; 