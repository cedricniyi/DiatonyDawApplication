#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include <atomic>
#include "../model/Piece.h"

// Forward declaration pour éviter l'include circulaire
class AppController;

/**
 * @file GenerationService.h
 * @brief Service de génération MIDI à partir du modèle Diatony (Thread Worker)
 * 
 * Cette classe agit comme un "Adapter" (pont) entre notre modèle fortement typé
 * et la librairie Diatony sous-jacente. Elle maintient un couplage faible en 
 * n'exposant aucune dépendance vers les headers externes dans son interface publique.
 * 
 * Design Pattern: Adapter Pattern + Background Thread
 * - Le service traduit les appels de notre API vers la librairie Diatony
 * - Seul le fichier .cpp connaît les détails de la librairie externe
 * - L'interface publique reste propre et indépendante
 * - Hérite de juce::Thread pour exécuter la génération en arrière-plan
 */
class GenerationService : public juce::Thread {
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
     * @brief Démarre la génération MIDI de manière asynchrone (nouveau point d'entrée)
     * 
     * Cette méthode lance la génération sur un thread séparé et retourne immédiatement.
     * Le callback vers l'AppController sera déclenché via AsyncUpdater quand c'est terminé.
     * 
     * @param piece La pièce musicale à convertir en MIDI
     * @param outputPath Le chemin de sortie pour le fichier MIDI
     * @param controller Pointeur vers l'AppController pour le callback
     * @return true si le thread a été lancé, false si une génération est déjà en cours
     */
    bool startGeneration(const Piece& piece, const juce::String& outputPath, AppController* controller);
    
    /**
     * @brief Vérifie si une génération est en cours
     * 
     * @return true si le thread de génération est actif
     */
    bool isGenerating() const;
    
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
    
    // === ACCESSEURS POUR LES RÉSULTATS (thread-safe) ===
    
    /**
     * @brief Obtient le résultat de la dernière génération
     * 
     * @return true si la dernière génération a réussi, false sinon
     */
    bool getLastGenerationSuccess() const;

protected:
    /**
     * @brief Implémentation du thread (appelée automatiquement par startThread)
     * 
     * Cette méthode contient toute la logique de génération MIDI.
     * Elle s'exécute sur un thread séparé et notifie l'AppController à la fin.
     */
    void run() override;

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
     * Si une qualité est en mode Auto, utilise tonality->get_chord_quality(degree)
     */
    ChordVectors extractChordVectors(const Progression& progression, class Tonality* tonality);
    
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
    
    /**
     * @brief Génère un fichier MIDI de manière synchrone (logique interne)
     * 
     * Cette méthode contient toute la logique de génération.
     * Elle est appelée depuis run() sur le thread worker.
     * 
     * @param piece La pièce musicale à convertir en MIDI
     * @param outputPath Le chemin de sortie pour le fichier MIDI
     * @return true si la génération a réussie, false sinon
     */
    bool generateMidiFromPiece(const Piece& piece, const juce::String& outputPath);
    
    // === ÉTAT INTERNE (original) ===
    mutable juce::String lastError;
    bool ready;
    
    // === DONNÉES POUR LE THREADING ===
    
    // Callback vers l'AppController (pour AsyncUpdater)
    AppController* appController = nullptr;
    
    // Données à générer (on garde une RÉFÉRENCE CONSTANTE - la Piece doit rester valide!)
    // Note: La Piece est possédée par l'AppController qui vit plus longtemps que ce thread
    const Piece* pieceToGenerate = nullptr;
    juce::String outputPathToGenerate;
    
    // Résultats de la génération (thread-safe avec std::atomic)
    std::atomic<bool> generationSuccess { false };
    
    // Synchronisation
    juce::CriticalSection callbackLock;  // Protection pour l'accès au callback
}; 