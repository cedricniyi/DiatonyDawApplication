#pragma once

#include <JuceHeader.h>

//==============================================================================
// LookAndFeel personnalisé pour ComboBox avec design simple et élégant
// Compatible avec le style InfoColoredPanel (coins arrondis, couleurs harmonieuses)
class DiatonyComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DiatonyComboBoxLookAndFeel();
    ~DiatonyComboBoxLookAndFeel() override = default;
    
    // Override des méthodes ComboBox pour personnaliser l'apparence
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH,
                     juce::ComboBox& comboBox) override;
    
    int getComboBoxTextHeight(juce::ComboBox& comboBox);
    
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isSeparator, bool isActive, bool isHighlighted,
                          bool isTicked, bool hasSubMenu, const juce::String& text,
                          const juce::String& shortcutKeyText,
                          const juce::Drawable* icon, const juce::Colour* textColourToUse) override;
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    
    // Couleurs personnalisables
    void setBackgroundColour(juce::Colour colour);
    void setBorderColour(juce::Colour colour);
    void setTextColour(juce::Colour colour);
    void setArrowColour(juce::Colour colour);
    
private:
    // Couleurs du design
    juce::Colour backgroundColour;
    juce::Colour borderColour;
    juce::Colour textColour;
    juce::Colour arrowColour;
    juce::Colour highlightColour;
    
    // Constants de design
    static constexpr float cornerRadius = 6.0f;
    static constexpr int textPadding = 8;
    static constexpr int arrowWidth = 12;
    
    void drawArrow(juce::Graphics& g, juce::Rectangle<int> arrowArea, juce::Colour colour);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyComboBoxLookAndFeel)
};