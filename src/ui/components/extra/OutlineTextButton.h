#pragma once

#include <JuceHeader.h>

namespace extra {

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
};

// Classe de bouton personnalisée utilisant OutlineButtonLookAndFeel
class OutlineTextButton : public juce::TextButton
{
public:
    OutlineTextButton(const juce::String& buttonText,
                      juce::Colour textColourToUse,
                      juce::Colour borderColourToUse,
                      float borderThicknessToUse)
        : juce::TextButton(buttonText),
          borderThickness(borderThicknessToUse)
    {
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

private:
    float borderThickness;
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

    if (button.isDown() || shouldDrawButtonAsDown)
        currentBorderColour = currentBorderColour.darker(0.2f);
    else if (button.isOver() || shouldDrawButtonAsHighlighted)
        currentBorderColour = currentBorderColour.brighter(0.2f); // Ou légèrement différent pour le survol

    g.setColour(currentBorderColour);
    // Important : Dessiner le rectangle, ne pas le remplir pour un bouton avec contour uniquement
    g.drawRoundedRectangle(bounds, cornerRadius, thickness);
}

} // namespace extra 