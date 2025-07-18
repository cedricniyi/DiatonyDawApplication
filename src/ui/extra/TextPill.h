#pragma once

#include <JuceHeader.h>
#include "../../utils/FontManager.h"

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
    TextPill(juce::Colour backgroundColor = juce::Colours::lightgrey) 
        : bgColor(backgroundColor), useBackground(backgroundColor != juce::Colours::transparentBlack)
    {
        setOpaque(false);
    }
    
    /**
     * Définit le texte avec deux parties et leurs polices respectives
     */
    void setText(const juce::String& firstPart, 
                 const juce::String& secondPart,
                 float fontSize1 = 14.0f,
                 FontManager::FontWeight fontWeight1 = FontManager::FontWeight::Regular,
                 float fontSize2 = 14.0f,
                 FontManager::FontWeight fontWeight2 = FontManager::FontWeight::Regular)
    {
        text1 = firstPart;
        text2 = secondPart;
        font1Size = fontSize1;
        font1Weight = fontWeight1;
        font2Size = fontSize2;
        font2Weight = fontWeight2;
        repaint();
    }
    
    /**
     * Définit la couleur du texte
     */
    void setTextColor(juce::Colour color)
    {
        textColor = color;
        repaint();
    }
    
    /**
     * Définit la justification du texte
     */
    void setTextJustification(juce::Justification justification)
    {
        textJustification = justification;
        repaint();
    }
    
    /**
     * Définit la couleur de fond (optionnelle)
     */
    void setBackgroundColor(juce::Colour color)
    {
        bgColor = color;
        useBackground = (color != juce::Colours::transparentBlack);
        repaint();
    }
    
    /**
     * Définit le rayon des coins arrondis
     */
    void setCornerRadius(float radius)
    {
        cornerRadius = radius;
        repaint();
    }
    
    /**
     * Active/désactive la bordure
     */
    void setBorderEnabled(bool enabled)
    {
        useBorder = enabled;
        repaint();
    }
    
    /**
     * Définit la couleur et l'épaisseur de la bordure
     */
    void setBorder(juce::Colour borderColor, float thickness)
    {
        borderColour = borderColor;
        borderThickness = thickness;
        useBorder = true;
        repaint();
    }
    
    /**
     * Définit seulement la couleur de la bordure (garde l'épaisseur actuelle)
     */
    void setBorderColour(juce::Colour borderColor)
    {
        borderColour = borderColor;
        repaint();
    }
    
    /**
     * Définit seulement l'épaisseur de la bordure (garde la couleur actuelle)
     */
    void setBorderThickness(float thickness)
    {
        borderThickness = thickness;
        repaint();
    }
    
    /**
     * Getters pour les propriétés de bordure
     */
    bool isBorderEnabled() const { return useBorder; }
    juce::Colour getBorderColour() const { return borderColour; }
    float getBorderThickness() const { return borderThickness; }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Dessiner le fond coloré avec coins arrondis si nécessaire
        if (useBackground)
        {
            // Si on a une bordure, réduire légèrement les bounds pour le fond
            auto backgroundBounds = useBorder ? bounds.reduced(borderThickness * 0.5f) : bounds;
            g.setColour(bgColor);
            g.fillRoundedRectangle(backgroundBounds, cornerRadius);
        }
        
        // Dessiner la bordure si activée
        if (useBorder)
        {
            // Réduire les bounds par la moitié de l'épaisseur pour un dessin correct du contour externe
            auto borderBounds = bounds.reduced(borderThickness * 0.5f);
            g.setColour(borderColour);
            g.drawRoundedRectangle(borderBounds, cornerRadius, borderThickness);
        }
        
        // Créer et dessiner le texte avec AttributedString
        auto attributedText = createAttributedText();
        attributedText.setJustification(textJustification);
        attributedText.draw(g, bounds);
    }
    
private:
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
    
    juce::AttributedString createAttributedText()
    {
        juce::AttributedString attributedString;
        
        // Ajouter la première partie avec sa police
        if (!text1.isEmpty())
        {
            attributedString.append(text1, juce::Font(fontManager->getSFProText(font1Size, font1Weight)), textColor);
        }
        
        // Ajouter la deuxième partie avec sa police
        if (!text2.isEmpty())
        {
            attributedString.append(text2, juce::Font(fontManager->getSFProText(font2Size, font2Weight)), textColor);
        }
        
        return attributedString;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextPill)
}; 