#include <JuceHeader.h>
#include "model/Piece.h"
#include "model/Section.h"
#include "model/Progression.h"
#include "model/Chord.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/** @brief Tests unitaires pour la classe Progression (gestionnaire des Chords). */
class ProgressionTest : public juce::UnitTest
{
public:
    ProgressionTest() : juce::UnitTest("Progression Tests", "progression_tests") {}
    
    void runTest() override
    {
        beginTest(juce::String::fromUTF8("Progression vide par défaut"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto section = piece.getSection(0);
            auto progression = section.getProgression();
            
            expect(progression.isValid(), "Progression doit être valide");
            expect(progression.isEmpty(), "Progression vide par défaut");
            expectEquals(static_cast<int>(progression.size()), 0, "0 accords");
            
            logMessage(juce::String::fromUTF8("✓ Progression vide créée"));
        }
        
        beginTest(juce::String::fromUTF8("Ajout d'accords - IDs générés automatiquement"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto section = piece.getSection(0);
            auto progression = section.getProgression();
            
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            progression.addChord(Diatony::ChordDegree::Fifth);
            
            expectEquals(static_cast<int>(progression.size()), 3, "3 accords");
            expectEquals(progression.getChord(0).getId(), 0, "Premier accord ID=0");
            expectEquals(progression.getChord(1).getId(), 1, "Deuxième accord ID=1");
            expectEquals(progression.getChord(2).getId(), 2, "Troisième accord ID=2");
            
            logMessage(juce::String::fromUTF8("✓ IDs générés correctement: 0, 1, 2"));
        }
        
        beginTest(juce::String::fromUTF8("Suppression d'accord"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            progression.addChord(Diatony::ChordDegree::Fifth);
            
            progression.removeChord(1);
            
            expectEquals(static_cast<int>(progression.size()), 2, "2 accords restants");
            expectEquals(progression.getChord(0).getId(), 0, "Premier accord toujours ID=0");
            expectEquals(progression.getChord(1).getId(), 2, "Deuxième accord maintenant ID=2");
            
            logMessage(juce::String::fromUTF8("✓ Suppression correcte, IDs préservés"));
        }
        
        beginTest(juce::String::fromUTF8("IDs dynamiques - cycle créer/supprimer/créer"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            
            progression.clear();
            expectEquals(static_cast<int>(progression.size()), 0, "Vide après clear");
            
            progression.addChord(Diatony::ChordDegree::Fifth);
            progression.addChord(Diatony::ChordDegree::First);
            
            expectEquals(progression.getChord(0).getId(), 0, "Premier ID = 0 après clear");
            expectEquals(progression.getChord(1).getId(), 1, "Deuxième ID = 1 après clear");
            
            logMessage(juce::String::fromUTF8("✓ IDs réinitialisés après clear()"));
        }
        
        beginTest(juce::String::fromUTF8("Accès par ID"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            progression.addChord(Diatony::ChordDegree::Fifth);
            
            progression.removeChord(1);
            
            auto chord0 = progression.getChordById(0);
            auto chord2 = progression.getChordById(2);
            auto chordInvalid = progression.getChordById(1);
            
            expect(chord0.isValid(), "Chord ID=0 valide");
            expect(chord2.isValid(), "Chord ID=2 valide");
            expect(!chordInvalid.isValid(), "Chord ID=1 invalide (supprimé)");
            
            expectEquals(progression.getChordIndexById(0), 0, "ID=0 → Index=0");
            expectEquals(progression.getChordIndexById(2), 1, "ID=2 → Index=1");
            expectEquals(progression.getChordIndexById(1), -1, "ID=1 → -1 (supprimé)");
            
            logMessage(juce::String::fromUTF8("✓ Accès par ID fonctionne"));
        }
        
        beginTest(juce::String::fromUTF8("Insertion d'accord à une position"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fifth);
            
            progression.insertChord(1, Diatony::ChordDegree::Fourth);
            
            expectEquals(static_cast<int>(progression.size()), 3, "3 accords");
            expectEquals(static_cast<int>(progression.getChord(0).getDegree()), 
                        static_cast<int>(Diatony::ChordDegree::First), "Index 0 = First");
            expectEquals(static_cast<int>(progression.getChord(1).getDegree()), 
                        static_cast<int>(Diatony::ChordDegree::Fourth), "Index 1 = Fourth (inséré)");
            expectEquals(static_cast<int>(progression.getChord(2).getDegree()), 
                        static_cast<int>(Diatony::ChordDegree::Fifth), "Index 2 = Fifth");
            
            logMessage(juce::String::fromUTF8("✓ Insertion à une position fonctionne"));
        }
        
        beginTest(juce::String::fromUTF8("Progression invalide (wrapper vide)"));
        {
            juce::ValueTree emptyTree;
            Progression invalidProgression{emptyTree};
            
            expect(!invalidProgression.isValid(), "Progression doit être invalide");
            expect(invalidProgression.isEmpty(), "isEmpty() ne crashe pas");
            expectEquals(static_cast<int>(invalidProgression.size()), 0, "size() retourne 0");
            
            invalidProgression.toString();
            
            logMessage(juce::String::fromUTF8("✓ Progression invalide gérée sans crash"));
        }
        
        beginTest(juce::String::fromUTF8("toString() retourne une description"));
        {
            Piece piece;
            piece.addSection("Test");
            
            auto progression = piece.getSection(0).getProgression();
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            
            auto str = progression.toString();
            expect(str.contains("2 chords"), "toString contient le nombre d'accords");
            
            logMessage(juce::String::fromUTF8("✓ toString(): ") + str);
        }
    }
};

static ProgressionTest progressionTest;
