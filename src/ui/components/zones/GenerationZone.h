#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Zone de génération avec un style distinct (fond uni gris, sans en-tête)
 */
class GenerationZone : public juce::Component
{
public:
    GenerationZone()
    {
        setOpaque(false);
    }
    
    ~GenerationZone() override = default;
    
    void paint(juce::Graphics& g) override
    {
        const float cornerSize = 8.0f;
        const int zoneBorderThickness = 1;
        const int padding = 5; // Même padding interne que BaseZone
        
        // Zone interne pour le dessin, réduite par le padding
        auto drawingBounds = getLocalBounds().reduced(padding);
        
        // Dessiner un fond uni gris dans la zone interne
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        g.fillRoundedRectangle(drawingBounds.toFloat(), cornerSize);
        
        // Dessiner le contour autour de la zone interne
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawRoundedRectangle(drawingBounds.toFloat(), cornerSize, zoneBorderThickness);
        
        // Dessiner le texte centré dans la zone interne
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText(juce::String::fromUTF8("Génération"), drawingBounds, juce::Justification::centred, false);
    }
    
    void resized() override
    {
        // Positionner les composants ici quand il y en aura, en utilisant un padding interne
        const int padding = 5;
        auto contentBounds = getLocalBounds().reduced(padding);
        // Exemple: myButton->setBounds(contentBounds.reduced(10));
    }

private:
    // Composants d'interface utilisateur à ajouter ici
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenerationZone)
}; 