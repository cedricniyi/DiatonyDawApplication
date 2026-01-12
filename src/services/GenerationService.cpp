#include "GenerationService.h"
#include "../controller/AppController.h"
#include "../model/DiatonyTypes.h"
#include "../model/Section.h"
#include "../model/Progression.h"
#include "../model/Chord.h"

// Point de contact unique avec la librairie Diatony
#include "../../Diatony/c++/headers/aux/Utilities.hpp"
#include "../../Diatony/c++/headers/aux/Tonality.hpp"
#include "../../Diatony/c++/headers/aux/MajorTonality.hpp"
#include "../../Diatony/c++/headers/aux/MinorTonality.hpp"
#include "../../Diatony/c++/headers/diatony/TonalProgressionParameters.hpp"
#include "../../Diatony/c++/headers/diatony/FourVoiceTextureParameters.hpp"
#include "../../Diatony/c++/headers/diatony/FourVoiceTexture.hpp"
#include "../../Diatony/c++/headers/diatony/ModulationParameters.hpp"
#include "../../Diatony/c++/headers/aux/MidiFileGeneration.hpp"
#include "../../Diatony/c++/headers/diatony/SolveDiatony.hpp"

struct GenerationService::Impl {
    bool initialized = false;
};

namespace {
    // Vérifie à la compilation que nos enums correspondent à ceux de Diatony
    #define VALIDATE_ENUM_MAPPING(ourEnum, theirEnum) \
        static_assert(static_cast<int>(ourEnum) == theirEnum, \
                      "Contrat rompu : " #ourEnum " != " #theirEnum)

    void runCompileTimeChecks()
    {
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::PerfectCadence, PERFECT_CADENCE_MODULATION);
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::PivotChord,     PIVOT_CHORD_MODULATION);
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::Alteration,     ALTERATION_MODULATION);
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::Chromatic,      CHROMATIC_MODULATION);

        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::First,   FIRST_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Second,  SECOND_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Third,   THIRD_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Fourth,  FOURTH_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Fifth,   FIFTH_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Sixth,   SIXTH_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Seventh, SEVENTH_DEGREE);

        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Major, MAJOR_CHORD);
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Minor, MINOR_CHORD);
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Diminished, DIMINISHED_CHORD);
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Augmented, AUGMENTED_CHORD);

        VALIDATE_ENUM_MAPPING(Diatony::ChordState::Fundamental, FUNDAMENTAL_STATE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordState::FirstInversion, FIRST_INVERSION);
        VALIDATE_ENUM_MAPPING(Diatony::ChordState::SecondInversion, SECOND_INVERSION);

        VALIDATE_ENUM_MAPPING(Diatony::Note::C, C);
        VALIDATE_ENUM_MAPPING(Diatony::Note::CSharp, C_SHARP);
        VALIDATE_ENUM_MAPPING(Diatony::Note::D, D);
        VALIDATE_ENUM_MAPPING(Diatony::Note::E, E);
        VALIDATE_ENUM_MAPPING(Diatony::Note::F, F);
        VALIDATE_ENUM_MAPPING(Diatony::Note::G, G);
        VALIDATE_ENUM_MAPPING(Diatony::Note::A, A);
        VALIDATE_ENUM_MAPPING(Diatony::Note::B, B);
    }

    #undef VALIDATE_ENUM_MAPPING
}

GenerationService::GenerationService() 
    : juce::Thread("Diatony Solver Thread"),
      pImpl(std::make_unique<Impl>()), 
      ready(false),
      generationSuccess(false)
{
    pImpl->initialized = true;
    ready = true;
    lastError.clear();
}

GenerationService::~GenerationService()
{
    stopThread(-1); // Attendre que le thread se termine
}

