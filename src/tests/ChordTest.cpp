#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/Section.h"
#include "model/Progression.h"
#include "model/Chord.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/**
 * Tests unitaires pour la classe Chord (wrapper)
 * 
 * Ces tests vérifient que Chord est un pur wrapper qui :
 * - Lit correctement les propriétés du ValueTree
 * - Écrit correctement les propriétés dans le ValueTree
 * - Ne génère pas d'IDs (c'est Progression qui le fait)
 */
class ChordTest : public juce::UnitTest
{
public:
    ChordTest() : juce::UnitTest("Chord Wrapper Tests", "chord_tests") {}
    
    void runTest() override
    {
        // ═══════════════════════════════════════════════════════════════════
        // TEST 1: Chord créé par Progression a les bonnes propriétés
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Propriétés d'un Chord créé par Progression"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            progression.addChord(Diatony::ChordDegree::Fifth, 
                                Diatony::ChordQuality::Major, 
                                Diatony::ChordState::FirstInversion);
            
            auto chord = progression.getChord(0);
            
            expect(chord.isValid(), "Chord doit être valide");
            expectEquals(chord.getId(), 0, "ID = 0");
            expectEquals(static_cast<int>(chord.getDegree()), 
                        static_cast<int>(Diatony::ChordDegree::Fifth), "Degree = Fifth");
            expectEquals(static_cast<int>(chord.getQuality()), 
                        static_cast<int>(Diatony::ChordQuality::Major), "Quality = Major");
            expectEquals(static_cast<int>(chord.getChordState()), 
                        static_cast<int>(Diatony::ChordState::FirstInversion), "State = FirstInversion");
            
            logMessage(juce::String::fromUTF8("✓ Propriétés correctes"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 2: Les setters modifient le ValueTree
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Setters modifient le ValueTree"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            progression.addChord(Diatony::ChordDegree::First);
            
            auto chord = progression.getChord(0);
            
            // Modifier les propriétés
            chord.setDegree(Diatony::ChordDegree::Fourth);
            chord.setQuality(Diatony::ChordQuality::Minor);
            chord.setChordState(Diatony::ChordState::SecondInversion);
            
            // Vérifier via un nouveau wrapper sur le même ValueTree
            auto sameChord = progression.getChord(0);
            
            expectEquals(static_cast<int>(sameChord.getDegree()), 
                        static_cast<int>(Diatony::ChordDegree::Fourth), "Degree modifié");
            expectEquals(static_cast<int>(sameChord.getQuality()), 
                        static_cast<int>(Diatony::ChordQuality::Minor), "Quality modifiée");
            expectEquals(static_cast<int>(sameChord.getChordState()), 
                        static_cast<int>(Diatony::ChordState::SecondInversion), "State modifié");
            
            logMessage(juce::String::fromUTF8("✓ Setters fonctionnent correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 3: L'ID est préservé après modifications
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("ID préservé après modifications"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            
            auto chord = progression.getChord(1);
            int originalId = chord.getId();
            
            // Modifier toutes les propriétés
            chord.setDegree(Diatony::ChordDegree::Seventh);
            chord.setQuality(Diatony::ChordQuality::Diminished);
            chord.setChordState(Diatony::ChordState::ThirdInversion);
            
            // L'ID ne doit pas changer
            expectEquals(chord.getId(), originalId, "ID inchangé après modifications");
            
            logMessage(juce::String::fromUTF8("✓ ID préservé"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 4: Chord invalide
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Chord invalide (wrapper vide)"));
        {
            juce::ValueTree emptyTree;
            Chord invalidChord{emptyTree};
            
            expect(!invalidChord.isValid(), "Chord doit être invalide");
            expectEquals(invalidChord.getId(), -1, "ID = -1 pour invalide");
            
            // Les getters ne doivent pas crasher
            invalidChord.getDegree();
            invalidChord.getQuality();
            invalidChord.getChordState();
            invalidChord.toString();
            
            logMessage(juce::String::fromUTF8("✓ Chord invalide géré sans crash"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 5: Valeurs par défaut
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Valeurs par défaut"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            
            // Ajouter un accord avec uniquement le degré
            progression.addChord(Diatony::ChordDegree::Second);
            
            auto chord = progression.getChord(0);
            
            // Vérifier les valeurs par défaut
            expectEquals(static_cast<int>(chord.getDegree()), 
                        static_cast<int>(Diatony::ChordDegree::Second), "Degree = Second");
            expectEquals(static_cast<int>(chord.getQuality()), 
                        static_cast<int>(Diatony::ChordQuality::Auto), "Quality par défaut = Auto");
            expectEquals(static_cast<int>(chord.getChordState()), 
                        static_cast<int>(Diatony::ChordState::Fundamental), "State par défaut = Fundamental");
            
            logMessage(juce::String::fromUTF8("✓ Valeurs par défaut correctes"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 6: toString()
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("toString() retourne une description"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            progression.addChord(Diatony::ChordDegree::Fifth);
            
            auto chord = progression.getChord(0);
            auto str = chord.toString();
            
            expect(str.contains("ID=0"), "toString contient l'ID");
            expect(str.contains("Degree="), "toString contient le degré");
            
            logMessage(juce::String::fromUTF8("✓ toString(): ") + str);
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 7: Tous les degrés
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Tous les degrés peuvent être utilisés"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            
            // Ajouter tous les degrés
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Second);
            progression.addChord(Diatony::ChordDegree::Third);
            progression.addChord(Diatony::ChordDegree::Fourth);
            progression.addChord(Diatony::ChordDegree::Fifth);
            progression.addChord(Diatony::ChordDegree::Sixth);
            progression.addChord(Diatony::ChordDegree::Seventh);
            
            expectEquals(static_cast<int>(progression.size()), 7, "7 degrés");
            
            // Vérifier chaque degré
            for (int i = 0; i < 7; ++i)
            {
                auto chord = progression.getChord(static_cast<size_t>(i));
                expectEquals(static_cast<int>(chord.getDegree()), i, 
                            "Degré " + juce::String(i) + " correct");
            }
            
            logMessage(juce::String::fromUTF8("✓ Tous les degrés First à Seventh fonctionnent"));
        }
    }
};

// Enregistrement statique du test
static ChordTest chordTest;

