#include "Piece.h"

Piece::Piece() : title("Untitled Piece") {
}

Piece::Piece(const juce::String& pieceTitle) : title(pieceTitle) {
}

void Piece::addSection(const Section& section) {
    // Créer une copie de la section
    auto newSection = std::make_unique<Section>(section);
    addSection(std::move(newSection));
}

void Piece::addSection(std::unique_ptr<Section> section) {
    // Si c'est la deuxième section ou plus, ajouter d'abord une modulation
    if (getSectionCount() > 0) {
        // Calculer les index globaux d'accords pour la modulation
        int currentSectionCount = getSectionCount();
        int startOfNewSection = getStartingChordIndexOfSection(currentSectionCount);
        int fromChordGlobalIndex = startOfNewSection - 1;  // Dernier accord de la section précédente
        int toChordGlobalIndex = startOfNewSection;        // Premier accord de la nouvelle section
        
        // S'assurer que l'index de départ est valide
        if (fromChordGlobalIndex < 0) {
            fromChordGlobalIndex = 0;
        }
        
        auto newModulation = std::make_unique<Modulation>(
            Diatony::ModulationType::PerfectCadence, 
            fromChordGlobalIndex, 
            toChordGlobalIndex
        );
        
        // Connecter le callback unifié de la modulation
        newModulation->onElementChanged = [this]() { notifyChange(); };
        
        // Ajouter la modulation à elements
        elements.add(newModulation.release());
    }
    
    // Connecter le callback unifié de la section
    section->onElementChanged = [this]() { notifyChange(); };
    
    // Ajouter la section à elements
    elements.add(section.release());
    
    notifyChange();
}

void Piece::removeLastSection() {
    if (elements.isEmpty()) return;
    
    // Vérifier que le dernier élément est bien une Section
    auto* lastElement = elements.getLast();
    if (lastElement->getType() != PieceElement::Type::Section) {
        // Si le dernier élément n'est pas une Section, quelque chose ne va pas
        // dans la structure. On ne fait rien pour éviter de corrompre davantage.
        return;
    }
    
    // Supprimer la dernière section
    elements.removeLast();
    
    // Si il reste des éléments ET que le nouveau dernier élément est une Modulation,
    // alors il faut aussi la supprimer pour maintenir l'invariant
    if (!elements.isEmpty()) {
        auto* newLastElement = elements.getLast();
        if (newLastElement->getType() == PieceElement::Type::Modulation) {
            elements.removeLast();
        }
    }
    
    notifyChange();
}

void Piece::clearAllElements() {
    elements.clear();
    notifyChange();
}

void Piece::clear() {
    clearAllElements();
    title = "Untitled Piece";
    notifyChange();
}

const Section& Piece::getSection(size_t index) const {
    size_t sectionIndex = 0;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Section) {
            if (sectionIndex == index) {
                return *static_cast<const Section*>(elements[i]);
            }
            sectionIndex++;
        }
    }
    throw std::out_of_range("Section index out of range");
}

Section& Piece::getSection(size_t index) {
    return const_cast<Section&>(static_cast<const Piece*>(this)->getSection(index));
}

const Modulation& Piece::getModulation(size_t index) const {
    size_t modulationIndex = 0;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Modulation) {
            if (modulationIndex == index) {
                return *static_cast<const Modulation*>(elements[i]);
            }
            modulationIndex++;
        }
    }
    throw std::out_of_range("Modulation index out of range");
}

Modulation& Piece::getModulation(size_t index) {
    return const_cast<Modulation&>(static_cast<const Piece*>(this)->getModulation(index));
}

void Piece::setTitle(const juce::String& newTitle) {
    title = newTitle;
    notifyChange();
}

bool Piece::isComplete() const {
    if (getSectionCount() == 0) {
        return false;
    }
    
    // Vérifier l'invariant des modulations
    if (!hasValidStructure()) {
        return false;
    }
    
    // Toutes les sections doivent être valides
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Section) {
            auto* section = static_cast<const Section*>(elements[i]);
            if (!section->isValid()) {
                return false;
            }
        }
    }
    
    return true;
}

bool Piece::hasValidStructure() const {
    // Vérifier l'invariant : modulations.size() == sections.size() - 1
    size_t sectionCount = getSectionCount();
    size_t modulationCount = getModulationCount();
    
    if (sectionCount == 0) {
        return modulationCount == 0;
    }
    
    return modulationCount == sectionCount - 1;
}

// Nouvelles méthodes pour la structure unifiée
size_t Piece::getSectionCount() const {
    size_t count = 0;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Section) {
            count++;
        }
    }
    return count;
}

