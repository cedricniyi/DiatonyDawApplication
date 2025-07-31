#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"

// Classe LookAndFeel personnalisée pour dessiner un bouton avec contour uniquement
class OutlineButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OutlineButtonLookAndFeel() = default;

    void drawButtonBackground(juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour, // Sera utilisée comme couleur de contour
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;
                               
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, 
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

// Classe de bouton personnalisée utilisant OutlineButtonLookAndFeel
class OutlineTextButton : public juce::TextButton
{
public:
    OutlineTextButton(const juce::String& buttonText,
                      juce::Colour textColourToUse,
                      juce::Colour borderColourToUse,
                      float borderThicknessToUse,
                      float fontSize = 14.0f,
                      FontManager::FontWeight fontWeight = FontManager::FontWeight::Regular);

    ~OutlineTextButton() override;

    float getBorderThickness() const;
    
    // Permet de connaître les couleurs normales (non-désactivées)
    juce::Colour getNormalTextColour() const;
    juce::Colour getNormalBorderColour() const;
    
    // Permet d'accéder à la font personnalisée
    juce::FontOptions getCustomFont() const;

private:
    float borderThickness;
    juce::Colour normalTextColour;    // Stockage des couleurs normales pour les comparer
    juce::Colour normalBorderColour;  // quand le bouton est désactivé
    juce::FontOptions customFont;     // Font personnalisée
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutlineTextButton)
};