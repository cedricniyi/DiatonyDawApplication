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

        auto bounds = getLocalBounds();
        auto keyboardHeight = 200; // Hauteur réservée pour le clavier
        
        // Espace restant après avoir retiré le clavier
        auto contentBounds = bounds;
        contentBounds.removeFromBottom(keyboardHeight);
        
        // Diviser l'espace restant en 3 zones égales
        const int zonePadding = 10; // Espacement entre zones
        
        // Zone 1: Tonalité et Mode (1/3 supérieur)
        auto tonalityModeArea = contentBounds.removeFromTop(contentBounds.getHeight() / 3);
        
        // Sous-division de la zone tonalité/mode
        auto tonalityZone = tonalityModeArea;
        auto modeZone = tonalityZone.removeFromRight(static_cast<int>(tonalityModeArea.getWidth() * 0.2f));
        
        // Appliquer le padding
        tonalityZone = tonalityZone.reduced(zonePadding);
        modeZone = modeZone.reduced(zonePadding);
        
        // Zone 2: Progression
        auto progressionZone = contentBounds.removeFromTop(contentBounds.getHeight() / 2);
        progressionZone = progressionZone.reduced(zonePadding);
        
        // Zone 3: Génération
        auto generationZone = contentBounds;
        generationZone = generationZone.reduced(zonePadding);
        
        // Dessiner les zones avec leurs titres
        drawZoneWithTitle(g, tonalityZone, juce::String::fromUTF8("Tonalité"));
        drawZoneWithTitle(g, modeZone, juce::String::fromUTF8("Mode"));
        drawZoneWithTitle(g, progressionZone, juce::String::fromUTF8("Progression"));
        drawZoneWithTitle(g, generationZone, juce::String::fromUTF8("Génération"));
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Réserver de l'espace en bas pour le clavier
        int keyboardHeight = 200;
        auto keyboardBounds = bounds.removeFromBottom(keyboardHeight);
        
        // Positionner le clavier
        keyboardComponent->setBounds(keyboardBounds);
        
        // Les zones sont dessinées dans la méthode paint
        // et ne nécessitent pas de positionnement particulier ici
    }

private:
    // Méthode pour dessiner une zone avec un titre
    void drawZoneWithTitle(juce::Graphics& g, const juce::Rectangle<int>& zone, const juce::String& title)
    {
        const int titleHeight = 30;
        const int zoneBorderThickness = 1;
        const float cornerSize = 8.0f;
        
        // Séparer la zone en en-tête et corps
        auto titleArea = zone.withHeight(titleHeight);
        auto contentArea = zone.withTrimmedTop(titleHeight);
        
        // Dessiner le contour de la zone complète
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(zone.toFloat(), cornerSize, zoneBorderThickness);
        
        // Dessiner le corps de la zone
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        
        // Créer un chemin pour le corps avec coins arrondis en bas seulement
        juce::Path contentPath;
        contentPath.addRoundedRectangle(
            contentArea.getX(), contentArea.getY(),
            contentArea.getWidth(), contentArea.getHeight(),
            cornerSize, cornerSize,
            false, false, // Pas de coins arrondis en haut
            true, true    // Coins arrondis en bas
        );
        g.fillPath(contentPath);
        
        // Dessiner l'en-tête
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        
        // Créer un chemin pour l'en-tête avec coins arrondis en haut seulement
        juce::Path headerPath;
        headerPath.addRoundedRectangle(
            titleArea.getX(), titleArea.getY(),
            titleArea.getWidth(), titleArea.getHeight(),
            cornerSize, cornerSize,
            true, true,  // Coins arrondis en haut
            false, false // Pas de coins arrondis en bas
        );
        g.fillPath(headerPath);
        
        // Dessiner le texte du titre
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        
        g.drawText(title, titleArea, juce::Justification::centred, false);
    }

    std::unique_ptr<InteractiveKeyboard> keyboardComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyContentPanel)
}; 