#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Zone de génération avec un titre à gauche et une zone de contenu à droite.
 */
class GenerationZone : public juce::Component
{
public:
    GenerationZone()
    {
        setOpaque(false); // Nécessaire pour voir le fond dessiné dans paint()

        // Initialisation du Label
        generationLabel = std::make_unique<juce::Label>("generationLabel", juce::String::fromUTF8("Solution"));
        generationLabel->setFont(juce::FontOptions(20.0f, juce::Font::bold)); // Utilisation de juce::Font
        generationLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        // La justification clé : centré horizontalement ET verticalement
        generationLabel->setJustificationType(juce::Justification::centred); 
        addAndMakeVisible(*generationLabel);
    }
    
    ~GenerationZone() override = default;
    
    void paint(juce::Graphics& g) override
    {
        const float cornerSize = 8.0f;
        const int zoneBorderThickness = 1;
        const int padding = 5; // Padding unique autour du contenu
        
        // Zone pour le fond et la bordure
        auto bounds = getLocalBounds().reduced(padding);
        
        // Dessiner le fond uni gris
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
        
        // Dessiner le contour
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, zoneBorderThickness);
        
        // Dessiner la ligne de séparation verticale
        if (bounds.getWidth() > titleWidth)
        {
            g.setColour(juce::Colours::grey.withAlpha(0.3f));
            g.drawLine(padding + titleWidth, padding, 
                      padding + titleWidth, getHeight() - padding, 
                      1.0f);
        }
    }
    
    void resized() override
    {
        const int padding = 5; // Utiliser le même padding que dans paint()
        
        // La zone disponible pour le contenu est toute la zone interne après padding
        auto contentBounds = getLocalBounds().reduced(padding);
        
        // Créer la zone du titre à gauche
        auto titleZone = contentBounds.removeFromLeft(titleWidth);
        
        // Espace restant pour le contenu à droite
        auto contentZone = contentBounds;

        // Si le label existe, le placer dans la zone du titre
        if (generationLabel)
        {
            generationLabel->setBounds(titleZone);
        }
    }

private:
    std::unique_ptr<juce::Label> generationLabel;
    const int titleWidth = 150; // Largeur fixe pour la zone du titre
    
    // Les pointeurs pour les boutons sont supprimés pour cette version simplifiée
    // std::unique_ptr<juce::TextButton> generateButton;
    // std::unique_ptr<juce::ArrowButton> playButton;
    // La classe LookAndFeel est aussi supprimée car non utilisée ici

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationZone)
}; 