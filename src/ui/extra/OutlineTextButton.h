#pragma once

#include <JuceHeader.h>
#include "../../utils/FontManager.h"

// Classe LookAndFeel personnalisée pour dessiner un bouton avec contour uniquement
class OutlineButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OutlineButtonLookAndFeel() = default;

    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour, // Sera utilisée comme couleur de contour
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;
                               
    void drawButtonText (juce::Graphics& g, juce::TextButton& button, 
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
                      FontManager::FontWeight fontWeight = FontManager::FontWeight::Regular)
        : juce::TextButton(buttonText),
          borderThickness(borderThicknessToUse),
          normalTextColour(textColourToUse),
          normalBorderColour(borderColourToUse)
    {
        // Initialiser la font après que fontManager soit disponible
        customFont = fontManager->getSFProDisplay(fontSize, fontWeight);
        
        setColour(juce::TextButton::textColourOnId, textColourToUse);
        setColour(juce::TextButton::textColourOffId, textColourToUse);
        // Utiliser buttonColourId pour passer la couleur du contour au LookAndFeel
        setColour(juce::TextButton::buttonColourId, borderColourToUse);

        // Utiliser une instance statique de LookAndFeel
        static OutlineButtonLookAndFeel sharedLookAndFeel;
        setLookAndFeel(&sharedLookAndFeel);
    }

    ~OutlineTextButton() override
    {
        setLookAndFeel(nullptr); // Nettoyer le LookAndFeel
    }

    float getBorderThickness() const { return borderThickness; }
    
    // Permet de connaître les couleurs normales (non-désactivées)
    juce::Colour getNormalTextColour() const { return normalTextColour; }
    juce::Colour getNormalBorderColour() const { return normalBorderColour; }
    
    // Permet d'accéder à la font personnalisée
    juce::FontOptions getCustomFont() const { return customFont; }

private:
    float borderThickness;
    juce::Colour normalTextColour;    // Stockage des couleurs normales pour les comparer
    juce::Colour normalBorderColour;  // quand le bouton est désactivé
    juce::FontOptions customFont;     // Font personnalisée
    juce::SharedResourcePointer<FontManager> fontManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutlineTextButton)
};

// Définition de la méthode drawButtonBackground (après la définition complète de OutlineTextButton)
inline void OutlineButtonLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                                           juce::Button& button,
                                                           const juce::Colour& backgroundColour,
                                                           bool shouldDrawButtonAsHighlighted,
                                                           bool shouldDrawButtonAsDown)
{
    auto* outlineButton = dynamic_cast<OutlineTextButton*>(&button);
    if (!outlineButton)
    {
        jassertfalse; // Devrait toujours être un OutlineTextButton
        return;
    }

    float thickness = outlineButton->getBorderThickness();
    // Réduire les limites par la moitié de l'épaisseur pour un dessin correct du contour externe
    auto bounds = button.getLocalBounds().toFloat().reduced(thickness * 0.5f);
    float cornerRadius = 6.0f; // Rayon de coin fixe

    juce::Colour currentBorderColour = backgroundColour;

    // Gérer l'état désactivé en réduisant l'opacité
    if (!button.isEnabled())
    {
        // Réduire l'opacité à 30%
        currentBorderColour = currentBorderColour.withAlpha(0.3f);
    }
    else if (button.isDown() || shouldDrawButtonAsDown)
        currentBorderColour = currentBorderColour.darker(0.2f);
    else if (button.isOver() || shouldDrawButtonAsHighlighted)
        currentBorderColour = currentBorderColour.brighter(0.2f);

    g.setColour(currentBorderColour);
    // Important : Dessiner le rectangle, ne pas le remplir pour un bouton avec contour uniquement
    g.drawRoundedRectangle(bounds, cornerRadius, thickness);
}

// Définition de la méthode drawButtonText
inline void OutlineButtonLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, 
                                                     bool /*shouldDrawButtonAsHighlighted*/, 
                                                     bool /*shouldDrawButtonAsDown*/)
{
    // Utiliser la font personnalisée si disponible
    auto* outlineButton = dynamic_cast<OutlineTextButton*>(&button);
    if (outlineButton)
    {
        g.setFont(juce::Font(outlineButton->getCustomFont()));
    }
    else
    {
        g.setFont(getTextButtonFont(button, button.getHeight()));
    }
    
    // Choisir la couleur du texte selon l'état du bouton
    juce::Colour textColour = button.findColour(button.getToggleState() ? 
                                                juce::TextButton::textColourOnId : 
                                                juce::TextButton::textColourOffId);
    
    // Si le bouton est désactivé, réduire l'opacité du texte
    if (!button.isEnabled())
    {
        textColour = textColour.withAlpha(0.3f);
    }

    g.setColour(textColour);
    
    const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;
    
    const int leftIndent = cornerSize / 2;
    const int rightIndent = cornerSize / 2;
    const int textWidth = button.getWidth() - leftIndent - rightIndent;
    
    g.drawFittedText(button.getButtonText(),
                     leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                     juce::Justification::centred, 2);
}