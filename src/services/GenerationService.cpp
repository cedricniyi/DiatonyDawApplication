#include "GenerationService.h"
#include "../model/DiatonyTypes.h"
#include "../model/Section.h"
#include "../model/Progression.h"
#include "../model/Chord.h"

// ⚠️ POINT DE CONTACT UNIQUE AVEC LA LIBRAIRIE DIATONY ⚠️
// Ce fichier est le SEUL endroit du projet où nous incluons les headers Diatony
#include "../../Diatony/c++/headers/aux/Utilities.hpp"
#include "../../Diatony/c++/headers/aux/Tonality.hpp"
#include "../../Diatony/c++/headers/aux/MajorTonality.hpp"
#include "../../Diatony/c++/headers/aux/MinorTonality.hpp"
#include "../../Diatony/c++/headers/diatony/TonalProgressionParameters.hpp"
#include "../../Diatony/c++/headers/diatony/FourVoiceTextureParameters.hpp"
#include "../../Diatony/c++/headers/diatony/ModulationParameters.hpp"
#include "../../Diatony/c++/headers/aux/MidiFileGeneration.hpp"
#include "../../Diatony/c++/headers/diatony/SolveDiatony.hpp"


/**
 * @brief Structure d'implémentation cachée (Pimpl Pattern)
 * 
 * Cette structure contient tous les détails d'implémentation qui nécessitent
 * les headers Diatony, gardant l'interface publique propre.
 */
struct GenerationService::Impl {
    // Ici nous pourrions stocker des données spécifiques à Diatony si nécessaire
    bool initialized = false;
};

namespace { // Namespace anonyme pour confiner nos outils de vérification
    /**
     * @brief Macro pour valider la correspondance des enums à la compilation.
     * @param ourEnum Notre type enum interne (ex: Diatony::ModulationType::PerfectCadence)
     * @param theirEnum L'enum correspondant de la librairie externe (ex: PERFECT_CADENCE_MODULATION)
     */
    #define VALIDATE_ENUM_MAPPING(ourEnum, theirEnum) \
        static_assert(static_cast<int>(ourEnum) == theirEnum, \
                      "Contrat rompu : " #ourEnum " ne correspond pas à " #theirEnum)

    void runCompileTimeChecks()
    {
        // 🛡️ FILET DE SÉCURITÉ COMPACT 🛡️
        // Vérification des Modulations (Erreur corrigée)
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::PerfectCadence, PERFECT_CADENCE_MODULATION); // CORRIGÉ
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::PivotChord,     PIVOT_CHORD_MODULATION);
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::Alteration,     ALTERATION_MODULATION);
        VALIDATE_ENUM_MAPPING(Diatony::ModulationType::Chromatic,      CHROMATIC_MODULATION);

        // Vérification des Degrés
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::First,   FIRST_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Second,  SECOND_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Third,   THIRD_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Fourth,  FOURTH_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Fifth,   FIFTH_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Sixth,   SIXTH_DEGREE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordDegree::Seventh, SEVENTH_DEGREE);

        // Vérification des Qualités
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Major, MAJOR_CHORD);
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Minor, MINOR_CHORD);
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Diminished, DIMINISHED_CHORD);
        VALIDATE_ENUM_MAPPING(Diatony::ChordQuality::Augmented, AUGMENTED_CHORD);

        // Vérification des États
        VALIDATE_ENUM_MAPPING(Diatony::ChordState::Fundamental, FUNDAMENTAL_STATE);
        VALIDATE_ENUM_MAPPING(Diatony::ChordState::FirstInversion, FIRST_INVERSION);
        VALIDATE_ENUM_MAPPING(Diatony::ChordState::SecondInversion, SECOND_INVERSION);

        // Vérification des Notes
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
} // fin namespace anonyme


// Constructeur
GenerationService::GenerationService() 
    : pImpl(std::make_unique<Impl>()), ready(false) {
    pImpl->initialized = true;
    ready = true;
    lastError.clear();
}

// Destructeur
GenerationService::~GenerationService() = default;

