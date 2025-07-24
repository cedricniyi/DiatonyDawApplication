#include "Piece.h"

Piece::Piece() : title("Untitled Piece") {
}

Piece::Piece(const juce::String& pieceTitle) : title(pieceTitle) {
}

void Piece::addSection(const Section& section) {
    sections.push_back(section);
    
    // Connecter le callback de la section ajoutée
    sections.back().onSectionChanged = [this]() { notifyChange(); };
    
    // Si c'est la deuxième section ou plus, ajouter une modulation
    if (sections.size() > 1) {
        // Calculer les index globaux d'accords pour la modulation
        int startOfNewSection = getStartingChordIndexOfSection(sections.size() - 1);
        int fromChordGlobalIndex = startOfNewSection - 1;  // Dernier accord de la section précédente
        int toChordGlobalIndex = startOfNewSection;        // Premier accord de la nouvelle section
        
        // S'assurer que l'index de départ est valide
        if (fromChordGlobalIndex < 0) {
            fromChordGlobalIndex = 0;
        }
        
        Modulation newModulation(0, fromChordGlobalIndex, toChordGlobalIndex);
        modulations.push_back(newModulation);
        
        // Connecter le callback de la modulation ajoutée
        modulations.back().onModulationChanged = [this]() { notifyChange(); };
    }
    
    notifyChange();
}

void Piece::removeLastSection() {
    if (sections.empty()) return;
    
    // Supprimer la dernière section
    sections.pop_back();
    
    // Si il y avait une modulation vers cette section, la supprimer aussi
    if (!modulations.empty()) {
        modulations.pop_back();
    }
    
    notifyChange();
}

void Piece::clear() {
    sections.clear();
    modulations.clear();
    title = "Untitled Piece";
    notifyChange();
}

Section& Piece::getSection(size_t index) {
    if (index >= sections.size()) {
        throw std::out_of_range("Section index out of range");
    }
    return sections[index];
}

const Section& Piece::getSection(size_t index) const {
    if (index >= sections.size()) {
        throw std::out_of_range("Section index out of range");
    }
    return sections[index];
}

Modulation& Piece::getModulation(size_t index) {
    if (index >= modulations.size()) {
        throw std::out_of_range("Modulation index out of range");
    }
    return modulations[index];
}

const Modulation& Piece::getModulation(size_t index) const {
    if (index >= modulations.size()) {
        throw std::out_of_range("Modulation index out of range");
    }
    return modulations[index];
}

void Piece::setTitle(const juce::String& newTitle) {
    title = newTitle;
    notifyChange();
}

bool Piece::isComplete() const {
    if (sections.empty()) {
        return false;
    }
    
    // Vérifier l'invariant des modulations
    if (!hasValidStructure()) {
        return false;
    }
    
    // Toutes les sections doivent être valides
    for (const auto& section : sections) {
        if (!section.isValid()) {
            return false;
        }
    }
    
    return true;
}

bool Piece::hasValidSections() const {
    for (const auto& section : sections) {
        if (section.isValid()) {
            return true;
        }
    }
    return false;
}

bool Piece::hasValidStructure() const {
    // Vérifier l'invariant : modulations.size() == sections.size() - 1
    if (sections.empty()) {
        return modulations.empty();
    }
    
    return modulations.size() == sections.size() - 1;
}

int Piece::getTotalChordCount() const {
    int total = 0;
    for (const auto& section : sections) {
        total += static_cast<int>(section.getProgression().size());
    }
    return total;
}

juce::String Piece::toString() const {
    juce::String result = title + " - ";
    
    if (isEmpty()) {
        result += "Empty piece";
    } else {
        result += juce::String(sections.size()) + " sections";
        if (!modulations.empty()) {
            result += ", " + juce::String(modulations.size()) + " modulations";
        }
        result += ", " + juce::String(getTotalChordCount()) + " total chords";
    }
    
    return result;
}

juce::String Piece::getDetailedSummary() const {
    juce::String result = "Piece: " + title + "\n";
    
    if (isEmpty()) {
        result += "  No sections defined\n";
    } else {
        result += "  Sections (" + juce::String(sections.size()) + "):\n";
        for (size_t i = 0; i < sections.size(); ++i) {
            result += "    " + juce::String(i + 1) + ". " + sections[i].toString() + "\n";
            
            // Afficher la modulation si elle existe
            if (i < modulations.size()) {
                result += "       → " + modulations[i].toString() + "\n";
            }
        }
        result += "  Total chords: " + juce::String(getTotalChordCount()) + "\n";
        result += "  Structure valid: " + juce::String(hasValidStructure() ? "Yes" : "No");
    }
    
    return result;
}

void Piece::notifyChange() {
    if (onPieceChanged) {
        onPieceChanged();
    }
}

void Piece::connectAllCallbacks() {
    // Connecter tous les callbacks des sections
    for (auto& section : sections) {
        section.onSectionChanged = [this]() { notifyChange(); };
    }
    
    // Connecter tous les callbacks des modulations
    for (auto& modulation : modulations) {
        modulation.onModulationChanged = [this]() { notifyChange(); };
    }
}

int Piece::getStartingChordIndexOfSection(size_t sectionIndex) const {
    if (sectionIndex > sections.size()) {
        throw std::out_of_range("Section index is out of range for chord index calculation");
    }
    
    int total = 0;
    for (size_t i = 0; i < sectionIndex; ++i) {
        total += static_cast<int>(sections[i].getProgression().size());
    }
    return total;
}

std::vector<ConstChordReference> Piece::getChordsWithGlobalIndices(size_t sectionIndex) const {
    if (sectionIndex >= sections.size()) {
        throw std::out_of_range("Section index out of range");
    }

    std::vector<ConstChordReference> result;
    const int globalStartIndex = getStartingChordIndexOfSection(sectionIndex);
    const auto& progression = sections[sectionIndex].getProgression();

    result.reserve(progression.size());
    for (size_t i = 0; i < progression.size(); ++i) {
        result.push_back({progression.getChord(i), globalStartIndex + static_cast<int>(i)});
    }
    return result;
}

std::vector<ChordReference> Piece::getChordsWithGlobalIndices(size_t sectionIndex) {
    if (sectionIndex >= sections.size()) {
        throw std::out_of_range("Section index out of range");
    }

    std::vector<ChordReference> result;
    const int globalStartIndex = getStartingChordIndexOfSection(sectionIndex);
    auto& progression = sections[sectionIndex].getProgression();

    result.reserve(progression.size());
    for (size_t i = 0; i < progression.size(); ++i) {
        result.push_back({progression.getChord(i), globalStartIndex + static_cast<int>(i)});
    }
    return result;
}