#pragma once

#include <JuceHeader.h>
#include "ColoredPanel.h"

//==============================================================================
/**
 * Panel coloré cliquable avec callback et gestion hover
 */
class ClickableColoredPanel : public ColoredPanel
{
public:
    // Type de callback pour les clics
    using ClickCallback = std::function<void(ClickableColoredPanel*)>;
    
    ClickableColoredPanel(juce::Colour color, ClickCallback clickCallback = nullptr);
    
    void paint(juce::Graphics& g) override;
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    // Setters pour les callbacks et données
    void setClickCallback(ClickCallback callback);
    void setUserData(const juce::var& data);
    juce::var getUserData() const;
    
    void setColor(juce::Colour color) override;
    
    // Gestion de la sélection
    void setSelected(bool selected);
    bool getSelected() const;

private:
    juce::Colour originalColor;
    ClickCallback onClickCallback;
    bool isHovered;
    bool isSelected;
    juce::var userData; // Pour stocker des données associées (ex: ID de progression)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClickableColoredPanel)
}; 