bool GenerationService::generateMidiFromPiece(const Piece& piece, const juce::String& outputPath) {
    // ========================================
    // 1. VALIDATION
    // ========================================
    if (!ready) {
        lastError = "Service not ready";
        return false;
    }
    
    if (piece.isEmpty()) {
        lastError = "Piece is empty";
        return false;
    }
    
    if (piece.getSectionCount() != 1) {
        lastError = "Only 1 section supported for now (got " + 
                    juce::String(static_cast<int>(piece.getSectionCount())) + ")";
        DBG("❌ ERREUR : " << lastError);
        return false;
    }
    
    DBG("=================================================================");
    DBG("🎹 GÉNÉRATION MIDI - CONVERSION VERS DIATONY");
    DBG("=================================================================");
    
    try {
        // ========================================
        // 2. RÉCUPÉRER LA SECTION
        // ========================================
        auto section = piece.getSection(0);
        int totalChords = static_cast<int>(section.getProgression().size());
        
        DBG("📊 Section récupérée :");
        DBG("  - Nombre d'accords : " << totalChords);
        DBG("  - Tonalité : " << section.toString());
        DBG("");
        
        // ========================================
        // 3. CRÉER LES PARAMÈTRES DE SECTION
        // ========================================
        auto sectionParams = createSectionParams(
            section,
            0,              // sectionIndex = 0 (première section)
            0,              // startChordIndex = 0 (commence au début)
            totalChords - 1 // endChordIndex = totalChords - 1
        );
        
        DBG("✅ TonalProgressionParameters créé");
        DBG("");
        
        // ========================================
        // 4. LOGGER LES PARAMÈTRES (version pretty)
        // ========================================
        DBG("📄 PARAMÈTRES DE LA SECTION (format lisible) :");
        DBG("-----------------------------------------------------------------");
        std::cout << *sectionParams << std::endl;  // Utilise operator<<
        DBG("");
        
        DBG("📄 PARAMÈTRES DE LA SECTION (format complet) :");
        DBG("-----------------------------------------------------------------");
        std::cout << sectionParams->to_string() << std::endl;
        DBG("");
        
        // ========================================
        // 5. CRÉER LES PARAMÈTRES GLOBAUX
        // ========================================
        vector<TonalProgressionParameters*> sections = { sectionParams };
        vector<ModulationParameters*> modulations = {};  // Pas de modulation
        
        auto pieceParams = new FourVoiceTextureParameters(
            totalChords,    // totalNumberOfChords
            1,              // numberOfSections
            sections,       // sectionParameters
            modulations     // modulationParameters (vide)
        );
        
        DBG("✅ FourVoiceTextureParameters créé");
        DBG("");
        
        // ========================================
        // 6. LOGGER LES PARAMÈTRES GLOBAUX
        // ========================================
        DBG("📄 PARAMÈTRES GLOBAUX DE LA PIÈCE :");
        DBG("=================================================================");
        std::cout << pieceParams->toString() << std::endl;
        DBG("=================================================================");
        DBG("");
        
        // ========================================
        // 7. TODO: RÉSOLUTION ET GÉNÉRATION MIDI
        // ========================================
        DBG("⏸️  Génération MIDI non implémentée (prochaine étape)");
        DBG("");
        
        // ========================================
        // 8. CLEANUP
        // ========================================
        // Note: On ne delete pas la tonalité car Tonality n'a pas de destructeur virtuel
        // et on ne sait pas qui possède le pointeur (TonalProgressionParameters ou nous)
        // TODO: Vérifier la gestion de la mémoire dans Diatony
        delete pieceParams;
        delete sectionParams;
        
        lastError.clear();
        return true;
        
    } catch (const std::exception& e) {
        lastError = juce::String("Error during generation: ") + e.what();
        DBG("❌ ERREUR : " << lastError);
        return false;
    }
}

bool GenerationService::isReady() const {
    return ready && pImpl && pImpl->initialized;
}

juce::String GenerationService::getLastError() const {
    return lastError;
}

void GenerationService::reset() {
    lastError.clear();
    if (pImpl) {
        pImpl->initialized = true;
    }
    ready = true;
}

// ========================================
// FONCTIONS DE CONVERSION (HELPERS)
// ========================================

/**
 * Crée une Tonality* (MajorTonality ou MinorTonality) depuis une Section
 * ⚠️ IMPORTANT : Le pointeur doit être libéré par l'appelant (delete)
 */
Tonality* GenerationService::createTonalityFromSection(const Section& section)
{
    int tonic = static_cast<int>(section.getNote());
    
    if (section.getIsMajor()) {
        return new MajorTonality(tonic);
    } else {
        return new MinorTonality(tonic);
    }
}

