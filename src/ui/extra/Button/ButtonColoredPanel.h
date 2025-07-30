#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Types de contenu pour les panels
 */
enum class PanelContentType
{
    Section,
    Modulation
};

/**
 * Panel coloré cliquable basé sur juce::Button
 * Ressemble visuellement à ColoredPanel mais avec onClick natif
 */
class ButtonColoredPanel : public juce::Button
{
public:
    ButtonColoredPanel(juce::Colour color);
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    // Gestion de la sélection
    void setSelected(bool selected);
    bool getSelected() const;
    
    void setColor(juce::Colour color);
    juce::Colour getColor() const;
    
    void setUserData(const juce::var& data);
    juce::var getUserData() const;
    
    // Gestion du type de contenu
    void setContentType(PanelContentType type);
    PanelContentType getContentType() const;

private:
    juce::Colour baseColor;
    bool isSelected;
    juce::var userData;
    PanelContentType contentType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonColoredPanel)
}; 