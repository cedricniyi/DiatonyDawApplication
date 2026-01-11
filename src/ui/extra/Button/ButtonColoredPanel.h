#pragma once

#include <JuceHeader.h>

enum class PanelContentType { Section, Modulation };

/** @brief Panel coloré cliquable avec gestion de sélection et texte centré. */
class ButtonColoredPanel : public juce::Button
{
public:
    ButtonColoredPanel(juce::Colour color);
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void mouseDown(const juce::MouseEvent& e) override;
    
    std::function<void()> onRightClick;
    
    void setSelected(bool selected);
    bool getSelected() const;
    
    void setColor(juce::Colour color);
    juce::Colour getColor() const;
    
    void setUserData(const juce::var& data);
    juce::var getUserData() const;
    
    void setContentType(PanelContentType type);
    PanelContentType getContentType() const;
    
    void setDisplayText(const juce::String& text);
    juce::String getDisplayText() const;
    
    void setShowText(bool shouldShowText);
    bool getShowText() const;

private:
    juce::Colour baseColor;
    bool isSelected;
    juce::var userData;
    PanelContentType contentType;
    juce::String displayText;
    bool showText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonColoredPanel)
}; 