bool GenerationService::startGeneration(const Piece& piece, const juce::String& outputPath, AppController* controller)
{
    if (isThreadRunning())
    {
        lastError = "Une génération est déjà en cours";
        return false;
    }
    
    if (!ready)
    {
        lastError = "Service not ready";
        return false;
    }
    
    pieceToGenerate = &piece;
    outputPathToGenerate = outputPath;
    
    {
        juce::ScopedLock lock(callbackLock);
        appController = controller;
    }
    
    generationSuccess.store(false);
    lastError.clear();
    startThread();
    
    return true;
}

void GenerationService::run()
{
    if (pieceToGenerate == nullptr)
    {
        generationSuccess.store(false);
        lastError = "Piece invalide (nullptr)";
        return;
    }
    
    bool success = generateMidiFromPiece(*pieceToGenerate, outputPathToGenerate);
    generationSuccess.store(success);
    
    AppController* controllerToNotify = nullptr;
    {
        juce::ScopedLock lock(callbackLock);
        controllerToNotify = appController;
    }
    
    if (controllerToNotify != nullptr)
        controllerToNotify->triggerAsyncUpdate();
}

bool GenerationService::isGenerating() const { return isThreadRunning(); }
bool GenerationService::getLastGenerationSuccess() const { return generationSuccess.load(); }
juce::String GenerationService::getLastGeneratedMidiPath() const { return lastGeneratedMidiPath; }

