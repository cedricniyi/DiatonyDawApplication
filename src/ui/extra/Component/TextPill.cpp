#include "TextPill.h"
#include "../../../utils/FontManager.h"

TextPill::TextPill(juce::Colour backgroundColor) 
    : bgColor(backgroundColor), useBackground(backgroundColor != juce::Colours::transparentBlack)
{
    setOpaque(false);
}

void TextPill::setText(const juce::String& firstPart, 
                      const juce::String& secondPart,
                      float fontSize1,
                      FontManager::FontWeight fontWeight1,
                      float fontSize2,
                      FontManager::FontWeight fontWeight2)
{
    text1 = firstPart;
    text2 = secondPart;
    font1Size = fontSize1;
    font1Weight = fontWeight1;
    font2Size = fontSize2;
    font2Weight = fontWeight2;
    repaint();
}

void TextPill::setTextColor(juce::Colour color)
{
    textColor = color;
    repaint();
}

void TextPill::setTextJustification(juce::Justification justification)
{
    textJustification = justification;
    repaint();
}

void TextPill::setBackgroundColor(juce::Colour color)
{
    bgColor = color;
    useBackground = (color != juce::Colours::transparentBlack);
    repaint();
}

void TextPill::setCornerRadius(float radius)
{
    cornerRadius = radius;
    repaint();
}

void TextPill::setBorderEnabled(bool enabled)
{
    useBorder = enabled;
    repaint();
}

void TextPill::setBorder(juce::Colour borderColor, float thickness)
{
    borderColour = borderColor;
    borderThickness = thickness;
    useBorder = true;
    repaint();
}

void TextPill::setBorderColour(juce::Colour borderColor)
{
    borderColour = borderColor;
    repaint();
}

void TextPill::setBorderThickness(float thickness)
{
    borderThickness = thickness;
    repaint();
}

bool TextPill::isBorderEnabled() const { return useBorder; }
juce::Colour TextPill::getBorderColour() const { return borderColour; }
float TextPill::getBorderThickness() const { return borderThickness; }

void TextPill::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    if (useBackground)
    {
        auto backgroundBounds = useBorder ? bounds.reduced(borderThickness * 0.5f) : bounds;
        g.setColour(bgColor);
        g.fillRoundedRectangle(backgroundBounds, cornerRadius);
    }
    
    if (useBorder)
    {
        auto borderBounds = bounds.reduced(borderThickness * 0.5f);
        g.setColour(borderColour);
        g.drawRoundedRectangle(borderBounds, cornerRadius, borderThickness);
    }
    
    auto attributedText = createAttributedText();
    attributedText.setJustification(textJustification);
    attributedText.draw(g, bounds);
}

juce::AttributedString TextPill::createAttributedText()
{
    juce::AttributedString attributedString;
    
    if (!text1.isEmpty())
        attributedString.append(text1, juce::Font(fontManager->getSFProText(font1Size, font1Weight)), textColor);
    
    if (!text2.isEmpty())
        attributedString.append(text2, juce::Font(fontManager->getSFProText(font2Size, font2Weight)), textColor);
    
    return attributedString;
}
