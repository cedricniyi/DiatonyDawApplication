#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../../Diatony/c++/headers/aux/MidiFileGeneration.hpp"
#include "../../Diatony/c++/headers/diatony/SolveDiatony.hpp"
#include "../../Diatony/c++/headers/diatony/FourVoiceTexture.hpp"
#include "../../Diatony/c++/headers/aux/Tonality.hpp"
#include "../../Diatony/c++/headers/aux/MajorTonality.hpp"
#include "../../Diatony/c++/headers/aux/MinorTonality.hpp"
#include "DiatonyModel.h"
#include "../utils/FileUtils.h"
#include "../utils/DatabaseManagement.h"

#ifndef APPLICATION_SUPPORT_PATH
    #error "APPLICATION_SUPPORT_PATH n'est pas défini"
#endif

/**
 * Service spécialisé pour la génération de solutions MIDI via Diatony
 * et la lecture de fichiers MIDI
 */
class ChordSequence {
public:
    ChordSequence() : midiPlaying(false), currentMidiPosition(0) {}
    
    // Méthode de génération - prend maintenant DiatonyModel en paramètre
    juce::String generateMidiSolution(const DiatonyModel& model) {
        // Vérifier que le modèle est complet
        if (!model.isComplete()) {
            return {};
        }
        
        // Récupérer les données du modèle
        const auto& progression = model.getProgression();
        auto chords = progression.getChords();
        auto states = progression.getStates();
        auto customQualities = progression.getQualities();
        int tonality = model.getTonality();
        bool isMajorMode = model.getIsMajor();
        
        // Obtenir le dossier Application Support
        juce::File appSupportDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                          .getChildFile(APPLICATION_SUPPORT_PATH)
                          .getChildFile("DiatonyDawApplication")
                          .getChildFile("Solutions")
                          .getChildFile("MidiFiles");

        // Créer l'arborescence de dossiers si nécessaire
        if (!appSupportDir.exists()) {
            appSupportDir.createDirectory();  // createDirectory() crée aussi les dossiers parents si nécessaire
        }

        // Chemin complet avec nom de fichier
        juce::String fullPath = appSupportDir.getFullPathName() 
                              + juce::File::getSeparatorString() 
                              + "solfromjuce";
                              
        // Créer la tonalité appropriée
        std::unique_ptr<MajorTonality> majorTonality;
        std::unique_ptr<MinorTonality> minorTonality;
        Tonality* tonalityPtr = nullptr;
        
        if (isMajorMode) {
            majorTonality = std::make_unique<MajorTonality>(tonality);
            tonalityPtr = majorTonality.get();
        } else {
            minorTonality = std::make_unique<MinorTonality>(tonality);
            tonalityPtr = minorTonality.get();
        }
        
        // Préparer les qualités d'accords
        std::vector<int> chordQualities;
        for(size_t i = 0; i < chords.size(); ++i) {
            if (i < customQualities.size() && customQualities[i] != -1) {
                chordQualities.push_back(customQualities[i]);
            } else {
                chordQualities.push_back(tonalityPtr->get_chord_quality(chords[i]));
            }
        }
        
        int size = chords.size();
        
        // Créer les paramètres pour la section tonale avec unique_ptr
        auto sectionParams = std::make_unique<TonalProgressionParameters>(
            0,                                          // sectionId
            size,                                       // size
            0,                                          // start
            size - 1,                                   // end
            tonalityPtr,                                // tonality
            chords,                                     // chords
            chordQualities,                             // qualities
            states                                      // states
        );
        
        // Créer les paramètres pour la texture à quatre voix (pas de modulation pour une section simple)
        std::vector<TonalProgressionParameters*> sectionParamsVector = {sectionParams.get()};
        std::vector<ModulationParameters*> modulationParams = {}; // Vide pour une section simple
        
        auto pieceParams = std::make_unique<FourVoiceTextureParameters>(
            size,                                       // totalNumberOfChords
            1,                                          // numberOfSections
            sectionParamsVector,                        // sectionParameters
            modulationParams                            // modulationParameters
        );
        
        try {
            // Obtention de la solution (utilise les options par défaut de solve_diatony)
            auto solution = solve_diatony(pieceParams.get(), nullptr, true);
            
            if (solution != nullptr) {
                // Utilisation de FileUtils pour générer un nom de fichier unique avec timestamp
                juce::String finalPath = FileUtils::generateUniqueFilename(fullPath);
                juce::String finalPathWithExtension = finalPath + ".mid";
                writeSolToMIDIFile(solution->getParameters()->get_totalNumberOfChords(), 
                                 finalPathWithExtension.toStdString(), solution);
                
                currentMidiFilePath = finalPathWithExtension;
                
                // Extraire le nom du fichier du chemin complet
                juce::File file(finalPathWithExtension);
                juce::String fileName = file.getFileName();
                
                // Sauvegarder les métadonnées dans la base de données JSON
                Db::addSolution(fileName, finalPathWithExtension);
                
                return finalPathWithExtension;
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
    
    
    // Méthode pour charger une solution spécifique par son chemin
    bool loadSolutionByPath(const juce::String& path) {
        currentMidiFilePath = path;
        return startMidiPlayback();
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
    juce::String currentMidiFilePath;
    std::unique_ptr<juce::MidiFile> midiFile;
    std::unique_ptr<juce::MidiMessageSequence> midiSequence;
    bool midiPlaying;
    double currentMidiPosition;
}; 