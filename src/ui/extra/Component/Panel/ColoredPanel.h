#pragma once

#include <JuceHeader.h>

/** @brief Panneau coloré avec coins arrondis et contour optionnel. */
class ColoredPanel : public juce::Component
{
public:
    ColoredPanel(juce::Colour color = juce::Colours::transparentBlack);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setAlpha(float alpha);
    virtual void setColor(juce::Colour color);
    juce::Colour getColor() const;
    
    void setCornerRadius(float radius);
    float getCornerRadius() const;
    
    void setBorderColor(juce::Colour color);
    void setBorderThickness(float thickness);
    void enableBorder(bool shouldShowBorder);
    
    void setInternalPadding(int padding);
    int getInternalPadding() const;
    
protected:
    juce::Rectangle<int> getDrawingBounds() const;
    
private:
    juce::Colour panelColor;
    juce::Colour borderColor { juce::Colours::grey.withAlpha(0.3f) };
    float cornerRadius { 10.0f };
    float borderThickness { 1.0f };
    int internalPadding { 0 };
    bool showBorder { false };
};
