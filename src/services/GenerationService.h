#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include <atomic>
#include "../model/Piece.h"

class AppController;

/**
 * @brief Service de génération MIDI via le solveur Diatony (thread worker).
 *
 * Pattern Adapter : traduit notre modèle Piece vers la librairie Diatony.
 * Seul le .cpp inclut les headers Diatony (couplage faible).
 */
class GenerationService : public juce::Thread {
public:
    GenerationService();
    ~GenerationService();
    
    /** @brief Lance la génération asynchrone ; retourne false si déjà en cours. */
    bool startGeneration(const Piece& piece, const juce::String& outputPath, AppController* controller);
    
    bool isGenerating() const;
    bool isReady() const;
    juce::String getLastError() const;
    void reset();
    
    /** @brief Log console de la pièce (debug). */
    void logGenerationInfo(const Piece& piece);
    
    bool getLastGenerationSuccess() const;
    juce::String getLastGeneratedMidiPath() const;

protected:
    void run() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void* createDiatonyParametersFromPiece(const Piece& piece);
    
    struct ChordVectors {
        std::vector<int> degrees;
        std::vector<int> qualities;
        std::vector<int> states;
    };
    
    /** @brief Crée Tonality* depuis Section. L'appelant doit delete. */
    class Tonality* createTonalityFromSection(const Section& section);
    
    /** @brief Extrait les vecteurs d'accords ; qualité Auto → tonality->get_chord_quality(). */
    ChordVectors extractChordVectors(const Progression& progression, class Tonality* tonality);
    
    /** @brief Crée TonalProgressionParameters*. L'appelant doit delete. */
    class TonalProgressionParameters* createSectionParams(
        const Section& section, int sectionIndex, int startChordIndex, int endChordIndex);
    
    bool generateMidiFromPiece(const Piece& piece, const juce::String& outputPath);
    
    mutable juce::String lastError;
    bool ready;
    
    AppController* appController = nullptr;
    const Piece* pieceToGenerate = nullptr;
    juce::String outputPathToGenerate;
    
    std::atomic<bool> generationSuccess { false };
    juce::String lastGeneratedMidiPath;
    juce::CriticalSection callbackLock;
}; 