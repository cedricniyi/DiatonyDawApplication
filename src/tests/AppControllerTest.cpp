#include <JuceHeader.h>
#include "controller/AppController.h"
#include "controller/ContextIdentifiers.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/**
 * Tests unitaires pour l'AppController
 * 
 * Ces tests vérifient :
 * - L'orchestration (auto-sélection après ajout)
 * - La sécurité (nettoyage de sélection après suppression)
 * - La traduction Index → ID (sélection stable)
 * - La robustesse (index invalides ne crashent pas)
 */
class AppControllerTest : public juce::UnitTest
{
public:
    AppControllerTest() : juce::UnitTest("AppController Tests", "appcontroller_tests") {}
    
    void runTest() override
    {
        // ═══════════════════════════════════════════════════════════════════
        // TEST 1: Auto-sélection après ajout de section
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Auto-sélection après ajout de section"));
        {
            AppController controller;
            controller.addNewSection("Section Test");
            
            // Vérifier que la section est créée
            expectEquals(controller.getSectionCount(), 1, "1 section créée");
            
            // Vérifier que le contrôleur a automatiquement sélectionné la nouvelle section
            auto& selState = controller.getSelectionState();
            juce::String selType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(selType == "Section", "Type de sélection = Section");
            
            // Vérifier que l'ID sélectionné correspond bien à la section 0
            auto section = controller.getPiece().getSection(0);
            juce::String expectedId = "Section_" + juce::String(section.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "ID sélectionné correct");
            
            logMessage(juce::String::fromUTF8("✓ Section auto-sélectionnée après ajout"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 2: Auto-sélection après ajout d'accord
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Auto-sélection après ajout d'accord"));
        {
            AppController controller;
            controller.addNewSection("Test");
            controller.addChordToSection(0, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            
            auto& selState = controller.getSelectionState();
            juce::String selType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(selType == "Chord", "Type de sélection = Chord");
            
            // Vérifier que l'ID de l'accord est correct
            auto section = controller.getPiece().getSection(0);
            auto progression = section.getProgression();
            auto chord = progression.getChord(0);
            juce::String expectedId = "Chord_" + juce::String(chord.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "ID accord sélectionné correct");
            
            logMessage(juce::String::fromUTF8("✓ Accord auto-sélectionné après ajout"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 3: Suppression d'une section sélectionnée nettoie la sélection
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Sécurité: Suppression section sélectionnée"));
        {
            AppController controller;
            controller.addNewSection("Section à supprimer");
            controller.selectSection(0);
            
            // Vérification pré-suppression
            auto& selState = controller.getSelectionState();
            juce::String preType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(preType == "Section", "Section sélectionnée avant suppression");
            
            // ACTION CRITIQUE : On supprime ce qui est sélectionné
            controller.removeSection(0);
            
            // VÉRIFICATION CRITIQUE : La sélection doit être nettoyée
            juce::String postType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(postType == "None", "Sélection nettoyée après suppression");
            
            logMessage(juce::String::fromUTF8("✓ Sélection nettoyée après suppression section"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 4: Suppression d'un accord sélectionné nettoie la sélection
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Sécurité: Suppression accord sélectionné"));
        {
            AppController controller;
            controller.addNewSection("Test");
            controller.addChordToSection(0, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            controller.selectChord(0, 0);
            
            // Vérification pré-suppression
            auto& selState = controller.getSelectionState();
            juce::String preType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(preType == "Chord", "Accord sélectionné avant suppression");
            
            // ACTION CRITIQUE : On supprime l'accord sélectionné
            controller.removeChordFromSection(0, 0);
            
            // VÉRIFICATION CRITIQUE : La sélection doit être nettoyée
            juce::String postType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(postType == "None", "Sélection nettoyée après suppression accord");
            
            logMessage(juce::String::fromUTF8("✓ Sélection nettoyée après suppression accord"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 5: Suppression d'une section NON-sélectionnée préserve la sélection
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Suppression section non-sélectionnée préserve sélection"));
        {
            AppController controller;
            controller.addNewSection("Section 1");
            controller.addNewSection("Section 2");
            
            // Sélectionner la première section
            controller.selectSection(0);
            
            auto& selState = controller.getSelectionState();
            juce::String preType = selState.getProperty(ContextIdentifiers::selectionType, "");
            juce::String preId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            
            // Supprimer la DEUXIÈME section (pas celle sélectionnée)
            controller.removeSection(1);
            
            // La sélection doit être préservée
            juce::String postType = selState.getProperty(ContextIdentifiers::selectionType, "");
            juce::String postId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            
            expect(postType == preType, "Type de sélection préservé");
            expect(postId == preId, "ID de sélection préservé");
            
            logMessage(juce::String::fromUTF8("✓ Sélection préservée lors suppression autre section"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 6: La sélection utilise l'ID (pas l'index)
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Sélection utilise ID et non index"));
        {
            AppController controller;
            controller.addNewSection("Section A");  // ID=0
            controller.addNewSection("Section B");  // ID=1
            controller.addNewSection("Section C");  // ID=2
            
            // Sélectionner la section du milieu (index=1, ID=1)
            controller.selectSection(1);
            
            auto& selState = controller.getSelectionState();
            juce::String selectedId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            
            // Vérifier que c'est bien l'ID qui est stocké
            auto sectionB = controller.getPiece().getSection(1);
            juce::String expectedId = "Section_" + juce::String(sectionB.getId());
            expect(selectedId == expectedId, "ID stocké (pas index)");
            
            // Supprimer la première section
            controller.removeSection(0);
            
            // La section B est maintenant à l'index 0, mais son ID n'a pas changé
            // La sélection doit toujours pointer vers le même élément
            juce::String afterId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(afterId == expectedId, "ID stable après suppression d'une autre section");
            
            logMessage(juce::String::fromUTF8("✓ Sélection par ID stable après réindexation"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 7: Sélection de modulation utilise l'ID
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Sélection modulation utilise ID"));
        {
            AppController controller;
            controller.addNewSection("Section 1");
            controller.addNewSection("Section 2");
            
            // Il y a maintenant une modulation entre les deux sections
            expectEquals(controller.getModulationCount(), 1, "1 modulation créée");
            
            controller.selectModulation(0);
            
            auto& selState = controller.getSelectionState();
            juce::String selType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(selType == "Modulation", "Type = Modulation");
            
            auto modulation = controller.getPiece().getModulation(0);
            juce::String expectedId = "Modulation_" + juce::String(modulation.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "ID modulation correct");
            
            logMessage(juce::String::fromUTF8("✓ Sélection modulation par ID"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 8: clearPiece nettoie la sélection
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("clearPiece nettoie la sélection"));
        {
            AppController controller;
            controller.addNewSection("Test");
            controller.selectSection(0);
            
            auto& selState = controller.getSelectionState();
            juce::String preType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(preType == "Section", "Section sélectionnée avant clear");
            
            controller.clearPiece();
            
            juce::String postType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(postType == "None", "Sélection nettoyée après clearPiece");
            expect(controller.isEmpty(), "Pièce vide après clearPiece");
            
            logMessage(juce::String::fromUTF8("✓ clearPiece nettoie tout"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 9: Index invalides ne crashent pas
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Robustesse: Index invalides ne crashent pas"));
        {
            AppController controller;
            
            // Ces appels ne doivent PAS crasher
            controller.selectSection(-1);       // Index négatif
            controller.selectSection(999);      // Index trop grand
            controller.removeSection(-1);       // Index négatif
            controller.removeSection(999);      // Index trop grand
            controller.selectModulation(-1);
            controller.selectModulation(999);
            controller.selectChord(-1, 0);
            controller.selectChord(0, -1);
            controller.selectChord(999, 999);
            controller.removeChordFromSection(-1, 0);
            controller.removeChordFromSection(0, -1);
            controller.addChordToSection(-1, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            
            // Si on arrive ici sans crash, c'est bon
            expect(true, "Aucun crash avec index invalides");
            
            logMessage(juce::String::fromUTF8("✓ Index invalides gérés sans crash"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 10: clearSelection est idempotent
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("clearSelection est idempotent"));
        {
            AppController controller;
            
            // Appeler clearSelection plusieurs fois ne doit pas poser problème
            controller.clearSelection();
            controller.clearSelection();
            controller.clearSelection();
            
            auto& selState = controller.getSelectionState();
            juce::String selType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(selType == "None", "Sélection reste None");
            
            logMessage(juce::String::fromUTF8("✓ clearSelection idempotent"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 11: Mode d'édition change avec la sélection
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Mode d'édition suit la sélection"));
        {
            AppController controller;
            controller.addNewSection("Test");
            controller.addChordToSection(0, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            
            // Sélection de section → SectionEdit
            controller.selectSection(0);
            expect(controller.getCurrentEditMode() == AppController::EditMode::SectionEdit, 
                   "Mode SectionEdit après selectSection");
            
            // Sélection d'accord → ChordEdit
            controller.selectChord(0, 0);
            expect(controller.getCurrentEditMode() == AppController::EditMode::ChordEdit, 
                   "Mode ChordEdit après selectChord");
            
            // Clear selection → Overview
            controller.clearSelection();
            expect(controller.getCurrentEditMode() == AppController::EditMode::Overview, 
                   "Mode Overview après clearSelection");
            
            logMessage(juce::String::fromUTF8("✓ Mode d'édition synchronisé avec sélection"));
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // TEST 12: Ajout de plusieurs accords avec IDs corrects
        // ═══════════════════════════════════════════════════════════════════
        beginTest(juce::String::fromUTF8("Ajout de plusieurs accords - IDs corrects"));
        {
            AppController controller;
            controller.addNewSection("Test");
            
            // Ajouter 3 accords
            controller.addChordToSection(0, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            controller.addChordToSection(0, Diatony::ChordDegree::Fourth, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            controller.addChordToSection(0, Diatony::ChordDegree::Fifth, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            
            auto section = controller.getPiece().getSection(0);
            auto progression = section.getProgression();
            
            expectEquals(static_cast<int>(progression.size()), 3, "3 accords créés");
            
            // Vérifier que le dernier accord est sélectionné
            auto& selState = controller.getSelectionState();
            auto lastChord = progression.getChord(2);
            juce::String expectedId = "Chord_" + juce::String(lastChord.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "Dernier accord sélectionné");
            
            logMessage(juce::String::fromUTF8("✓ Ajout multiple d'accords fonctionne"));
        }
    }
};

// Enregistrement statique du test
static AppControllerTest appControllerTest;

