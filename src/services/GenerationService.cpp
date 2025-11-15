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
#include "../../Diatony/c++/headers/diatony/FourVoiceTexture.hpp"
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
    
    if (piece.getSectionCount() == 0) {
        lastError = "Piece has no sections";
        DBG("❌ ERREUR : " << lastError);
        return false;
    }
    
    DBG("=================================================================");
    DBG("🎹 GÉNÉRATION MIDI - CONVERSION VERS DIATONY");
    DBG("=================================================================");
    DBG("📊 Pièce contient " << piece.getSectionCount() << " section(s)");
    DBG("");
    
    try {
        // ========================================
        // 2. TRAITER TOUTES LES SECTIONS
        // ========================================
        vector<TonalProgressionParameters*> sectionParamsList;
        int cumulativeChordIndex = 0;  // Index global cumulatif des accords
        int totalChords = static_cast<int>(piece.getTotalChordCount());
        
        DBG("📋 Traitement des sections :");
        DBG("-----------------------------------------------------------------");
        
        for (size_t i = 0; i < piece.getSectionCount(); ++i)
        {
            auto section = piece.getSection(static_cast<int>(i));
            int sectionChordCount = static_cast<int>(section.getProgression().size());
            
            // Calcul des indices globaux pour cette section
            int startChordIndex = cumulativeChordIndex;
            int endChordIndex = cumulativeChordIndex + sectionChordCount - 1;
            
            DBG("  Section " << (i + 1) << " :");
            DBG("    - Tonalité : " << section.toString());
            DBG("    - Nombre d'accords : " << sectionChordCount);
            DBG("    - Indices globaux : [" << startChordIndex << ", " << endChordIndex << "]");
            
            // ========================================
            // 3. CRÉER LES PARAMÈTRES POUR CETTE SECTION
            // ========================================
            auto sectionParams = createSectionParams(
                section,
                static_cast<int>(i),  // sectionIndex
                startChordIndex,       // startChordIndex (global)
                endChordIndex          // endChordIndex (global)
            );
            
            sectionParamsList.push_back(sectionParams);
            
            // ========================================
            // 4. LOGGER LES PARAMÈTRES DE CETTE SECTION
            // ========================================
            DBG("    📄 Paramètres (format lisible) :");
            std::cout << *sectionParams << std::endl;
            DBG("");
            
            // Avancer l'index cumulatif pour la prochaine section
            cumulativeChordIndex += sectionChordCount;
        }
        
        DBG("✅ Toutes les sections traitées");
        DBG("📊 Total des accords : " << totalChords);
        DBG("");
        
        // ========================================
        // 5. LIRE LES MODULATIONS DEPUIS LE MODÈLE
        // ========================================
        vector<ModulationParameters*> modulations;
        
        if (piece.getModulationCount() > 0)
        {
            DBG("🔄 Lecture des modulations depuis le modèle :");
            DBG("-----------------------------------------------------------------");
            
            for (size_t i = 0; i < piece.getModulationCount(); ++i)
            {
                auto modulationModel = piece.getModulation(static_cast<int>(i));
                
                // Récupérer les sections adjacentes via les IDs stockés
                auto [fromSection, toSection] = piece.getAdjacentSections(modulationModel);
                
                if (!fromSection.isValid() || !toSection.isValid())
                {
                    DBG("  ⚠️  Modulation " << (i + 1) << " ignorée : sections invalides");
                    continue;
                }
                
                // Trouver les indices des sections dans sectionParamsList
                int fromSectionId = modulationModel.getFromSectionId();
                int toSectionId = modulationModel.getToSectionId();
                
                int fromSectionIndex = piece.getSectionIndexById(fromSectionId);
                int toSectionIndex = piece.getSectionIndexById(toSectionId);
                
                if (fromSectionIndex < 0 || toSectionIndex < 0)
                {
                    DBG("  ⚠️  Modulation " << (i + 1) << " ignorée : impossible de trouver les indices des sections");
                    continue;
                }
                
                // Récupérer le type de modulation
                auto modulationType = modulationModel.getModulationType();
                
                // Récupérer les indices d'accords depuis le modèle
                int fromChordIndex = modulationModel.getFromChordIndex();
                int toChordIndex = modulationModel.getToChordIndex();
                
                // Tailles des sections
                int fromSectionSize = static_cast<int>(fromSection.getProgression().size());
                int toSectionSize = static_cast<int>(toSection.getProgression().size());
                
                // Validation des tailles
                if (fromSectionSize == 0)
                {
                    DBG("  ⚠️  Modulation " << (i + 1) << " ignorée : section source vide");
                    continue;
                }
                if (toSectionSize == 0)
                {
                    DBG("  ⚠️  Modulation " << (i + 1) << " ignorée : section destination vide");
                    continue;
                }
                
                // ====================================================================
                // CALCUL AUTOMATIQUE DES INDICES SELON LE TYPE DE MODULATION
                // ====================================================================
                
                // CAS 1 : Pivot Chord - Mode MANUEL (l'utilisateur a défini les indices)
                if (modulationType == Diatony::ModulationType::PivotChord)
                {
                    // Si l'utilisateur n'a pas défini les indices, utiliser des valeurs par défaut
                    if (fromChordIndex == -1)
                    {
                        fromChordIndex = fromSectionSize - 1;  // Dernier accord de la section source
                        DBG("  📝 PivotChord: fromChordIndex non défini, défaut = " << fromChordIndex);
                    }
                    if (toChordIndex == -1)
                    {
                        toChordIndex = (toSectionSize >= 2) ? 1 : 0;  // 2ème accord si possible, sinon 1er
                        DBG("  📝 PivotChord: toChordIndex non défini, défaut = " << toChordIndex);
                    }
                }
                
                // CAS 2 : Perfect Cadence - Mode AUTOMATIQUE
                // Intervalle : [avant-dernier accord de section i] → [dernier accord de section i]
                // Les DEUX accords sont dans la section SOURCE
                else if (modulationType == Diatony::ModulationType::PerfectCadence)
                {
                    if (fromSectionSize < 2)
                    {
                        DBG("  ⚠️  Modulation " << (i + 1) << " (Perfect Cadence) ignorée : section source < 2 accords");
                        continue;
                    }
                    fromChordIndex = fromSectionSize - 2;  // Avant-dernier accord de fromSection
                    toChordIndex = fromSectionSize - 1;    // Dernier accord de fromSection
                    
                    // IMPORTANT : Les deux indices sont relatifs à fromSection, donc on force toSectionIndex = fromSectionIndex
                    toSectionIndex = fromSectionIndex;
                    toSectionSize = fromSectionSize;
                    
                    DBG("  🤖 Perfect Cadence (auto): [" << fromChordIndex << " → " << toChordIndex << "] (V-I de section " << (fromSectionIndex + 1) << ")");
                }
                
                // CAS 3 : Alteration - Mode AUTOMATIQUE
                // Intervalle : [1er accord de section i+1] → [2ème accord de section i+1]
                // Les DEUX accords sont dans la section DESTINATION
                else if (modulationType == Diatony::ModulationType::Alteration)
                {
                    if (toSectionSize < 2)
                    {
                        DBG("  ⚠️  Modulation " << (i + 1) << " (Alteration) ignorée : section destination < 2 accords");
                        continue;
                    }
                    fromChordIndex = 0;  // 1er accord de toSection
                    toChordIndex = 1;    // 2ème accord de toSection
                    
                    // IMPORTANT : Les deux indices sont relatifs à toSection, donc on force fromSectionIndex = toSectionIndex
                    fromSectionIndex = toSectionIndex;
                    fromSectionSize = toSectionSize;
                    
                    DBG("  🤖 Alteration (auto): [" << fromChordIndex << " → " << toChordIndex << "] (début section " << (toSectionIndex + 1) << ")");
                }
                
                // CAS 4 : Chromatic - Mode AUTOMATIQUE
                // Intervalle : [dernier accord de section i] → [1er accord de section i+1]
                // Les accords sont dans DEUX SECTIONS DIFFÉRENTES (contrairement aux autres types auto)
                else if (modulationType == Diatony::ModulationType::Chromatic)
                {
                    fromChordIndex = fromSectionSize - 1;  // Dernier accord de fromSection (préparation)
                    toChordIndex = 0;                       // Premier accord de toSection (résolution sur V)
                    
                    // IMPORTANT : On GARDE les deux sections différentes (pas de "force")
                    // fromSectionIndex et toSectionIndex restent différents
                    
                    DBG("  🤖 Chromatic (auto): [" << fromChordIndex << " → " << toChordIndex << "] (dernier de section " << (fromSectionIndex + 1) << " → 1er de section " << (toSectionIndex + 1) << ")");
                }
                
                // Vérifier que les indices sont dans les bornes
                if (fromChordIndex < 0 || fromChordIndex >= fromSectionSize ||
                    toChordIndex < 0 || toChordIndex >= toSectionSize)
                {
                    DBG("  ⚠️  Modulation " << (i + 1) << " ignorée : indices d'accords hors limites");
                    DBG("    fromChordIndex=" << fromChordIndex << " (max=" << (fromSectionSize-1) << ")");
                    DBG("    toChordIndex=" << toChordIndex << " (max=" << (toSectionSize-1) << ")");
                    continue;
                }
                
                // Calculer les indices globaux cumulatifs
                int globalFromChordIndex = 0;
                for (int j = 0; j < fromSectionIndex; ++j)
                {
                    globalFromChordIndex += static_cast<int>(piece.getSection(j).getProgression().size());
                }
                globalFromChordIndex += fromChordIndex;
                
                int globalToChordIndex = 0;
                for (int j = 0; j < toSectionIndex; ++j)
                {
                    globalToChordIndex += static_cast<int>(piece.getSection(j).getProgression().size());
                }
                globalToChordIndex += toChordIndex;
                
                // Afficher les sections conceptuelles (du modèle)
                int originalFromSectionIndex = piece.getSectionIndexById(modulationModel.getFromSectionId());
                int originalToSectionIndex = piece.getSectionIndexById(modulationModel.getToSectionId());
                
                DBG("  Modulation " << (i + 1) << " :");
                DBG("    - Type : " << modulationModel.toString());
                DBG("    - Connecte Section " << (originalFromSectionIndex + 1) << " → Section " << (originalToSectionIndex + 1) << " (conceptuel)");
                DBG("    - Intervalle réel : Section " << (fromSectionIndex + 1) << " → Section " << (toSectionIndex + 1) << " (solveur)");
                DBG("    - Indices locaux : [accord " << (fromChordIndex + 1) << " de S" << (fromSectionIndex + 1) 
                    << " → accord " << (toChordIndex + 1) << " de S" << (toSectionIndex + 1) << "]");
                DBG("    - Indices globaux : [" << globalFromChordIndex << " → " << globalToChordIndex << "]");
                
                // Créer la modulation Diatony avec le type depuis le modèle
                auto modulation = new ModulationParameters(
                    static_cast<int>(modulationModel.getModulationType()),  // Utiliser le type du modèle
                    globalFromChordIndex,                                     // start (index global)
                    globalToChordIndex,                                       // end (index global)
                    sectionParamsList[fromSectionIndex],                     // from section
                    sectionParamsList[toSectionIndex]                        // to section
                );
                
                modulations.push_back(modulation);
                DBG("");
            }
            
            DBG("✅ " << modulations.size() << " modulation(s) chargée(s) depuis le modèle");
            DBG("");
        }
        else
        {
            DBG("ℹ️  Aucune modulation dans le modèle");
            DBG("");
        }
        
        // ========================================
        // 6. CRÉER LES PARAMÈTRES GLOBAUX
        // ========================================
        
        auto pieceParams = new FourVoiceTextureParameters(
            totalChords,                                  // totalNumberOfChords
            static_cast<int>(piece.getSectionCount()),   // numberOfSections
            sectionParamsList,                            // sectionParameters
            modulations                                   // modulationParameters
        );
        
        DBG("✅ FourVoiceTextureParameters créé");
        DBG("  - " << piece.getSectionCount() << " section(s)");
        DBG("  - " << modulations.size() << " modulation(s)");
        DBG("  - " << totalChords << " accord(s) total");
        DBG("");
        
        // ========================================
        // 7. LOGGER LES PARAMÈTRES GLOBAUX
        // ========================================
        DBG("📄 PARAMÈTRES GLOBAUX DE LA PIÈCE :");
        DBG("=================================================================");
        std::cout << pieceParams->toString() << std::endl;
        DBG("=================================================================");
        DBG("");
        
        // ========================================
        // 8. PRÉPARER LE CHEMIN DE SAUVEGARDE
        // ========================================
        
        // Créer le dossier dans Application Support
        juce::File appSupportDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
            .getChildFile(APPLICATION_SUPPORT_PATH)
            .getChildFile("DiatonyDawApplication")
            .getChildFile("Solutions")
            .getChildFile("MidiFiles");
        
        if (!appSupportDir.exists()) {
            appSupportDir.createDirectory();
            DBG("📁 Dossier créé : " << appSupportDir.getFullPathName());
        }
        
        // Générer un nom de fichier unique avec timestamp
        auto now = juce::Time::getCurrentTime();
        juce::String timestamp = now.formatted("%Y%m%d_%H%M%S");
        juce::String fileName = "diatony_piece_" + timestamp + ".mid";
        juce::File midiFile = appSupportDir.getChildFile(fileName);
        juce::String finalPath = midiFile.getFullPathName();
        
        DBG("📄 Fichier MIDI : " << finalPath);
        DBG("");
        
        // ========================================
        // 9. RÉSOLUTION AVEC DIATONY
        // ========================================
        DBG("🔍 Résolution du problème avec Diatony...");
        DBG("  - Utilisation des options par défaut (timeout: 60s)");
        DBG("");
        
        // Appel de solve_diatony avec options par défaut (nullptr)
        auto solution = solve_diatony(pieceParams, nullptr, false);
        
        if (solution == nullptr) {
            lastError = "No solution found by Diatony solver";
            DBG("❌ ERREUR : Aucune solution trouvée");
            DBG("");
            
            // Cleanup avant de retourner
            delete pieceParams;
            for (auto* sectionParams : sectionParamsList) {
                delete sectionParams;
            }
            for (auto* modulation : modulations) {
                delete modulation;
            }
            return false;
        }
        
        DBG("✅ Solution trouvée !");
        DBG("");
        
        // ========================================
        // 10. GÉNÉRATION DU FICHIER MIDI
        // ========================================
        DBG("🎼 Génération du fichier MIDI...");
        
        try {
            writeSolToMIDIFile(
                totalChords,
                finalPath.toStdString(),
                solution
            );
            
            DBG("✅ Fichier MIDI généré avec succès !");
            DBG("📁 Emplacement : " << finalPath);
            DBG("");
            
        } catch (const std::exception& e) {
            lastError = juce::String("Error writing MIDI file: ") + e.what();
            DBG("❌ ERREUR lors de l'écriture du fichier MIDI : " << e.what());
            
            // Cleanup
            delete pieceParams;
            for (auto* sectionParams : sectionParamsList) {
                delete sectionParams;
            }
            for (auto* modulation : modulations) {
                delete modulation;
            }
            return false;
        }
        
        // ========================================
        // 11. CLEANUP
        // ========================================
        // Note: On ne delete pas la tonalité car Tonality n'a pas de destructeur virtuel
        // et on ne sait pas qui possède le pointeur (TonalProgressionParameters ou nous)
        delete pieceParams;
        
        // Nettoyer tous les paramètres de sections
        for (auto* sectionParams : sectionParamsList) {
            delete sectionParams;
        }
        
        // Nettoyer toutes les modulations
        for (auto* modulation : modulations) {
            delete modulation;
        }
        
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