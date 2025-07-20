#include "ColorGenerator.h"

//==============================================================================
juce::Colour ColorGenerator::generateRandomColor()
{
    static juce::Random random;
    const auto& palette = getDefaultPalette();
    return palette[random.nextInt(static_cast<int>(palette.size()))];
}

juce::Colour ColorGenerator::generateRandomColorHSL(float saturation, float lightness)
{
    static juce::Random random;
    
    // Générer une teinte aléatoire (0.0 à 1.0)
    float hue = random.nextFloat();
    
    return juce::Colour::fromHSV(hue, saturation, lightness, 1.0f);
}

const std::vector<juce::Colour>& ColorGenerator::getDefaultPalette()
{
    static const auto palette = createDefaultPalette();
    return palette;
}

std::vector<juce::Colour> ColorGenerator::createDefaultPalette()
{
    return {
        juce::Colours::lightcoral,      // Rose corail
        juce::Colours::lightblue,       // Bleu clair
        juce::Colours::lightgreen,      // Vert clair
        juce::Colours::lightyellow,     // Jaune clair
        juce::Colours::plum,            // Prune (violet)
        juce::Colours::lightcyan,       // Cyan clair
        juce::Colours::peachpuff,       // Pêche
        juce::Colours::lavender,        // Lavande
        juce::Colours::mistyrose,       // Rose brumeux
        juce::Colours::lightseagreen    // Vert mer clair
    };
} 