bool GenerationService::generateMidiFromPiece(const Piece& piece, const juce::String& outputPath) {
    if (!ready) {
        lastError = "Service not ready";
        return false;
    }
    
    if (piece.isEmpty()) {
        lastError = "Piece is empty";
        return false;
    }
    
    if (piece.getSectionCount() == 0) {
        lastError = "Piece has no sections";
        return false;
    }
    
    // Validation : chaque section doit avoir au moins 2 accords
    // Justification : les contraintes harmoniques de Diatony (cadences V-I, voice leading)
    // nécessitent des transitions entre accords, donc minimum 2 par section.
    for (size_t i = 0; i < piece.getSectionCount(); ++i)
    {
        auto section = piece.getSection(static_cast<int>(i));
        int chordCount = static_cast<int>(section.getProgression().size());
        
        if (chordCount < 2)
        {
            lastError = juce::String("Section ") + juce::String(i + 1) 
                + " has " + juce::String(chordCount) 
                + " chord(s). Minimum 2 chords required per section for harmonic constraints.";
            return false;
        }
    }
    
    try {
        vector<TonalProgressionParameters*> sectionParamsList;
        int cumulativeChordIndex = 0;
        int totalChords = static_cast<int>(piece.getTotalChordCount());
        
        for (size_t i = 0; i < piece.getSectionCount(); ++i)
        {
            auto section = piece.getSection(static_cast<int>(i));
            int sectionChordCount = static_cast<int>(section.getProgression().size());
            
            int startChordIndex = cumulativeChordIndex;
            int endChordIndex = cumulativeChordIndex + sectionChordCount - 1;
            
            auto sectionParams = createSectionParams(
                section,
                static_cast<int>(i),
                startChordIndex,
                endChordIndex
            );
            
            sectionParamsList.push_back(sectionParams);
            cumulativeChordIndex += sectionChordCount;
        }
        
        vector<ModulationParameters*> modulations;
        
        for (size_t i = 0; i < piece.getModulationCount(); ++i)
        {
            auto modulationModel = piece.getModulation(static_cast<int>(i));
            auto [fromSection, toSection] = piece.getAdjacentSections(modulationModel);
            
            if (!fromSection.isValid() || !toSection.isValid())
                continue;
            
            int fromSectionId = modulationModel.getFromSectionId();
            int toSectionId = modulationModel.getToSectionId();
            int fromSectionIndex = piece.getSectionIndexById(fromSectionId);
            int toSectionIndex = piece.getSectionIndexById(toSectionId);
            
            if (fromSectionIndex < 0 || toSectionIndex < 0)
                continue;
            
            auto modulationType = modulationModel.getModulationType();
            int fromChordIndex = modulationModel.getFromChordIndex();
            int toChordIndex = modulationModel.getToChordIndex();
            
            int fromSectionSize = static_cast<int>(fromSection.getProgression().size());
            int toSectionSize = static_cast<int>(toSection.getProgression().size());
            
            if (fromSectionSize == 0 || toSectionSize == 0)
                continue;
            
            // Calcul automatique des indices selon le type de modulation
            if (modulationType == Diatony::ModulationType::PivotChord)
            {
                if (fromChordIndex == -1)
                    fromChordIndex = fromSectionSize - 1;
                if (toChordIndex == -1)
                    toChordIndex = (toSectionSize >= 2) ? 1 : 0;
            }
            else if (modulationType == Diatony::ModulationType::PerfectCadence)
            {
                // V-I dans la section source (avant-dernier → dernier accord)
                if (fromSectionSize < 2)
                    continue;
                fromChordIndex = fromSectionSize - 2;
                toChordIndex = fromSectionSize - 1;
            }
            else if (modulationType == Diatony::ModulationType::Alteration)
            {
                // Changement soudain : accords 1-2 de la section destination
                if (toSectionSize < 2)
                    continue;
                fromChordIndex = 0;
                toChordIndex = 1;
            }
            else if (modulationType == Diatony::ModulationType::Chromatic)
            {
                // Dernier accord source → premier accord destination
                fromChordIndex = fromSectionSize - 1;
                toChordIndex = 0;
            }
            
            // Validation des bornes selon le type
            int fromChordMaxIndex = fromSectionSize - 1;
            int toChordMaxIndex = toSectionSize - 1;
            
            if (modulationType == Diatony::ModulationType::PerfectCadence)
                toChordMaxIndex = fromSectionSize - 1;
            else if (modulationType == Diatony::ModulationType::Alteration)
                fromChordMaxIndex = toSectionSize - 1;
            
            if (fromChordIndex < 0 || fromChordIndex > fromChordMaxIndex ||
                toChordIndex < 0 || toChordIndex > toChordMaxIndex)
                continue;
            
            // Section de référence pour le calcul des indices globaux
            int fromChordSectionRef = fromSectionIndex;
            int toChordSectionRef = toSectionIndex;
            
            if (modulationType == Diatony::ModulationType::PerfectCadence)
                toChordSectionRef = fromSectionIndex;
            else if (modulationType == Diatony::ModulationType::Alteration)
                fromChordSectionRef = toSectionIndex;
            
            // Calcul des indices globaux cumulatifs
            int globalFromChordIndex = 0;
            for (int j = 0; j < fromChordSectionRef; ++j)
                globalFromChordIndex += static_cast<int>(piece.getSection(j).getProgression().size());
            globalFromChordIndex += fromChordIndex;
            
            int globalToChordIndex = 0;
            for (int j = 0; j < toChordSectionRef; ++j)
                globalToChordIndex += static_cast<int>(piece.getSection(j).getProgression().size());
            globalToChordIndex += toChordIndex;
            
            auto modulation = new ModulationParameters(
                static_cast<int>(modulationModel.getModulationType()),
                globalFromChordIndex,
                globalToChordIndex,
                sectionParamsList[fromSectionIndex],
                sectionParamsList[toSectionIndex]
            );
            
            modulations.push_back(modulation);
        }
        
        auto pieceParams = new FourVoiceTextureParameters(
            totalChords,
            static_cast<int>(piece.getSectionCount()),
            sectionParamsList,
            modulations
        );
        
        // Préparer le chemin de sauvegarde dans Application Support
        juce::File appSupportDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
            .getChildFile(APPLICATION_SUPPORT_PATH)
            .getChildFile("DiatonyDawApplication")
            .getChildFile("Solutions")
            .getChildFile("MidiFiles");
        
        if (!appSupportDir.exists())
            appSupportDir.createDirectory();
        
        auto now = juce::Time::getCurrentTime();
        juce::String timestamp = now.formatted("%Y%m%d_%H%M%S");
        juce::String fileName = "diatony_piece_" + timestamp + ".mid";
        juce::File midiFile = appSupportDir.getChildFile(fileName);
        juce::String finalPath = midiFile.getFullPathName();
        lastGeneratedMidiPath = finalPath;
        
        // Résolution avec Diatony
        auto solution = solve_diatony(pieceParams, nullptr, false);
        
        if (solution == nullptr) {
            lastError = "No solution found by Diatony solver";
            delete pieceParams;
            for (auto* sp : sectionParamsList) delete sp;
            for (auto* m : modulations) delete m;
            return false;
        }
        
        // Génération du fichier MIDI
        try {
            writeSolToMIDIFile(totalChords, finalPath.toStdString(), solution);
        } catch (const std::exception& e) {
            lastError = juce::String("Error writing MIDI file: ") + e.what();
            delete pieceParams;
            for (auto* sp : sectionParamsList) delete sp;
            for (auto* m : modulations) delete m;
            return false;
        }
        
        // Cleanup
        delete pieceParams;
        for (auto* sp : sectionParamsList) delete sp;
        for (auto* m : modulations) delete m;
        
        lastError.clear();
        return true;
        
    } catch (const std::exception& e) {
        lastError = juce::String("Error during generation: ") + e.what();
        return false;
    }
}

