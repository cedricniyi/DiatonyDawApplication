#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/ModelIdentifiers.h"

/**
 * Tests unitaires pour la classe Piece
 * 
 * Ces tests vérifient les invariants fondamentaux :
 * 1. modulations.count() == sections.count() - 1
 * 2. L'ordre ValueTree est S-M-S-M-S
 * 3. Les IDs sont uniques et correctement générés
 */
class PieceTest : public juce::UnitTest
{
public:
    PieceTest() : juce::UnitTest("Piece Model Tests", "piece_tests") {}
    
    void runTest() override
    {
        // ═══════════════════════════════════════════════════════════════════
        // TEST 1: Création d'une Piece vide
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Création d'une Piece vide"));
        {
            Piece piece;
            
            expect(piece.isEmpty(), "Une nouvelle Piece devrait être vide");
            expectEquals(static_cast<int>(piece.getSectionCount()), 0, "Pas de sections");
            expectEquals(static_cast<int>(piece.getModulationCount()), 0, "Pas de modulations");
            expect(piece.hasValidStructure(), "Structure vide = valide");
            
            logMessage(juce::String::fromUTF8("✓ Piece vide créée correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 2: Ajout d'une seule section
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Ajout d'une seule section"));
        {
            Piece piece;
            piece.addSection("Section A");
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 1, "1 section");
            expectEquals(static_cast<int>(piece.getModulationCount()), 0, "0 modulation (pas de liaison)");
            expect(piece.hasValidStructure(), "Invariant: 0 == 1-1");
            
            // Vérifier l'ID
            auto section = piece.getSection(0);
            int sectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
            expectEquals(sectionId, 0, "Premier ID = 0");
            
            logMessage(juce::String::fromUTF8("✓ Section unique ajoutée, ID=") + juce::String(sectionId));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 3: Ajout de plusieurs sections (vérifie la chaîne S-M-S)
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Ajout de 3 sections - structure S-M-S-M-S"));
        {
            Piece piece;
            piece.addSection("Section 1");
            piece.addSection("Section 2");
            piece.addSection("Section 3");
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 3, "3 sections");
            expectEquals(static_cast<int>(piece.getModulationCount()), 2, "2 modulations");
            expect(piece.hasValidStructure(), "Invariant: 2 == 3-1");
            
            // Vérifier l'ordre dans le ValueTree
            auto& state = piece.getState();
            expectEquals(state.getNumChildren(), 5, "5 enfants au total (S-M-S-M-S)");
            
            expect(state.getChild(0).hasType(ModelIdentifiers::SECTION), "Index 0 = Section");
            expect(state.getChild(1).hasType(ModelIdentifiers::MODULATION), "Index 1 = Modulation");
            expect(state.getChild(2).hasType(ModelIdentifiers::SECTION), "Index 2 = Section");
            expect(state.getChild(3).hasType(ModelIdentifiers::MODULATION), "Index 3 = Modulation");
            expect(state.getChild(4).hasType(ModelIdentifiers::SECTION), "Index 4 = Section");
            
            // Vérifier les IDs
            expectEquals(static_cast<int>(state.getChild(0).getProperty(ModelIdentifiers::id)), 0, "Section 0");
            expectEquals(static_cast<int>(state.getChild(2).getProperty(ModelIdentifiers::id)), 1, "Section 1");
            expectEquals(static_cast<int>(state.getChild(4).getProperty(ModelIdentifiers::id)), 2, "Section 2");
            
            logMessage(juce::String::fromUTF8( "✓ Structure S-M-S-M-S correcte"));
            logMessage(piece.getDetailedSummary());
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 4: Suppression de la dernière section
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Suppression de la dernière section"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            // Avant: A - M - B - M - C (3 sections, 2 modulations)
            piece.removeSection(2); // Supprime C
            
            // Après: A - M - B (2 sections, 1 modulation)
            expectEquals(static_cast<int>(piece.getSectionCount()), 2, "2 sections restantes");
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation restante");
            expect(piece.hasValidStructure(), "Invariant respecté");
            
            logMessage(juce::String::fromUTF8("✓ Dernière section supprimée correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 5: Suppression de la première section
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Suppression de la première section"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            // Avant: A(id=0) - M - B(id=1) - M - C(id=2)
            piece.removeSection(0); // Supprime A
            
            // Après: B(id=1) - M - C(id=2) (2 sections, 1 modulation)
            expectEquals(static_cast<int>(piece.getSectionCount()), 2, "2 sections restantes");
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation restante");
            expect(piece.hasValidStructure(), "Invariant respecté");
            
            // Vérifier que les IDs sont préservés
            auto firstSection = piece.getSection(0);
            int firstId = firstSection.getState().getProperty(ModelIdentifiers::id, -1);
            expectEquals(firstId, 1, "L'ancienne section B (id=1) est maintenant à l'index 0");
            
            logMessage(juce::String::fromUTF8("✓ Première section supprimée, IDs préservés"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 6: Suppression d'une section du milieu
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Suppression d'une section du milieu"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            // Avant: A(id=0) - M0 - B(id=1) - M1 - C(id=2)
            // Ordre VT: [S0, M0, S1, M1, S2]
            
            piece.removeSection(1); // Supprime B
            
            // Après: A(id=0) - M_new - C(id=2)
            expectEquals(static_cast<int>(piece.getSectionCount()), 2, "2 sections restantes");
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 nouvelle modulation de liaison");
            expect(piece.hasValidStructure(), "Invariant respecté");
            
            // Vérifier l'ordre: S - M - S
            auto& state = piece.getState();
            expect(state.getChild(0).hasType(ModelIdentifiers::SECTION), "Index 0 = Section");
            expect(state.getChild(1).hasType(ModelIdentifiers::MODULATION), "Index 1 = Modulation");
            expect(state.getChild(2).hasType(ModelIdentifiers::SECTION), "Index 2 = Section");
            
            // Vérifier que la modulation relie bien A et C
            auto modulation = piece.getModulation(0);
            int fromId = modulation.getFromSectionId();
            int toId = modulation.getToSectionId();
            expectEquals(fromId, 0, "Modulation part de A (id=0)");
            expectEquals(toId, 2, "Modulation va vers C (id=2)");
            
            logMessage(juce::String::fromUTF8("✓ Section du milieu supprimée, modulation de liaison créée"));
            logMessage(piece.getDetailedSummary());
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 7: getSectionIndexById
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Conversion ID → Index"));
        {
            Piece piece;
            piece.addSection("First");
            piece.addSection("Second");
            piece.addSection("Third");
            
            // Les IDs sont 0, 1, 2
            // Les Index sont aussi 0, 1, 2 (correspondance directe au début)
            expectEquals(piece.getSectionIndexById(0), 0, "ID 0 → Index 0");
            expectEquals(piece.getSectionIndexById(1), 1, "ID 1 → Index 1");
            expectEquals(piece.getSectionIndexById(2), 2, "ID 2 → Index 2");
            
            // Supprimer la section du milieu
            piece.removeSection(1); // Supprime ID=1
            
            // Maintenant: Index 0 = ID 0, Index 1 = ID 2
            expectEquals(piece.getSectionIndexById(0), 0, "ID 0 → Index 0");
            expectEquals(piece.getSectionIndexById(1), -1, "ID 1 → -1 (supprimé)");
            expectEquals(piece.getSectionIndexById(2), 1, "ID 2 → Index 1");
            
            logMessage(juce::String::fromUTF8("✓ Conversion ID→Index correcte après suppression"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 8: IDs dynamiques après cycle créer/supprimer/créer
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("IDs dynamiques - cycle créer/supprimer/créer"));
        {
            Piece piece;
            piece.addSection("A"); // ID = 0
            piece.addSection("B"); // ID = 1
            piece.addSection("C"); // ID = 2
            
            // Supprimer tout
            piece.clear();
            expectEquals(static_cast<int>(piece.getSectionCount()), 0, "Vide après clear");
            
            // Recréer
            piece.addSection("New1"); // ID devrait être 0 (max=-1, donc 0)
            piece.addSection("New2"); // ID devrait être 1
            
            int id1 = piece.getSection(0).getState().getProperty(ModelIdentifiers::id, -1);
            int id2 = piece.getSection(1).getState().getProperty(ModelIdentifiers::id, -1);
            
            expectEquals(id1, 0, "Après clear, premier ID = 0");
            expectEquals(id2, 1, "Après clear, deuxième ID = 1");
            
            logMessage(juce::String::fromUTF8("✓ IDs réinitialisés après clear()"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 9: Suppression de la section unique
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Suppression de la section unique"));
        {
            Piece piece;
            piece.addSection("Only");
            
            piece.removeSection(0);
            
            expect(piece.isEmpty(), "Piece vide après suppression");
            expect(piece.hasValidStructure(), "Structure vide = valide");
            
            logMessage(juce::String::fromUTF8("✓ Section unique supprimée"));
        }
    }
};

// Enregistrement statique du test
static PieceTest pieceTest;

