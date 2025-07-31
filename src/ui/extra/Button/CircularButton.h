#pragma once

#include <JuceHeader.h>
#include "utils/FontManager.h"

/**
 * Look-and-Feel personnalisé pour dessiner des boutons circulaires simples
 */
class CircularButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CircularButtonLookAndFeel() = default;
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, 
                            bool shouldDrawButtonAsDown) override;
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted, 
                       bool shouldDrawButtonAsDown) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CircularButtonLookAndFeel)
};

/**
 * Bouton circulaire réutilisable basé sur TextButton avec Look-and-Feel personnalisé
 * Style simple comme ButtonColoredPanel avec gestion de sélection
 */
class CircularButton : public juce::TextButton
{
public:
    CircularButton(const juce::String& buttonText = "",
                  juce::Colour baseColourToUse = juce::Colours::lightblue,
                  juce::Colour textColourToUse = juce::Colours::black,
                  float fontSize = 14.0f,
                  FontManager::FontWeight fontWeight = FontManager::FontWeight::Medium);
    ~CircularButton() override;
    
    // Gestion de la sélection (comme ButtonColoredPanel)
    void setSelected(bool selected);
    bool getSelected() const;
    
    // Configuration des couleurs
    void setBaseColour(const juce::Colour& colour);
    juce::Colour getBaseColour() const;
    
    void setTextColour(const juce::Colour& colour);
    juce::Colour getTextColour() const;
    
    // Accès à la font personnalisée (comme OutlineTextButton)
    juce::FontOptions getCustomFont() const;

private:
    bool isSelected;
    juce::Colour baseColour;
    juce::Colour normalTextColour;
    juce::FontOptions customFont;
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // LookAndFeel statique partagé
    static CircularButtonLookAndFeel sharedLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CircularButton)
};