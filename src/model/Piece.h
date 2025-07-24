#pragma once

#include <vector>
#include <juce_core/juce_core.h>
#include "Section.h"
#include "Modulation.h"

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
 */
class Piece {
public:
    Piece();
    explicit Piece(const juce::String& pieceTitle);
    
    // Gestion des sections
    void addSection(const Section& section);
    void removeLastSection();  // Simplifié : supprime seulement la dernière section
    void clear();
    
    // Accesseurs pour les sections
    const std::vector<Section>& getSections() const { return sections; }
    std::vector<Section>& getSections() { return sections; }
    
    Section& getSection(size_t index);
    const Section& getSection(size_t index) const;
    
    // Accesseurs pour les modulations
    const std::vector<Modulation>& getModulations() const { return modulations; }
    std::vector<Modulation>& getModulations() { return modulations; }
    
    Modulation& getModulation(size_t index);
    const Modulation& getModulation(size_t index) const;
    
    // Accès aux accords avec index globaux
    std::vector<ConstChordReference> getChordsWithGlobalIndices(size_t sectionIndex) const;
    std::vector<ChordReference> getChordsWithGlobalIndices(size_t sectionIndex);
    
    // Informations sur la pièce
    size_t getSectionCount() const { return sections.size(); }
    size_t getModulationCount() const { return modulations.size(); }
    bool isEmpty() const { return sections.empty(); }
    
    void setTitle(const juce::String& newTitle);
    const juce::String& getTitle() const { return title; }
    
    // Validation
    bool isComplete() const;
    bool hasValidSections() const;
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
    std::vector<Section> sections;
    std::vector<Modulation> modulations;  // Invariant : modulations.size() == sections.size() - 1
    
    void notifyChange();
    void connectAllCallbacks();  // Helper pour reconnecter tous les callbacks
    int getStartingChordIndexOfSection(size_t sectionIndex) const;
}; 