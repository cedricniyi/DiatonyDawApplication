#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include "../model/Piece.h"

/**
 * @file GenerationService.h
 * @brief Service de génération MIDI à partir du modèle Diatony
 * 
 * Cette classe agit comme un "Adapter" (pont) entre notre modèle fortement typé
 * et la librairie Diatony sous-jacente. Elle maintient un couplage faible en 
 * n'exposant aucune dépendance vers les headers externes dans son interface publique.
 * 
 * Design Pattern: Adapter Pattern
 * - Le service traduit les appels de notre API vers la librairie Diatony
 * - Seul le fichier .cpp connaît les détails de la librairie externe
 * - L'interface publique reste propre et indépendante
 */
class GenerationService {
public:
    /**
     * @brief Constructeur par défaut
     */
    GenerationService();
    
    /**
     * @brief Destructeur
     */
    ~GenerationService();
    
    /**
     * @brief Génère un fichier MIDI à partir d'une pièce musicale
     * 
     * Cette méthode est le point d'entrée principal du service. Elle prend
     * un objet Piece (notre modèle propre) et génère un fichier MIDI en
     * utilisant la librairie Diatony en arrière-plan.
     * 
     * @param piece La pièce musicale à convertir en MIDI
     * @param outputPath Le chemin de sortie pour le fichier MIDI
     * @return true si la génération a réussie, false sinon
     */
    bool generateMidiFromPiece(const Piece& piece, const juce::String& outputPath);
    
    /**
     * @brief Vérifie si le service est prêt pour la génération
     * 
     * @return true si le service peut effectuer des générations
     */
    bool isReady() const;
    
    /**
     * @brief Obtient la dernière erreur survenue
     * 
     * @return Description de la dernière erreur, ou chaîne vide si aucune erreur
     */
    juce::String getLastError() const;
    
    /**
     * @brief Réinitialise l'état du service
     */
    void reset();
    
    /**
     * @brief Affiche (log) toutes les informations de la pièce
     * 
     * Cette méthode est utilisée pour tester le flux de données complet
     * avant d'implémenter la génération réelle. Elle affiche dans la console
     * toutes les informations de la pièce musicale.
     * 
     * @param piece La pièce musicale à analyser et afficher
     */
    void logGenerationInfo(const Piece& piece);

private:
    // Implémentation cachée pour éviter les fuites de dépendances 
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Méthode privée de traduction (déclarée ici, implémentée dans le .cpp)
    // Cette méthode convertira notre Piece vers les paramètres Diatony
    void* createDiatonyParametersFromPiece(const Piece& piece);
    
    // === FONCTIONS DE CONVERSION (HELPERS) ===
    
    /**
     * Structure pour regrouper les vectors d'accords
     */
    struct ChordVectors {
        std::vector<int> degrees;
        std::vector<int> qualities;
        std::vector<int> states;
    };
    
    /**
     * Crée une Tonality* (MajorTonality ou MinorTonality) depuis une Section
     * ⚠️ IMPORTANT : Le pointeur doit être libéré par l'appelant (delete)
     */
    class Tonality* createTonalityFromSection(const Section& section);
    
    /**
     * Extrait les vectors d'accords depuis une Progression
     * Convertit nos enums vers les int attendus par Diatony
     */
    ChordVectors extractChordVectors(const Progression& progression);
    
    /**
     * Crée un TonalProgressionParameters* depuis une Section
     * ⚠️ IMPORTANT : Le pointeur doit être libéré par l'appelant (delete)
     */
    class TonalProgressionParameters* createSectionParams(
        const Section& section,
        int sectionIndex,
        int startChordIndex,
        int endChordIndex
    );
    
    // État interne
    mutable juce::String lastError;
    bool ready;
}; 