bool GenerationService::isReady() const { return ready && pImpl && pImpl->initialized; }
juce::String GenerationService::getLastError() const { return lastError; }

void GenerationService::reset()
{
    lastError.clear();
    if (pImpl)
        pImpl->initialized = true;
    ready = true;
}

Tonality* GenerationService::createTonalityFromSection(const Section& section)
{
    int tonic = static_cast<int>(section.getNote());
    return section.getIsMajor() ? static_cast<Tonality*>(new MajorTonality(tonic))
                                : static_cast<Tonality*>(new MinorTonality(tonic));
}

GenerationService::ChordVectors GenerationService::extractChordVectors(const Progression& progression, Tonality* tonality)
{
    ChordVectors result;
    
    for (size_t i = 0; i < progression.size(); ++i) {
        auto chord = progression.getChord(i);
        int degree = static_cast<int>(chord.getDegree());
        
        result.degrees.push_back(degree);
        result.states.push_back(static_cast<int>(chord.getChordState()));
        
        auto quality = chord.getQuality();
        if (quality == Diatony::ChordQuality::Auto)
            result.qualities.push_back(tonality->get_chord_quality(degree));
        else
            result.qualities.push_back(static_cast<int>(quality));
    }
    
    return result;
}

TonalProgressionParameters* GenerationService::createSectionParams(
    const Section& section, int sectionIndex, int startChordIndex, int endChordIndex)
{
    Tonality* tonality = createTonalityFromSection(section);
    auto progression = section.getProgression();
    auto chordVectors = extractChordVectors(progression, tonality);
    int numberOfChords = static_cast<int>(progression.size());
    
    return new TonalProgressionParameters(
        sectionIndex, numberOfChords, startChordIndex, endChordIndex,
        tonality, chordVectors.degrees, chordVectors.qualities, chordVectors.states
    );
}

void GenerationService::logGenerationInfo(const Piece& piece)
{
    std::cout << "=== PIECE INFO ===" << std::endl;
    std::cout << "Title: " << piece.getTitle() << std::endl;
    std::cout << "Sections: " << piece.getSectionCount() << std::endl;
    std::cout << "Modulations: " << piece.getModulationCount() << std::endl;
    std::cout << "Total chords: " << piece.getTotalChordCount() << std::endl;
    std::cout << piece.getDetailedSummary() << std::endl;
}

void* GenerationService::createDiatonyParametersFromPiece(const Piece& piece)
{
    (void)piece;
    return nullptr;
} 