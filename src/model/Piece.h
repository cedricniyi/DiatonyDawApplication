#pragma once

#include <vector>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "Section.h"
#include "Modulation.h"
#include "PieceElement.h"

// Nouvelle structure pour encapsuler un accord et son index global
struct ChordReference {
    Chord& chord;
    int globalIndex;
};

// Version const pour les accès en lecture seule
struct ConstChordReference {
    const Chord& chord;
    int globalIndex;
};

/**
 * Représente une pièce musicale complète avec ses sections tonales et modulations
 * Maintient l'invariant : modulations.size() == sections.size() - 1
 * Utilise maintenant une collection unifiée PieceElement pour l'ordre chronologique
 */
class Piece {
public:
    Piece();
    explicit Piece(const juce::String& pieceTitle);
    
    // Gestion des sections (API préservée pour compatibilité)
    void addSection(const Section& section);
    void addSection(std::unique_ptr<Section> section);  // Nouvelle surcharge
    void removeLastSection();  // Supprime section et modulation associée
    void clearAllElements();   // Nouvelle méthode unifiée
    void clear();             // Conservée pour compatibilité
    
    // Nouvelle API unifiée pour les éléments
    int getNumElements() const { return elements.size(); }
    PieceElement* getElement(int index) const { return elements[index]; }
    const juce::OwnedArray<PieceElement>& getElements() const { return elements; }
    
    // API préservée pour compatibilité (reconstruite dynamiquement)
    std::vector<Section*> getSections() const;
    std::vector<Modulation*> getModulations() const;
    
    Section& getSection(size_t index);
    const Section& getSection(size_t index) const;
    
    Modulation& getModulation(size_t index);
    const Modulation& getModulation(size_t index) const;
    
    // Accès aux accords avec index globaux
    std::vector<ConstChordReference> getChordsWithGlobalIndices(size_t sectionIndex) const;
    std::vector<ChordReference> getChordsWithGlobalIndices(size_t sectionIndex);
    
    // Informations sur la pièce
    size_t getSectionCount() const;    // Implémentation déplacée dans .cpp
    size_t getModulationCount() const; // Implémentation déplacée dans .cpp  
    bool isEmpty() const { return elements.isEmpty(); }
    
    void setTitle(const juce::String& newTitle);
    const juce::String& getTitle() const { return title; }
    
    // Validation
    bool isComplete() const;
    bool hasValidStructure() const;  // Vérifie l'invariant des modulations
    
    // Statistiques
    int getTotalChordCount() const;
    
    // Méthodes utilitaires
    juce::String toString() const;
    juce::String getDetailedSummary() const;
    
    // Callback pour notifier les changements
    std::function<void()> onPieceChanged;
    
private:
    juce::String title;
    juce::OwnedArray<PieceElement> elements;  // Collection unifiée : Section, Modulation, Section, etc.
    
    void notifyChange();
    void connectAllCallbacks();  // Helper pour reconnecter tous les callbacks
    int getStartingChordIndexOfSection(size_t sectionIndex) const;
    
    // Helpers pour la nouvelle architecture  
    Section* getSectionAtElementIndex(int elementIndex) const;     // Cast sécurisé vers Section
    Modulation* getModulationAtElementIndex(int elementIndex) const; // Cast sécurisé vers Modulation
}; 