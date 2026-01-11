#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/ModelIdentifiers.h"

/**
 * @brief Tests unitaires pour la classe Piece.
 * 
 * Vérifie les invariants : modulations.count() == sections.count() - 1, ordre S-M-S-M-S, IDs uniques.
 */
class PieceTest : public juce::UnitTest
{
public:
    PieceTest() : juce::UnitTest("Piece Model Tests", "piece_tests") {}
    
    void runTest() override
    {
        beginTest(juce::String::fromUTF8("Création d'une Piece vide"));
        {
            Piece piece;
            
            expect(piece.isEmpty(), "Une nouvelle Piece devrait être vide");
            expectEquals(static_cast<int>(piece.getSectionCount()), 0, "Pas de sections");
            expectEquals(static_cast<int>(piece.getModulationCount()), 0, "Pas de modulations");
            expect(piece.hasValidStructure(), "Structure vide = valide");
            
            logMessage(juce::String::fromUTF8("✓ Piece vide créée correctement"));
        }
        
        beginTest(juce::String::fromUTF8("Ajout d'une seule section"));
        {
            Piece piece;
            piece.addSection("Section A");
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 1, "1 section");
            expectEquals(static_cast<int>(piece.getModulationCount()), 0, "0 modulation (pas de liaison)");
            expect(piece.hasValidStructure(), "Invariant: 0 == 1-1");
            
            auto section = piece.getSection(0);
            int sectionId = section.getState().getProperty(ModelIdentifiers::id, -1);
            expectEquals(sectionId, 0, "Premier ID = 0");
            
            logMessage(juce::String::fromUTF8("✓ Section unique ajoutée, ID=") + juce::String(sectionId));
        }
        
        beginTest(juce::String::fromUTF8("Ajout de 3 sections - structure S-M-S-M-S"));
        {
            Piece piece;
            piece.addSection("Section 1");
            piece.addSection("Section 2");
            piece.addSection("Section 3");
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 3, "3 sections");
            expectEquals(static_cast<int>(piece.getModulationCount()), 2, "2 modulations");
            expect(piece.hasValidStructure(), "Invariant: 2 == 3-1");
            
            auto& state = piece.getState();
            expectEquals(state.getNumChildren(), 5, "5 enfants au total (S-M-S-M-S)");
            
            expect(state.getChild(0).hasType(ModelIdentifiers::SECTION), "Index 0 = Section");
            expect(state.getChild(1).hasType(ModelIdentifiers::MODULATION), "Index 1 = Modulation");
            expect(state.getChild(2).hasType(ModelIdentifiers::SECTION), "Index 2 = Section");
            expect(state.getChild(3).hasType(ModelIdentifiers::MODULATION), "Index 3 = Modulation");
            expect(state.getChild(4).hasType(ModelIdentifiers::SECTION), "Index 4 = Section");
            
            expectEquals(static_cast<int>(state.getChild(0).getProperty(ModelIdentifiers::id)), 0, "Section 0");
            expectEquals(static_cast<int>(state.getChild(2).getProperty(ModelIdentifiers::id)), 1, "Section 1");
            expectEquals(static_cast<int>(state.getChild(4).getProperty(ModelIdentifiers::id)), 2, "Section 2");
            
            logMessage(juce::String::fromUTF8("✓ Structure S-M-S-M-S correcte"));
            logMessage(piece.getDetailedSummary());
        }
        
        beginTest(juce::String::fromUTF8("Suppression de la dernière section"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            piece.removeSection(2);
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 2, "2 sections restantes");
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation restante");
            expect(piece.hasValidStructure(), "Invariant respecté");
            
            logMessage(juce::String::fromUTF8("✓ Dernière section supprimée correctement"));
        }
        
        beginTest(juce::String::fromUTF8("Suppression de la première section"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            piece.removeSection(0);
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 2, "2 sections restantes");
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 modulation restante");
            expect(piece.hasValidStructure(), "Invariant respecté");
            
            auto firstSection = piece.getSection(0);
            int firstId = firstSection.getState().getProperty(ModelIdentifiers::id, -1);
            expectEquals(firstId, 1, "L'ancienne section B (id=1) est maintenant à l'index 0");
            
            logMessage(juce::String::fromUTF8("✓ Première section supprimée, IDs préservés"));
        }
        
        beginTest(juce::String::fromUTF8("Suppression d'une section du milieu"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            piece.removeSection(1);
            
            expectEquals(static_cast<int>(piece.getSectionCount()), 2, "2 sections restantes");
            expectEquals(static_cast<int>(piece.getModulationCount()), 1, "1 nouvelle modulation de liaison");
            expect(piece.hasValidStructure(), "Invariant respecté");
            
            auto& state = piece.getState();
            expect(state.getChild(0).hasType(ModelIdentifiers::SECTION), "Index 0 = Section");
            expect(state.getChild(1).hasType(ModelIdentifiers::MODULATION), "Index 1 = Modulation");
            expect(state.getChild(2).hasType(ModelIdentifiers::SECTION), "Index 2 = Section");
            
            auto modulation = piece.getModulation(0);
            int fromId = modulation.getFromSectionId();
            int toId = modulation.getToSectionId();
            expectEquals(fromId, 0, "Modulation part de A (id=0)");
            expectEquals(toId, 2, "Modulation va vers C (id=2)");
            
            logMessage(juce::String::fromUTF8("✓ Section du milieu supprimée, modulation de liaison créée"));
            logMessage(piece.getDetailedSummary());
        }
        
        beginTest(juce::String::fromUTF8("Conversion ID → Index"));
        {
            Piece piece;
            piece.addSection("First");
            piece.addSection("Second");
            piece.addSection("Third");
            
            expectEquals(piece.getSectionIndexById(0), 0, "ID 0 → Index 0");
            expectEquals(piece.getSectionIndexById(1), 1, "ID 1 → Index 1");
            expectEquals(piece.getSectionIndexById(2), 2, "ID 2 → Index 2");
            
            piece.removeSection(1);
            
            expectEquals(piece.getSectionIndexById(0), 0, "ID 0 → Index 0");
            expectEquals(piece.getSectionIndexById(1), -1, "ID 1 → -1 (supprimé)");
            expectEquals(piece.getSectionIndexById(2), 1, "ID 2 → Index 1");
            
            logMessage(juce::String::fromUTF8("✓ Conversion ID→Index correcte après suppression"));
        }
        
        beginTest(juce::String::fromUTF8("IDs dynamiques - cycle créer/supprimer/créer"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            piece.addSection("C");
            
            piece.clear();
            expectEquals(static_cast<int>(piece.getSectionCount()), 0, "Vide après clear");
            
            piece.addSection("New1");
            piece.addSection("New2");
            
            int id1 = piece.getSection(0).getState().getProperty(ModelIdentifiers::id, -1);
            int id2 = piece.getSection(1).getState().getProperty(ModelIdentifiers::id, -1);
            
            expectEquals(id1, 0, "Après clear, premier ID = 0");
            expectEquals(id2, 1, "Après clear, deuxième ID = 1");
            
            logMessage(juce::String::fromUTF8("✓ IDs réinitialisés après clear()"));
        }
        
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

static PieceTest pieceTest;
