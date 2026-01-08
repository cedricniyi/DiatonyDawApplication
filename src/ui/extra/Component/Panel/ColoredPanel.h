#pragma once

#include <JuceHeader.h>

//==============================================================================
// Classe pour les panneaux colorés avec coins arrondis et contour optionnel
class ColoredPanel : public juce::Component
{
public:
    // Constructeur avec couleur uniquement (comportement par défaut)
    ColoredPanel(juce::Colour color = juce::Colours::transparentBlack);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // === Configuration de base ===
    void setAlpha(float alpha);
    virtual void setColor(juce::Colour color);
    juce::Colour getColor() const;
    
    // === Configuration des coins arrondis ===
    void setCornerRadius(float radius);
    float getCornerRadius() const;
    
    // === Configuration du contour ===
    void setBorderColor(juce::Colour color);
    void setBorderThickness(float thickness);
    void enableBorder(bool shouldShowBorder);
    
    // === Configuration du padding interne (affecte le dessin) ===
    void setInternalPadding(int padding);
    int getInternalPadding() const;
    
protected:
    // Zone de dessin interne (après padding)
    juce::Rectangle<int> getDrawingBounds() const;
    
private:
    juce::Colour panelColor;
    juce::Colour borderColor { juce::Colours::grey.withAlpha(0.3f) };
    float cornerRadius { 10.0f };
    float borderThickness { 1.0f };
    int internalPadding { 0 };
    bool showBorder { false };
};