/**
 * Extrait les vectors d'accords depuis une Progression
 * Convertit nos enums vers les int attendus par Diatony
 */
GenerationService::ChordVectors GenerationService::extractChordVectors(const Progression& progression)
{
    ChordVectors result;
    
    for (size_t i = 0; i < progression.size(); ++i) {
        auto chord = progression.getChord(i);
        
        // Conversion enum → int (cast direct car nos enums matchent Diatony)
        result.degrees.push_back(static_cast<int>(chord.getDegree()));
        result.qualities.push_back(static_cast<int>(chord.getQuality()));
        result.states.push_back(static_cast<int>(chord.getChordState()));  // Corrigé: getChordState() au lieu de getState()
    }
    
    return result;
}

/**
 * Crée un TonalProgressionParameters* depuis une Section
 * Pour une seule section : start=0, end=size-1
 * ⚠️ IMPORTANT : Le pointeur doit être libéré par l'appelant (delete)
 */
TonalProgressionParameters* GenerationService::createSectionParams(
    const Section& section,
    int sectionIndex,
    int startChordIndex,
    int endChordIndex
)
{
    // 1. Créer la tonalité
    Tonality* tonality = createTonalityFromSection(section);
    
    // 2. Extraire les accords
    auto progression = section.getProgression();
    auto chordVectors = extractChordVectors(progression);
    
    // 3. Créer les paramètres
    int numberOfChords = static_cast<int>(progression.size());
    
    auto params = new TonalProgressionParameters(
        sectionIndex,                   // progression_number
        numberOfChords,                 // size
        startChordIndex,                // start (global)
        endChordIndex,                  // end (global)
        tonality,                       // tonality*
        chordVectors.degrees,           // chord_degrees
        chordVectors.qualities,         // chord_qualities
        chordVectors.states             // chord_states
    );
    
    return params;
}

// ========================================
// LOGGING
// ========================================

void GenerationService::logGenerationInfo(const Piece& piece) {
    DBG("=================================================================");
    DBG("📄 INFORMATIONS DE LA PIÈCE MUSICALE");
    DBG("=================================================================");
    
    // Titre
    DBG("🎵 Titre : " << piece.getTitle());
    DBG("");
    
    // Nombre d'éléments
    DBG("📊 Structure :");
    DBG("  - Nombre de sections : " << piece.getSectionCount());
    DBG("  - Nombre de modulations : " << piece.getModulationCount());
    DBG("  - Nombre total d'accords : " << piece.getTotalChordCount());
    DBG("");
    
    // Détails des sections
    DBG("📑 DÉTAILS DES SECTIONS :");
    DBG("-----------------------------------------------------------------");
    for (size_t i = 0; i < piece.getSectionCount(); ++i) {
        auto section = piece.getSection(static_cast<int>(i));
        DBG("  Section " << (i + 1) << " : " << section.toString());
        DBG("");
    }
    
    // Détails des modulations
    if (piece.getModulationCount() > 0) {
        DBG("🔄 DÉTAILS DES MODULATIONS :");
        DBG("-----------------------------------------------------------------");
        for (size_t i = 0; i < piece.getModulationCount(); ++i) {
            auto modulation = piece.getModulation(static_cast<int>(i));
            DBG("  Modulation " << (i + 1) << " : " << modulation.toString());
        }
        DBG("");
    }
    
    // Résumé global
    DBG("📝 RÉSUMÉ GLOBAL :");
    DBG("-----------------------------------------------------------------");
    DBG(piece.getDetailedSummary());
    DBG("");
    
    // État du service
    DBG("=================================================================");
    DBG("⚙️  État du service :");
    DBG("  - Service prêt : " << (isReady() ? "✓" : "✗"));
    DBG("  - Dernière erreur : " << (lastError.isEmpty() ? "(aucune)" : lastError));
    DBG("=================================================================");
    DBG("");
}

void* GenerationService::createDiatonyParametersFromPiece(const Piece& piece) {
    // TODO: Implémenter la traduction Piece -> FourVoiceTextureParameters
    // Cette méthode sera le cœur de la traduction entre notre modèle et Diatony
    
    (void)piece; // Éviter warning unused parameter pour le moment
    return nullptr;
} 