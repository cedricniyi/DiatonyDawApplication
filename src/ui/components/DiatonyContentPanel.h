#pragma once

#include <JuceHeader.h>
#include "keyboard/InteractiveKeyboard.h"
#include "zones/TonalityZone.h"
#include "zones/ModeZone.h"
#include "zones/ProgressionZone.h"
#include "zones/GenerationZone.h"

//==============================================================================
/**
 * Panel principal pour le mode Diatony
 * Contient et arrange les différentes zones de paramètres
 */
class DiatonyContentPanel : public juce::Component
{
public:
    DiatonyContentPanel()
    {
        setOpaque(true);
        
        // Créer les zones de paramètres
        tonalityZone = std::make_unique<TonalityZone>();
        modeZone = std::make_unique<ModeZone>();
        progressionZone = std::make_unique<ProgressionZone>();
        generationZone = std::make_unique<GenerationZone>();
        
        // Créer et configurer le clavier interactif
        keyboardComponent = std::make_unique<InteractiveKeyboard>();
        
        // Ajouter les composants à l'interface
        addAndMakeVisible(*tonalityZone);
        addAndMakeVisible(*modeZone);
        addAndMakeVisible(*progressionZone);
        addAndMakeVisible(*generationZone);
        addAndMakeVisible(*keyboardComponent);
        
    }

    ~DiatonyContentPanel() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        // Couleur de fond pour tout le panel
        g.fillAll(juce::Colour(0xFF2A2A2A));
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Réserver de l'espace en bas pour le clavier
        int keyboardHeight = 200;
        auto keyboardBounds = bounds.removeFromBottom(keyboardHeight);
        
        // Positionner le clavier
        keyboardComponent->setBounds(keyboardBounds);
        
        // Espace restant pour les zones de paramètres, AVEC padding global
        const int globalPadding = 10; 
        auto contentBounds = bounds.reduced(globalPadding);
        
        // Espace vertical entre les rangées de zones
        const int verticalGap = 10;
        
        // Zone 1: Tonalité et Mode (1/3 supérieur de l'espace disponible)
        auto row1Height = (contentBounds.getHeight() - 2 * verticalGap) / 3;
        auto tonalityModeArea = contentBounds.removeFromTop(row1Height);
        
        // Zone 2: Progression (agrandie à 75% de l'espace restant)
        contentBounds.removeFromTop(verticalGap);
        auto remainingHeight = contentBounds.getHeight();
        auto progressionAreaHeight = static_cast<int>(remainingHeight * 0.75f);
        auto progressionArea = contentBounds.removeFromTop(progressionAreaHeight);
        
        // Zone 3: Génération (réduite à 25% de l'espace restant)
        contentBounds.removeFromTop(verticalGap);
        auto generationArea = contentBounds;
        
        // --- Positionnement des composants --- 
        
        // Tonalité et Mode
        const int horizontalGap = 10; // Espace entre Tonalité et Mode
        auto tonalityArea = tonalityModeArea;
        auto modeArea = tonalityArea.removeFromRight(static_cast<int>(tonalityModeArea.getWidth() * 0.2f));
        tonalityArea.removeFromRight(horizontalGap); // Créer l'espace
        
        tonalityZone->setBounds(tonalityArea);
        modeZone->setBounds(modeArea);
        
        // Progression
        progressionZone->setBounds(progressionArea);
        
        // Génération
        generationZone->setBounds(generationArea);
    }

private:
    // Zones de paramètres
    std::unique_ptr<TonalityZone> tonalityZone;
    std::unique_ptr<ModeZone> modeZone;
    std::unique_ptr<ProgressionZone> progressionZone;
    std::unique_ptr<GenerationZone> generationZone;
    
    // Clavier interactif
    std::unique_ptr<InteractiveKeyboard> keyboardComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyContentPanel)
}; 