#pragma once
#include <JuceHeader.h>
#include <memory>

/** @brief Button avec icône SVG et couleurs de fond personnalisées. */
class IconStyledButton : public juce::Button
{
public:
    IconStyledButton(const juce::String& buttonName,
                     const char* svgData,
                     size_t svgDataSize,
                     juce::Colour normalColor,
                     juce::Colour highlightColor,
                     juce::Colour iconColor = juce::Colours::white);
    
    ~IconStyledButton() override = default;
    
    void setIconColour(juce::Colour newColour);
    void setBackgroundColours(juce::Colour normalColor, juce::Colour highlightColor);

protected:
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    std::unique_ptr<juce::Drawable> loadSvgIcon(const char* data, size_t dataSize);
    void updateIconColours();
    
    std::unique_ptr<juce::Drawable> originalIcon;
    std::unique_ptr<juce::Drawable> normalIcon;
    std::unique_ptr<juce::Drawable> hoverIcon;
    std::unique_ptr<juce::Drawable> downIcon;
    
    juce::Colour backgroundNormal;
    juce::Colour backgroundHighlight;
    juce::Colour iconColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconStyledButton)
};
