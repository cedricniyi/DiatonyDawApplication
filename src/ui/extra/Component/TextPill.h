#pragma once

#include <JuceHeader.h>
#include "../../../utils/FontManager.h"

/**
 * @brief Composant pour afficher du texte avec deux parties ayant des polices différentes.
 */
class TextPill : public juce::Component
{
public:
    TextPill(juce::Colour backgroundColor = juce::Colours::lightgrey);
    
    /** @brief Définit le texte avec deux parties et leurs polices respectives. */
    void setText(const juce::String& firstPart, 
                 const juce::String& secondPart,
                 float fontSize1 = 14.0f,
                 FontManager::FontWeight fontWeight1 = FontManager::FontWeight::Regular,
                 float fontSize2 = 14.0f,
                 FontManager::FontWeight fontWeight2 = FontManager::FontWeight::Regular);
    
    void setTextColor(juce::Colour color);
    void setTextJustification(juce::Justification justification);
    void setBackgroundColor(juce::Colour color);
    void setCornerRadius(float radius);
    void setBorderEnabled(bool enabled);
    void setBorder(juce::Colour borderColor, float thickness);
    void setBorderColour(juce::Colour borderColor);
    void setBorderThickness(float thickness);
    
    bool isBorderEnabled() const;
    juce::Colour getBorderColour() const;
    float getBorderThickness() const;

    void paint(juce::Graphics& g) override;
    
private:
    juce::AttributedString createAttributedText();
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::String text1;
    juce::String text2;
    float font1Size = 14.0f;
    FontManager::FontWeight font1Weight = FontManager::FontWeight::Regular;
    float font2Size = 14.0f;
    FontManager::FontWeight font2Weight = FontManager::FontWeight::Regular;
    
    juce::Colour textColor = juce::Colours::black;
    juce::Colour bgColor = juce::Colours::transparentBlack;
    juce::Colour borderColour = juce::Colours::grey;
    juce::Justification textJustification = juce::Justification::centred;
    float cornerRadius = 4.0f;
    float borderThickness = 1.0f;
    bool useBackground = false;
    bool useBorder = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextPill)
};
