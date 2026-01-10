#pragma once

#include <JuceHeader.h>
#include <map>

class DiatonyComboBox;

/** @brief LookAndFeel personnalisé pour ComboBox avec coins arrondis et couleurs harmonieuses. */
class DiatonyComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DiatonyComboBoxLookAndFeel();
    ~DiatonyComboBoxLookAndFeel() override = default;
    
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH,
                     juce::ComboBox& comboBox) override;
    
    int getComboBoxTextHeight(juce::ComboBox& comboBox);
    juce::Label* createComboBoxTextBox(juce::ComboBox& comboBox) override;
    void positionComboBoxText(juce::ComboBox& comboBox, juce::Label& labelToPosition) override;
    
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                          bool isSeparator, bool isActive, bool isHighlighted,
                          bool isTicked, bool hasSubMenu, const juce::String& text,
                          const juce::String& shortcutKeyText,
                          const juce::Drawable* icon, const juce::Colour* textColourToUse) override;
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    
    void setBackgroundColour(juce::Colour colour);
    void setBorderColour(juce::Colour colour);
    void setTextColour(juce::Colour colour);
    void setArrowColour(juce::Colour colour);
    
    void setArrowVisible(bool visible) { showArrow = visible; }
    bool isArrowVisible() const { return showArrow; }
    
    void setShortTextForItem(int itemId, const juce::String& shortText) { shortTexts[itemId] = shortText; }
    void clearShortTexts() { shortTexts.clear(); }
    void setShortDisplayMode(bool enabled) { shortDisplayMode = enabled; }
    juce::String getDisplayText(const juce::ComboBox& comboBox) const;
    
private:
    juce::Colour backgroundColour;
    juce::Colour borderColour;
    juce::Colour textColour;
    juce::Colour arrowColour;
    juce::Colour highlightColour;
    
    static constexpr float cornerRadius = 6.0f;
    static constexpr int textPadding = 8;
    static constexpr int arrowWidth = 12;
    
    bool showArrow = true;
    bool shortDisplayMode = false;
    std::map<int, juce::String> shortTexts;
    
    void drawArrow(juce::Graphics& g, juce::Rectangle<int> arrowArea, juce::Colour colour);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyComboBoxLookAndFeel)
};
