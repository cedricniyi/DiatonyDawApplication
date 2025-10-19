#include "GenerationService.h"
#include "../model/DiatonyTypes.h"

// ⚠️ POINT DE CONTACT UNIQUE AVEC LA LIBRAIRIE DIATONY ⚠️
// Ce fichier est le SEUL endroit du projet où nous incluons les headers Diatony
#include "../../Diatony/c++/headers/aux/Utilities.hpp"
#include "../../Diatony/c++/headers/diatony/FourVoiceTextureParameters.hpp"
#include "../../Diatony/c++/headers/aux/MidiFileGeneration.hpp"
#include "../../Diatony/c++/headers/diatony/SolveDiatony.hpp" // Ajout du solveur


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
    if (!ready) {
        lastError = "Service not ready";
        return false;
    }
    
    if (piece.isEmpty()) {
        lastError = "Piece is empty";
        return false;
    }
    
    try {
        // TODO: Implémenter la logique de génération MIDI
        // 1. Convertir Piece vers FourVoiceTextureParameters
        // 2. Appeler solve_diatony()
        // 3. Générer le fichier MIDI
        
        lastError = "Generation not yet implemented";
        return false;
        
    } catch (const std::exception& e) {
        lastError = juce::String("Error during generation: ") + e.what();
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