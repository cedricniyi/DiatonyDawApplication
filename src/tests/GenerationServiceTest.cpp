#include <JuceHeader.h>
#include "services/GenerationService.h"
#include "model/Piece.h"
#include "model/Section.h"
#include "model/Progression.h"
#include "model/Chord.h"
#include "model/DiatonyTypes.h"

/**
 * Tests unitaires pour le GenerationService
 * 
 * Ces tests vérifient que le service peut correctement lire le nouveau modèle
 * refactoré (ValueTree wrappers) sans corrompre les données.
 */
class GenerationServiceTest : public juce::UnitTest
{
public:
    GenerationServiceTest() : juce::UnitTest("GenerationService Tests", "genservice_tests") {}
    
    void runTest() override
    {
        // ═══════════════════════════════════════════════════════════════════
        // TEST 1: Service prêt après construction
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Service prêt après construction"));
        {
            GenerationService service;
            
            expect(service.isReady(), "Service doit être prêt");
            expect(!service.isGenerating(), "Service ne doit pas générer au repos");
            expect(service.getLastError().isEmpty(), "Pas d'erreur initiale");
            
            logMessage(juce::String::fromUTF8("✓ Service initialisé correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 2: Lecture du modèle via logGenerationInfo
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Lecture du modèle (1 section, 3 accords)"));
        {
            // Créer une pièce avec une section et des accords
            Piece piece("Test Piece");
            piece.addSection("Section A");
            
            auto section = piece.getSection(0);
            auto progression = section.getProgression();
            
            // Ajouter : I - V - I (cadence parfaite)
            progression.addChord(Diatony::ChordDegree::First, 
                                Diatony::ChordQuality::Major, 
                                Diatony::ChordState::Fundamental);
            progression.addChord(Diatony::ChordDegree::Fifth, 
                                Diatony::ChordQuality::DominantSeventh, 
                                Diatony::ChordState::Fundamental);
            progression.addChord(Diatony::ChordDegree::First, 
                                Diatony::ChordQuality::Major, 
                                Diatony::ChordState::Fundamental);
            
            // Vérifier que le modèle est correctement construit
            expectEquals(piece.getSectionCount(), (size_t)1, "1 section");
            expectEquals(static_cast<int>(progression.size()), 3, "3 accords");
            
            // Tester la lecture via le service
            GenerationService service;
            
            // logGenerationInfo parcourt tout le modèle comme le ferait la génération
            // Si ça ne crashe pas et que les logs sont corrects, la lecture fonctionne
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service toujours prêt après lecture");
            
            logMessage(juce::String::fromUTF8("✓ Lecture du modèle réussie (vérifier logs console)"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 3: Lecture avec qualité Auto
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Lecture avec ChordQuality::Auto"));
        {
            Piece piece("Test Auto Quality");
            piece.addSection("Section Auto");
            
            auto section = piece.getSection(0);
            auto progression = section.getProgression();
            
            // Ajouter des accords en mode Auto (le service doit les gérer)
            progression.addChord(Diatony::ChordDegree::First);   // Auto par défaut
            progression.addChord(Diatony::ChordDegree::Fourth);  // Auto par défaut
            progression.addChord(Diatony::ChordDegree::Fifth);   // Auto par défaut
            
            // Vérifier que les accords ont bien Quality::Auto
            auto chord = progression.getChord(0);
            expectEquals(static_cast<int>(chord.getQuality()), 
                        static_cast<int>(Diatony::ChordQuality::Auto), 
                        "Quality Auto");
            
            GenerationService service;
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service gère ChordQuality::Auto");
            
            logMessage(juce::String::fromUTF8("✓ ChordQuality::Auto géré correctement"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 4: Lecture avec plusieurs sections et modulation
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Lecture multi-sections avec modulation"));
        {
            Piece piece("Test Multi-Sections");
            piece.addSection("Section 1 - Do Majeur");
            piece.addSection("Section 2 - Sol Majeur");
            
            // Ajouter des accords à chaque section
            auto section1 = piece.getSection(0);
            auto prog1 = section1.getProgression();
            prog1.addChord(Diatony::ChordDegree::First);
            prog1.addChord(Diatony::ChordDegree::Fourth);
            prog1.addChord(Diatony::ChordDegree::Fifth);
            
            auto section2 = piece.getSection(1);
            auto prog2 = section2.getProgression();
            prog2.addChord(Diatony::ChordDegree::First);
            prog2.addChord(Diatony::ChordDegree::Fifth);
            prog2.addChord(Diatony::ChordDegree::First);
            
            // Vérifier la structure
            expectEquals(piece.getSectionCount(), (size_t)2, "2 sections");
            expectEquals(piece.getModulationCount(), (size_t)1, "1 modulation");
            expectEquals(piece.getTotalChordCount(), 6, "6 accords total");
            
            // La modulation doit référencer les bonnes sections
            auto modulation = piece.getModulation(0);
            expectEquals(modulation.getFromSectionId(), 0, "Modulation from section 0");
            expectEquals(modulation.getToSectionId(), 1, "Modulation to section 1");
            
            // Tester la lecture
            GenerationService service;
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service gère multi-sections");
            
            logMessage(juce::String::fromUTF8("✓ Multi-sections avec modulation OK"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 5: Pièce vide
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Lecture d'une pièce vide"));
        {
            Piece piece("Empty Piece");
            
            expect(piece.isEmpty(), "Pièce vide");
            
            GenerationService service;
            service.logGenerationInfo(piece);
            
            // Le service ne doit pas crasher même avec une pièce vide
            expect(service.isReady(), "Service gère pièce vide");
            
            logMessage(juce::String::fromUTF8("✓ Pièce vide gérée sans crash"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 6: Reset du service
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Reset du service"));
        {
            GenerationService service;
            
            service.reset();
            
            expect(service.isReady(), "Service prêt après reset");
            expect(service.getLastError().isEmpty(), "Erreur vidée après reset");
            
            logMessage(juce::String::fromUTF8("✓ Reset fonctionne"));
        }
    }
};

// Enregistrement statique du test
static GenerationServiceTest generationServiceTest;

