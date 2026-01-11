#include <JuceHeader.h>
#include "controller/AppController.h"
#include "controller/ContextIdentifiers.h"
#include "model/ModelIdentifiers.h"
#include "model/DiatonyTypes.h"

/**
 * @brief Tests unitaires pour l'AppController.
 *
 * Vérifie : orchestration (auto-sélection), sécurité (nettoyage sélection), traduction Index→ID, robustesse (index invalides ne crashent pas).
 */
class AppControllerTest : public juce::UnitTest
{
public:
    AppControllerTest() : juce::UnitTest("AppController Tests", "appcontroller_tests") {}
    
    void runTest() override
    {
        beginTest(juce::String::fromUTF8("Auto-sélection après ajout de section"));
        {
            AppController controller;
            controller.addNewSection("Section Test");
            
            expectEquals(controller.getSectionCount(), 1, "1 section créée");
            
            auto& selState = controller.getSelectionState();
            juce::String selType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(selType == "Section", "Type de sélection = Section");
            
            auto section = controller.getPiece().getSection(0);
            juce::String expectedId = "Section_" + juce::String(section.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "ID sélectionné correct");
            
            logMessage(juce::String::fromUTF8("✓ Section auto-sélectionnée après ajout"));
        }
        
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
            
            auto section = controller.getPiece().getSection(0);
            auto progression = section.getProgression();
            auto chord = progression.getChord(0);
            juce::String expectedId = "Chord_" + juce::String(chord.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "ID accord sélectionné correct");
            
            logMessage(juce::String::fromUTF8("✓ Accord auto-sélectionné après ajout"));
        }
        
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
        
        beginTest(juce::String::fromUTF8("Sécurité: Suppression accord sélectionné"));
        {
            AppController controller;
            controller.addNewSection("Test");
            controller.addChordToSection(0, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            controller.selectChord(0, 0);
            
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
        
        beginTest(juce::String::fromUTF8("Suppression section non-sélectionnée préserve sélection"));
        {
            AppController controller;
            controller.addNewSection("Section 1");
            controller.addNewSection("Section 2");
            
            controller.selectSection(0);
            
            auto& selState = controller.getSelectionState();
            juce::String preType = selState.getProperty(ContextIdentifiers::selectionType, "");
            juce::String preId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            
            controller.removeSection(1);
            
            juce::String postType = selState.getProperty(ContextIdentifiers::selectionType, "");
            juce::String postId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            
            expect(postType == preType, "Type de sélection préservé");
            expect(postId == preId, "ID de sélection préservé");
            
            logMessage(juce::String::fromUTF8("✓ Sélection préservée lors suppression autre section"));
        }
        
        beginTest(juce::String::fromUTF8("Sélection utilise ID et non index"));
        {
            AppController controller;
            controller.addNewSection("Section A");  // ID=0
            controller.addNewSection("Section B");  // ID=1
            controller.addNewSection("Section C");  // ID=2
            
            controller.selectSection(1);
            
            auto& selState = controller.getSelectionState();
            juce::String selectedId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            
            auto sectionB = controller.getPiece().getSection(1);
            juce::String expectedId = "Section_" + juce::String(sectionB.getId());
            expect(selectedId == expectedId, "ID stocké (pas index)");
            
            controller.removeSection(0);
            
            juce::String afterId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(afterId == expectedId, "ID stable après suppression d'une autre section");
            
            logMessage(juce::String::fromUTF8("✓ Sélection par ID stable après réindexation"));
        }
        
        beginTest(juce::String::fromUTF8("Sélection modulation utilise ID"));
        {
            AppController controller;
            controller.addNewSection("Section 1");
            controller.addNewSection("Section 2");
            
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
        
        beginTest(juce::String::fromUTF8("Robustesse: Index invalides ne crashent pas"));
        {
            AppController controller;
            
            controller.selectSection(-1);
            controller.selectSection(999);
            controller.removeSection(-1);
            controller.removeSection(999);
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
            
            expect(true, "Aucun crash avec index invalides");
            
            logMessage(juce::String::fromUTF8("✓ Index invalides gérés sans crash"));
        }
        
        beginTest(juce::String::fromUTF8("clearSelection est idempotent"));
        {
            AppController controller;
            
            controller.clearSelection();
            controller.clearSelection();
            controller.clearSelection();
            
            auto& selState = controller.getSelectionState();
            juce::String selType = selState.getProperty(ContextIdentifiers::selectionType, "");
            expect(selType == "None", "Sélection reste None");
            
            logMessage(juce::String::fromUTF8("✓ clearSelection idempotent"));
        }
        
        beginTest(juce::String::fromUTF8("Mode d'édition suit la sélection"));
        {
            AppController controller;
            controller.addNewSection("Test");
            controller.addChordToSection(0, Diatony::ChordDegree::First, 
                                         Diatony::ChordQuality::Auto, 
                                         Diatony::ChordState::Fundamental);
            
            controller.selectSection(0);
            expect(controller.getCurrentEditMode() == AppController::EditMode::SectionEdit, 
                   "Mode SectionEdit après selectSection");
            
            controller.selectChord(0, 0);
            expect(controller.getCurrentEditMode() == AppController::EditMode::ChordEdit, 
                   "Mode ChordEdit après selectChord");
            
            controller.clearSelection();
            expect(controller.getCurrentEditMode() == AppController::EditMode::Overview, 
                   "Mode Overview après clearSelection");
            
            logMessage(juce::String::fromUTF8("✓ Mode d'édition synchronisé avec sélection"));
        }
        
        beginTest(juce::String::fromUTF8("Ajout de plusieurs accords - IDs corrects"));
        {
            AppController controller;
            controller.addNewSection("Test");
            
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
            
            auto& selState = controller.getSelectionState();
            auto lastChord = progression.getChord(2);
            juce::String expectedId = "Chord_" + juce::String(lastChord.getId());
            juce::String actualId = selState.getProperty(ContextIdentifiers::selectedElementId, "");
            expect(actualId == expectedId, "Dernier accord sélectionné");
            
            logMessage(juce::String::fromUTF8("✓ Ajout multiple d'accords fonctionne"));
        }
    }
};

static AppControllerTest appControllerTest;
