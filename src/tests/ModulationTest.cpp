#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/Modulation.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/**
 * Tests unitaires pour la classe Modulation (wrapper)
 * 
 * Ces tests vérifient que Modulation est un pur wrapper qui :
 * - Lit correctement les propriétés du ValueTree
 * - Écrit correctement les propriétés dans le ValueTree
 * - Maintient les références vers les sections (fromSectionId, toSectionId)
 */
class ModulationTest : public juce::UnitTest
{
public:
    ModulationTest() : juce::UnitTest("Modulation Wrapper Tests", "modulation_tests") {}
    
    void runTest() override
    {
        // ═══════════════════════════════════════════════════════════════════
        // TEST 1: Modulation créée automatiquement par Piece
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Modulation créée entre deux sections"));
        {
            Piece piece;
            piece.addSection("Section A");
            piece.addSection("Section B");
            
            // Une modulation doit avoir été créée automatiquement
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation");
            
            auto modulation = piece.getModulation(0);
            
            expect(modulation.isValid(), "Modulation doit être valide");
            expectEquals(modulation.getId(), 0, "ID = 0");
            expectEquals(modulation.getFromSectionId(), 0, "fromSectionId = 0");
            expectEquals(modulation.getToSectionId(), 1, "toSectionId = 1");
            
            logMessage(juce::String::fromUTF8("✓ Modulation créée avec bonnes références"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 2: Les setters modifient le ValueTree
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Setters modifient le ValueTree"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            
            // Modifier les propriétés
            modulation.setModulationType(Diatony::ModulationType::Chromatic);
            modulation.setFromChordIndex(2);
            modulation.setToChordIndex(1);
            modulation.setName("Custom Modulation");
            
            // Vérifier via un nouveau wrapper
            auto sameModulation = piece.getModulation(0);
            
            expectEquals(static_cast<int>(sameModulation.getModulationType()), 
                        static_cast<int>(Diatony::ModulationType::Chromatic), 
                        "Type = Chromatic");
            expectEquals(sameModulation.getFromChordIndex(), 2, "fromChordIndex = 2");
            expectEquals(sameModulation.getToChordIndex(), 1, "toChordIndex = 1");
            expectEquals(sameModulation.getName(), juce::String("Custom Modulation"), "Nom modifié");
            
            logMessage(juce::String::fromUTF8("✓ Setters fonctionnent correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 3: hasValidSectionReferences
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("hasValidSectionReferences"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            
            expect(modulation.hasValidSectionReferences(), "Références valides");
            
            // Créer une modulation avec références invalides
            juce::ValueTree invalidMod(ModelIdentifiers::MODULATION);
            invalidMod.setProperty(ModelIdentifiers::fromSectionId, -1, nullptr);
            invalidMod.setProperty(ModelIdentifiers::toSectionId, -1, nullptr);
            
            Modulation invalidModulation(invalidMod);
            expect(!invalidModulation.hasValidSectionReferences(), "Références invalides");
            
            logMessage(juce::String::fromUTF8("✓ Validation des références fonctionne"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 4: hasChordIndices
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("hasChordIndices"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto modulation = piece.getModulation(0);
            
            // Par défaut, les indices d'accords sont -1
            expect(!modulation.hasChordIndices(), "Pas d'indices d'accords par défaut");
            
            // Définir les indices
            modulation.setFromChordIndex(0);
            modulation.setToChordIndex(1);
            
            expect(modulation.hasChordIndices(), "Indices définis");
            
            logMessage(juce::String::fromUTF8("✓ Validation des indices d'accords fonctionne"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 5: Modulation invalide
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Modulation invalide (wrapper vide)"));
        {
            juce::ValueTree emptyTree;
            Modulation invalidModulation{emptyTree};
            
            expect(!invalidModulation.isValid(), "Modulation doit être invalide");
            expectEquals(invalidModulation.getId(), -1, "ID = -1 pour invalide");
            
            // Les getters ne doivent pas crasher
            invalidModulation.getName();
            invalidModulation.getModulationType();
            invalidModulation.getFromSectionId();
            invalidModulation.getToSectionId();
            
            logMessage(juce::String::fromUTF8("✓ Modulation invalide gérée sans crash"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 6: Modulation de liaison après suppression du milieu
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Modulation de liaison après suppression"));
        {
            Piece piece;
            piece.addSection("A");  // ID=0
            piece.addSection("B");  // ID=1
            piece.addSection("C");  // ID=2
            
            // Avant: A -M0-> B -M1-> C
            expectEquals(static_cast<int>(piece.getModulationCount()), 2, "2 modulations avant");
            
            // Supprimer B
            piece.removeSection(1);
            
            // Après: A -M_new-> C
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation après");
            
            auto newModulation = piece.getModulation(0);
            expectEquals(newModulation.getFromSectionId(), 0, "Relie A (id=0)");
            expectEquals(newModulation.getToSectionId(), 2, "Vers C (id=2)");
            
            logMessage(juce::String::fromUTF8("✓ Modulation de liaison créée correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 7: toString() fonctionne
        // ═══════════════════════════════════════════════════════════════════
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

// Enregistrement statique du test
static ModulationTest modulationTest;

