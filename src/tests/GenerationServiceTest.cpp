#include <JuceHeader.h>
#include "services/GenerationService.h"
#include "model/Piece.h"
#include "model/Section.h"
#include "model/Progression.h"
#include "model/Chord.h"
#include "model/DiatonyTypes.h"

/** @brief Tests unitaires pour le GenerationService (lecture du modèle ValueTree). */
class GenerationServiceTest : public juce::UnitTest
{
public:
    GenerationServiceTest() : juce::UnitTest("GenerationService Tests", "genservice_tests") {}
    
    void runTest() override
    {
        beginTest(juce::String::fromUTF8("Service prêt après construction"));
        {
            GenerationService service;
            
            expect(service.isReady(), "Service doit être prêt");
            expect(!service.isGenerating(), "Service ne doit pas générer au repos");
            expect(service.getLastError().isEmpty(), "Pas d'erreur initiale");
            
            logMessage(juce::String::fromUTF8("✓ Service initialisé correctement"));
        }
        
        beginTest(juce::String::fromUTF8("Lecture du modèle (1 section, 3 accords)"));
        {
            Piece piece("Test Piece");
            piece.addSection("Section A");
            
            auto section = piece.getSection(0);
            auto progression = section.getProgression();
            
            progression.addChord(Diatony::ChordDegree::First, 
                                Diatony::ChordQuality::Major, 
                                Diatony::ChordState::Fundamental);
            progression.addChord(Diatony::ChordDegree::Fifth, 
                                Diatony::ChordQuality::DominantSeventh, 
                                Diatony::ChordState::Fundamental);
            progression.addChord(Diatony::ChordDegree::First, 
                                Diatony::ChordQuality::Major, 
                                Diatony::ChordState::Fundamental);
            
            expectEquals(piece.getSectionCount(), (size_t)1, "1 section");
            expectEquals(static_cast<int>(progression.size()), 3, "3 accords");
            
            GenerationService service;
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service toujours prêt après lecture");
            
            logMessage(juce::String::fromUTF8("✓ Lecture du modèle réussie (vérifier logs console)"));
        }
        
        beginTest(juce::String::fromUTF8("Lecture avec ChordQuality::Auto"));
        {
            Piece piece("Test Auto Quality");
            piece.addSection("Section Auto");
            
            auto section = piece.getSection(0);
            auto progression = section.getProgression();
            
            progression.addChord(Diatony::ChordDegree::First);
            progression.addChord(Diatony::ChordDegree::Fourth);
            progression.addChord(Diatony::ChordDegree::Fifth);
            
            auto chord = progression.getChord(0);
            expectEquals(static_cast<int>(chord.getQuality()), 
                        static_cast<int>(Diatony::ChordQuality::Auto), 
                        "Quality Auto");
            
            GenerationService service;
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service gère ChordQuality::Auto");
            
            logMessage(juce::String::fromUTF8("✓ ChordQuality::Auto géré correctement"));
        }
        
        beginTest(juce::String::fromUTF8("Lecture multi-sections avec modulation"));
        {
            Piece piece("Test Multi-Sections");
            piece.addSection("Section 1 - Do Majeur");
            piece.addSection("Section 2 - Sol Majeur");
            
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
            
            expectEquals(piece.getSectionCount(), (size_t)2, "2 sections");
            expectEquals(piece.getModulationCount(), (size_t)1, "1 modulation");
            expectEquals(piece.getTotalChordCount(), 6, "6 accords total");
            
            auto modulation = piece.getModulation(0);
            expectEquals(modulation.getFromSectionId(), 0, "Modulation from section 0");
            expectEquals(modulation.getToSectionId(), 1, "Modulation to section 1");
            
            GenerationService service;
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service gère multi-sections");
            
            logMessage(juce::String::fromUTF8("✓ Multi-sections avec modulation OK"));
        }
        
        beginTest(juce::String::fromUTF8("Lecture d'une pièce vide"));
        {
            Piece piece("Empty Piece");
            
            expect(piece.isEmpty(), "Pièce vide");
            
            GenerationService service;
            service.logGenerationInfo(piece);
            
            expect(service.isReady(), "Service gère pièce vide");
            
            logMessage(juce::String::fromUTF8("✓ Pièce vide gérée sans crash"));
        }
        
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

static GenerationServiceTest generationServiceTest;
