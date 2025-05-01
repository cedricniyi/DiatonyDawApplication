#pragma once

#include <JuceHeader.h>
#include "InteractiveKeyboard.h"

//==============================================================================
class DiatonyContentPanel : public juce::Component
{
public:
    DiatonyContentPanel()
    {
        setOpaque(true);
        
        // Créer et configurer le clavier interactif
        keyboardComponent = std::make_unique<InteractiveKeyboard>();
        addAndMakeVisible(*keyboardComponent);
        
        // Nous n'activons plus aucune touche automatiquement
        // Le clavier sera affiché en gris avec les étiquettes standard
        
        // Configurer le callback pour les touches
        keyboardComponent->onKeyPressed = [this](int keyCode) {
            // Juste pour déboguer: afficher le code de la touche pressée
            DBG("Touche pressée: " + juce::String(keyCode));
        };
    }

    ~DiatonyContentPanel() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF2A2A2A));

        // Dessiner le titre
        g.setColour(juce::Colours::orange.withAlpha(0.8f));
        g.setFont(20.0f);
        
        auto bounds = getLocalBounds();
        auto keyboardHeight = 200; // Hauteur réservée pour le clavier (augmentée à 200)
        
        // Zone de titre au-dessus du reste
        auto titleBounds = bounds;
        titleBounds.removeFromBottom(keyboardHeight); // Laisser la place au clavier
        
        g.drawText("Mode Diatony", titleBounds,
                   juce::Justification::centred, true);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Réserver de l'espace en bas pour le clavier
        int keyboardHeight = 200; // Hauteur augmentée à 200
        auto keyboardBounds = bounds.removeFromBottom(keyboardHeight);
        
        // Positionner le clavier
        keyboardComponent->setBounds(keyboardBounds);
    }

private:
    std::unique_ptr<InteractiveKeyboard> keyboardComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyContentPanel)
}; 