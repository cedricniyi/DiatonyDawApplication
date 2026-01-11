#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/Modulation.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/** @brief Tests unitaires pour la classe Modulation (wrapper ValueTree avec références aux sections). */
class ModulationTest : public juce::UnitTest
{
public:
    ModulationTest() : juce::UnitTest("Modulation Wrapper Tests", "modulation_tests") {}
    
    void runTest() override
    {
        beginTest(juce::String::fromUTF8("Modulation créée entre deux sections"));
        {
            Piece piece;
            piece.addSection("Section A");
            piece.addSection("Section B");
            
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation");
            
            auto modulation = piece.getModulation(0);
            
            expect(modulation.isValid(), "Modulation doit être valide");
            expectEquals(modulation.getId(), 0, "ID = 0");
            expectEquals(modulation.getFromSectionId(), 0, "fromSectionId = 0");
            expectEquals(modulation.getToSectionId(), 1, "toSectionId = 1");
            
            logMessage(juce::String::fromUTF8("✓ Modulation créée avec bonnes références"));
        }
        
        beginTest(juce::String::fromUTF8("Setters modifient le ValueTree"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            
            modulation.setModulationType(Diatony::ModulationType::Chromatic);
            modulation.setFromChordIndex(2);
            modulation.setToChordIndex(1);
            modulation.setName("Custom Modulation");
            
            auto sameModulation = piece.getModulation(0);
            
            expectEquals(static_cast<int>(sameModulation.getModulationType()), 
                        static_cast<int>(Diatony::ModulationType::Chromatic), 
                        "Type = Chromatic");
            expectEquals(sameModulation.getFromChordIndex(), 2, "fromChordIndex = 2");
            expectEquals(sameModulation.getToChordIndex(), 1, "toChordIndex = 1");
            expectEquals(sameModulation.getName(), juce::String("Custom Modulation"), "Nom modifié");
            
            logMessage(juce::String::fromUTF8("✓ Setters fonctionnent correctement"));
        }
        
        beginTest(juce::String::fromUTF8("hasValidSectionReferences"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            
            expect(modulation.hasValidSectionReferences(), "Références valides");
            
            juce::ValueTree invalidMod(ModelIdentifiers::MODULATION);
            invalidMod.setProperty(ModelIdentifiers::fromSectionId, -1, nullptr);
            invalidMod.setProperty(ModelIdentifiers::toSectionId, -1, nullptr);
            
            Modulation invalidModulation(invalidMod);
            expect(!invalidModulation.hasValidSectionReferences(), "Références invalides");
            
            logMessage(juce::String::fromUTF8("✓ Validation des références fonctionne"));
        }
        
        beginTest(juce::String::fromUTF8("hasChordIndices"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            
            expect(!modulation.hasChordIndices(), "Pas d'indices d'accords par défaut");
            
            modulation.setFromChordIndex(0);
            modulation.setToChordIndex(1);
            
            expect(modulation.hasChordIndices(), "Indices définis");
            
            logMessage(juce::String::fromUTF8("✓ Validation des indices d'accords fonctionne"));
        }
        
        beginTest(juce::String::fromUTF8("Modulation invalide (wrapper vide)"));
        {
            juce::ValueTree emptyTree;
            Modulation invalidModulation{emptyTree};
            
            expect(!invalidModulation.isValid(), "Modulation doit être invalide");
            expectEquals(invalidModulation.getId(), -1, "ID = -1 pour invalide");
            
            invalidModulation.getName();
            invalidModulation.getModulationType();
            invalidModulation.getFromSectionId();
            invalidModulation.getToSectionId();
            
            logMessage(juce::String::fromUTF8("✓ Modulation invalide gérée sans crash"));
        }
        
        beginTest(juce::String::fromUTF8("Modulation de liaison après suppression"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            expectEquals(static_cast<int>(piece.getModulationCount()), 2, "2 modulations avant");
            
            piece.removeSection(1);
            
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation après");
            
            auto newModulation = piece.getModulation(0);
            expectEquals(newModulation.getFromSectionId(), 0, "Relie A (id=0)");
            expectEquals(newModulation.getToSectionId(), 2, "Vers C (id=2)");
            
            logMessage(juce::String::fromUTF8("✓ Modulation de liaison créée correctement"));
        }
        
        beginTest(juce::String::fromUTF8("toString() retourne une description"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            auto str = modulation.toString();
            
            expect(str.contains("ID=0"), "toString contient l'ID");
            expect(str.contains("Section 0"), "toString contient fromSectionId");
            expect(str.contains("Section 1"), "toString contient toSectionId");
            
            logMessage(juce::String::fromUTF8("✓ toString(): ") + str);
        }
    }
};

static ModulationTest modulationTest;
