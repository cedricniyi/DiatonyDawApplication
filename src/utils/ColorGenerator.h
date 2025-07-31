#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/**
 * Utilitaire pour générer des couleurs aléatoires harmonieuses
 */
class ColorGenerator
{
public:
    /**
     * Génère une couleur aléatoire à partir d'une palette prédéfinie
     */
    static juce::Colour generateRandomColor();
    
    /**
     * Génère une couleur aléatoire avec une saturation et luminosité contrôlées
     */
    static juce::Colour generateRandomColorHSL(float saturation = 0.7f, float lightness = 0.8f);
    
    /**
     * Obtient la palette de couleurs par défaut
     */
    static const std::vector<juce::Colour>& getDefaultPalette();

private:
    static std::vector<juce::Colour> createDefaultPalette();
    
    ColorGenerator() = delete; // Classe utilitaire - pas d'instanciation
}; 