#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/Section.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/**
 * Tests unitaires pour la classe Section (wrapper)
 * 
 * Ces tests vérifient que Section est un pur wrapper qui :
 * - Lit correctement les propriétés du ValueTree
 * - Écrit correctement les propriétés dans le ValueTree
 * - Ne génère pas d'IDs (c'est Piece qui le fait)
 */
class SectionTest : public juce::UnitTest
{
public:
    SectionTest() : juce::UnitTest("Section Wrapper Tests", "section_tests") {}
    
    void runTest() override
    {
        // ═══════════════════════════════════════════════════════════════════
        // TEST 1: Section créée par Piece a les bonnes propriétés par défaut
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Propriétés par défaut d'une Section"));
        {
            Piece piece;
            piece.addSection("Test Section");
            
            auto section = piece.getSection(0);
            
            expect(section.isValid(), "Section doit être valide");
            expectEquals(section.getId(), 0, "ID = 0");
            expectEquals(section.getName(), juce::String("Test Section"), "Nom correct");
            expectEquals(static_cast<int>(section.getNote()), static_cast<int>(Diatony::Note::C), "Note par défaut = C");
            expectEquals(static_cast<int>(section.getAlteration()), static_cast<int>(Diatony::Alteration::Natural), "Altération par défaut = Natural");
            expect(section.getIsMajor(), "Mode par défaut = Majeur");
            
            logMessage(juce::String::fromUTF8("✓ Propriétés par défaut correctes"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 2: Les setters modifient le ValueTree
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Setters modifient le ValueTree"));
        {
            Piece piece;
            piece.addSection("Original");
            
            auto section = piece.getSection(0);
            
            // Modifier les propriétés
            section.setName("Modified");
            section.setNote(Diatony::Note::G);
            section.setAlteration(Diatony::Alteration::Sharp);
            section.setIsMajor(false);
            
            // Vérifier via un nouveau wrapper sur le même ValueTree
            auto sameSection = piece.getSection(0);
            
            expectEquals(sameSection.getName(), juce::String("Modified"), "Nom modifié");
            expectEquals(static_cast<int>(sameSection.getNote()), static_cast<int>(Diatony::Note::G), "Note = G");
            expectEquals(static_cast<int>(sameSection.getAlteration()), static_cast<int>(Diatony::Alteration::Sharp), "Altération = Sharp");
            expect(!sameSection.getIsMajor(), "Mode = Mineur");
            
            logMessage(juce::String::fromUTF8("✓ Setters fonctionnent correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 3: Section a une Progression
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Section contient une Progression"));
        {
            Piece piece;
            piece.addSection("With Progression");
            
            auto section = piece.getSection(0);
            
            expect(section.hasProgression(), "Section doit avoir une Progression");
            
            auto progression = section.getProgression();
            expect(progression.isValid(), "Progression doit être valide");
            expect(progression.isEmpty(), "Progression vide par défaut");
            expectEquals(static_cast<int>(progression.size()), 0, "0 accords");
            
            logMessage(juce::String::fromUTF8("✓ Progression accessible"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 4: Section invalide
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Section invalide (wrapper vide)"));
        {
            // Créer un wrapper sur un ValueTree vide
            juce::ValueTree emptyTree;
            Section invalidSection{emptyTree};
            
            expect(!invalidSection.isValid(), "Section doit être invalide");
            expectEquals(invalidSection.getId(), -1, "ID = -1 pour invalide");
            
            // Les getters ne doivent pas crasher
            invalidSection.getName();
            invalidSection.getNote();
            invalidSection.getAlteration();
            invalidSection.getIsMajor();
            
            logMessage(juce::String::fromUTF8("✓ Section invalide gérée sans crash"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 5: L'ID est préservé après modification des propriétés
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("ID préservé après modifications"));
        {
            Piece piece;
            piece.addSection("A");
            piece.addSection("B");
            
            auto sectionB = piece.getSection(1);
            int originalId = sectionB.getId();
            
            // Modifier plein de propriétés
            sectionB.setName("Modified B");
            sectionB.setNote(Diatony::Note::F);
            sectionB.setIsMajor(false);
            
            // L'ID ne doit pas changer
            expectEquals(sectionB.getId(), originalId, "ID inchangé après modifications");
            
            logMessage(juce::String::fromUTF8("✓ ID préservé"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 6: toString() fonctionne
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("toString() retourne une description"));
        {
            Piece piece;
            piece.addSection("My Section");
            
            auto section = piece.getSection(0);
            auto str = section.toString();
            
            expect(str.contains("My Section"), "toString contient le nom");
            expect(str.contains("ID=0"), "toString contient l'ID");
            
            logMessage(juce::String::fromUTF8("✓ toString(): ") + str);
        }
    }
};

// Enregistrement statique du test
static SectionTest sectionTest;

