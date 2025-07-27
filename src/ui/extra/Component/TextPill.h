#pragma once

#include <JuceHeader.h>
#include "../../../utils/FontManager.h"

//==============================================================================
/**
 * Composant réutilisable pour afficher du texte avec deux parties ayant des polices différentes
 * Utilise AttributedString pour la concaténation avec styles variés
 */
class TextPill : public juce::Component
{
public:
    /**
     * Constructeur avec couleur de fond et coins arrondis
     */
    TextPill(juce::Colour backgroundColor = juce::Colours::lightgrey);
    
    /**
     * Définit le texte avec deux parties et leurs polices respectives
     */
    void setText(const juce::String& firstPart, 
                 const juce::String& secondPart,
                 float fontSize1 = 14.0f,
                 FontManager::FontWeight fontWeight1 = FontManager::FontWeight::Regular,
                 float fontSize2 = 14.0f,
                 FontManager::FontWeight fontWeight2 = FontManager::FontWeight::Regular);
    
    /**
     * Définit la couleur du texte
     */
    void setTextColor(juce::Colour color);
    
    /**
     * Définit la justification du texte
     */
    void setTextJustification(juce::Justification justification);
    
    /**
     * Définit la couleur de fond (optionnelle)
     */
    void setBackgroundColor(juce::Colour color);
    
    /**
     * Définit le rayon des coins arrondis
     */
    void setCornerRadius(float radius);
    
    /**
     * Active/désactive la bordure
     */
    void setBorderEnabled(bool enabled);
    
    /**
     * Définit la couleur et l'épaisseur de la bordure
     */
    void setBorder(juce::Colour borderColor, float thickness);
    
    /**
     * Définit seulement la couleur de la bordure (garde l'épaisseur actuelle)
     */
    void setBorderColour(juce::Colour borderColor);
    
    /**
     * Définit seulement l'épaisseur de la bordure (garde la couleur actuelle)
     */
    void setBorderThickness(float thickness);
    
    /**
     * Getters pour les propriétés de bordure
     */
    bool isBorderEnabled() const;
    juce::Colour getBorderColour() const;
    float getBorderThickness() const;

    void paint(juce::Graphics& g) override;
    
private:
    juce::AttributedString createAttributedText();
    
    juce::SharedResourcePointer<FontManager> fontManager;
    
    // Propriétés du texte
    juce::String text1;
    juce::String text2;
    float font1Size = 14.0f;
    FontManager::FontWeight font1Weight = FontManager::FontWeight::Regular;
    float font2Size = 14.0f;
    FontManager::FontWeight font2Weight = FontManager::FontWeight::Regular;
    
    // Propriétés visuelles
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