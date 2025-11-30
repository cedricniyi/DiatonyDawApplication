#include <JuceHeader.h>

/**
 * Point d'entrée pour exécuter les tests unitaires JUCE.
 * 
 * Usage:
 *   ./DiatonyTests              - Exécute tous les tests
 *   ./DiatonyTests Model        - Exécute les tests de la catégorie "Model"
 *   ./DiatonyTests "Piece"      - Exécute les tests contenant "Piece" dans le nom
 */
int main(int argc, char* argv[])
{
    // Initialiser JUCE (nécessaire pour certaines classes)
    juce::ScopedJuceInitialiser_GUI juceInit;
    
    // Créer le runner de tests
    juce::UnitTestRunner runner;
    
    // Configuration du runner
    runner.setAssertOnFailure(false);  // Ne pas crasher sur les échecs, juste les reporter
    
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           DIATONY DAW - TESTS UNITAIRES                       ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Vérifier s'il y a un argument de catégorie
    if (argc > 1)
    {
        juce::String category(argv[1]);
        std::cout << "📋 Exécution des tests de catégorie: " << category << std::endl;
        std::cout << std::endl;
        runner.runTestsInCategory(category);
    }
    else
    {
        std::cout << "📋 Exécution de TOUS les tests..." << std::endl;
        std::cout << std::endl;
        runner.runAllTests();
    }
    
    // Afficher le résumé
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "RÉSUMÉ DES TESTS" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    
    int totalTests = 0;
    int failedTests = 0;
    
    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        auto* result = runner.getResult(i);
        if (result != nullptr)
        {
            totalTests += result->passes + result->failures;
            failedTests += result->failures;
            
            juce::String status = (result->failures == 0) ? juce::String::fromUTF8("✅ PASS") : juce::String::fromUTF8("❌ FAIL");
            std::cout << status << " " << result->unitTestName 
                      << " (" << result->passes << "/" << (result->passes + result->failures) << ")"
                      << std::endl;
            
            // Afficher les messages d'erreur s'il y en a
            for (auto& msg : result->messages)
            {
                std::cout << "      ⚠️  " << msg << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    
    if (failedTests == 0)
    {
        std::cout << "🎉 TOUS LES TESTS PASSENT! (" << totalTests << " assertions)" << std::endl;
    }
    else
    {
        std::cout << "💥 " << failedTests << " TEST(S) ÉCHOUÉ(S) sur " << totalTests << std::endl;
    }
    
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    
    return failedTests > 0 ? 1 : 0;
}

