#include "CircularButton.h"

//==============================================================================
// CircularButtonLookAndFeel Implementation

void CircularButtonLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                                                   const juce::Colour& backgroundColour,
                                                   bool shouldDrawButtonAsHighlighted, 
                                                   bool shouldDrawButtonAsDown)
{
    auto* circularButton = dynamic_cast<CircularButton*>(&button);
    if (!circularButton)
        return;
    
    auto bounds = button.getLocalBounds().toFloat();
    auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto radius = diameter * 0.5f;
    
    // Centrer le cercle dans les bounds du bouton
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto circleBounds = juce::Rectangle<float>(centreX - radius, centreY - radius, diameter, diameter);
    
    // Couleur avec effets hover et sélection (comme ButtonColoredPanel)
    juce::Colour currentColor = circularButton->getBaseColour();
    
    if (shouldDrawButtonAsDown)
        currentColor = currentColor.darker(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        currentColor = currentColor.brighter(0.15f);
    
    // Dessiner le cercle plein (style simple, pas de gradient)
    g.setColour(currentColor);
    g.fillEllipse(circleBounds);
}

void CircularButtonLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                             bool shouldDrawButtonAsHighlighted, 
                                             bool shouldDrawButtonAsDown)
{
    // Utiliser la font personnalisée si disponible (comme OutlineTextButton)
    auto* circularButton = dynamic_cast<CircularButton*>(&button);
    if (circularButton)
    {
        g.setFont(juce::Font(circularButton->getCustomFont()));
        g.setColour(circularButton->getTextColour());
    }
    else
    {
        // Utiliser FontOptions pour éviter le warning de dépréciation
        g.setFont(juce::Font(juce::FontOptions(14.0f)));
        g.setColour(juce::Colours::black);
    }
    
    auto bounds = button.getLocalBounds();
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true);
}

//==============================================================================
// CircularButton Implementation

CircularButton::CircularButton(const juce::String& buttonText,
                              juce::Colour baseColourToUse,
                              juce::Colour textColourToUse,
                              float fontSize,
                              FontManager::FontWeight fontWeight)
    : juce::TextButton(buttonText),
      isSelected(false),
      baseColour(baseColourToUse),
      normalTextColour(textColourToUse)
{
    // Initialiser la font avec FontManager (comme OutlineTextButton)
    customFont = fontManager->getSFProDisplay(fontSize, fontWeight);
    
    // Utiliser une instance statique locale de LookAndFeel (plus sûr que static membre)
    static CircularButtonLookAndFeel sharedLookAndFeel;
    setLookAndFeel(&sharedLookAndFeel);
}

CircularButton::~CircularButton()
{
    // Nettoyer le LookAndFeel
    setLookAndFeel(nullptr);
}

void CircularButton::setSelected(bool selected)
{
    if (isSelected != selected)
    {
        isSelected = selected;
        repaint();
    }
}

bool CircularButton::getSelected() const
{
    return isSelected;
}

void CircularButton::setBaseColour(const juce::Colour& colour)
{
    baseColour = colour;
    repaint();
}

juce::Colour CircularButton::getBaseColour() const
{
    return baseColour;
}

void CircularButton::setTextColour(const juce::Colour& colour)
{
    normalTextColour = colour;
    repaint();
}

juce::Colour CircularButton::getTextColour() const
{
    return normalTextColour;
}

juce::FontOptions CircularButton::getCustomFont() const
{
    return customFont;
}