#include "OutlineTextButton.h"
#include "utils/FontManager.h"

//==============================================================================
// OutlineButtonLookAndFeel Implementation
//==============================================================================

void OutlineButtonLookAndFeel::drawButtonBackground(juce::Graphics& g,
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

void OutlineButtonLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, 
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

//==============================================================================
// OutlineTextButton Implementation
//==============================================================================

OutlineTextButton::OutlineTextButton(const juce::String& buttonText,
                                   juce::Colour textColourToUse,
                                   juce::Colour borderColourToUse,
                                   float borderThicknessToUse,
                                   float fontSize,
                                   FontManager::FontWeight fontWeight)
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

OutlineTextButton::~OutlineTextButton()
{
    setLookAndFeel(nullptr); // Nettoyer le LookAndFeel
}

float OutlineTextButton::getBorderThickness() const
{
    return borderThickness;
}

juce::Colour OutlineTextButton::getNormalTextColour() const
{
    return normalTextColour;
}

juce::Colour OutlineTextButton::getNormalBorderColour() const
{
    return normalBorderColour;
}

juce::FontOptions OutlineTextButton::getCustomFont() const
{
    return customFont;
} 