size_t Piece::getModulationCount() const {
    size_t count = 0;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Modulation) {
            count++;
        }
    }
    return count;
}

std::vector<Section*> Piece::getSections() const {
    std::vector<Section*> sections;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Section) {
            sections.push_back(static_cast<Section*>(elements[i]));
        }
    }
    return sections;
}

std::vector<Modulation*> Piece::getModulations() const {
    std::vector<Modulation*> modulations;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Modulation) {
            modulations.push_back(static_cast<Modulation*>(elements[i]));
        }
    }
    return modulations;
}

int Piece::getTotalChordCount() const {
    int total = 0;
    for (int i = 0; i < elements.size(); ++i) {
        if (elements[i]->getType() == PieceElement::Type::Section) {
            auto* section = static_cast<const Section*>(elements[i]);
            total += static_cast<int>(section->getProgression().size());
        }
    }
    return total;
}

juce::String Piece::toString() const {
    juce::String result = title + " - ";
    
    if (isEmpty()) {
        result += "Empty piece";
    } else {
        result += juce::String(getSectionCount()) + " sections";
        if (getModulationCount() > 0) {
            result += ", " + juce::String(getModulationCount()) + " modulations";
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
        result += "  Elements (" + juce::String(elements.size()) + "):\n";
        int sectionCounter = 1;
        int modulationCounter = 1;
        
        for (int i = 0; i < elements.size(); ++i) {
            auto* element = elements[i];
            if (element->getType() == PieceElement::Type::Section) {
                auto* section = static_cast<const Section*>(element);
                result += "    S" + juce::String(sectionCounter) + ". " + section->toString() + "\n";
                sectionCounter++;
            } else if (element->getType() == PieceElement::Type::Modulation) {
                auto* modulation = static_cast<const Modulation*>(element);
                result += "    M" + juce::String(modulationCounter) + ". → " + modulation->toString() + "\n";
                modulationCounter++;
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
    // Connecter tous les callbacks unifiés de tous les éléments
    for (int i = 0; i < elements.size(); ++i) {
        auto* element = elements[i];
        element->onElementChanged = [this]() { notifyChange(); };
    }
}

int Piece::getStartingChordIndexOfSection(size_t sectionIndex) const {
    if (sectionIndex > getSectionCount()) {
        throw std::out_of_range("Section index is out of range for chord index calculation");
    }
    
    int total = 0;
    size_t currentSectionIndex = 0;
    
    for (int i = 0; i < elements.size() && currentSectionIndex < sectionIndex; ++i) {
        if (elements[i]->getType() == PieceElement::Type::Section) {
            auto* section = static_cast<const Section*>(elements[i]);
            total += static_cast<int>(section->getProgression().size());
            currentSectionIndex++;
        }
    }
    return total;
}

std::vector<ChordReference> Piece::getChordsWithGlobalIndices(size_t sectionIndex) {
    // Déléguer à la version const et convertir le résultat
    auto constResult = static_cast<const Piece*>(this)->getChordsWithGlobalIndices(sectionIndex);
    
    std::vector<ChordReference> result;
    result.reserve(constResult.size());
    
    for (const auto& constRef : constResult) {
        result.push_back({const_cast<Chord&>(constRef.chord), constRef.globalIndex});
    }
    
    return result;
}

std::vector<ConstChordReference> Piece::getChordsWithGlobalIndices(size_t sectionIndex) const {
    if (sectionIndex >= getSectionCount()) {
        throw std::out_of_range("Section index out of range");
    }

    std::vector<ConstChordReference> result;
    const int globalStartIndex = getStartingChordIndexOfSection(sectionIndex);
    const auto& section = getSection(sectionIndex);
    const auto& progression = section.getProgression();

    result.reserve(progression.size());
    for (size_t i = 0; i < progression.size(); ++i) {
        result.push_back({progression.getChord(i), globalStartIndex + static_cast<int>(i)});
    }
    return result;
}

// Implémentation des helpers auxiliaires
Section* Piece::getSectionAtElementIndex(int elementIndex) const {
    if (elementIndex < 0 || elementIndex >= elements.size()) {
        return nullptr;
    }
    
    auto* element = elements[elementIndex];
    if (element->getType() == PieceElement::Type::Section) {
        return static_cast<Section*>(element);
    }
    return nullptr;
}

Modulation* Piece::getModulationAtElementIndex(int elementIndex) const {
    if (elementIndex < 0 || elementIndex >= elements.size()) {
        return nullptr;
    }
    
    auto* element = elements[elementIndex];
    if (element->getType() == PieceElement::Type::Modulation) {
        return static_cast<Modulation*>(element);
    }
    return nullptr;
}