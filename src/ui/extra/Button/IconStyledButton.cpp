#include "IconStyledButton.h"

IconStyledButton::IconStyledButton(const juce::String& buttonName,
                                   const char* svgData,
                                   size_t svgDataSize,
                                   juce::Colour normalColor,
                                   juce::Colour highlightColor,
                                   juce::Colour iconColor)
    : juce::Button(buttonName),
      backgroundNormal(normalColor),
      backgroundHighlight(highlightColor),
      iconColour(iconColor)
{
    originalIcon = loadSvgIcon(svgData, svgDataSize);
    updateIconColours();
}

void IconStyledButton::setIconColour(juce::Colour newColour)
{
    iconColour = newColour;
    updateIconColours();
    repaint();
}

void IconStyledButton::setBackgroundColours(juce::Colour normalColor, juce::Colour highlightColor)
{
    backgroundNormal = normalColor;
    backgroundHighlight = highlightColor;
    repaint();
}

void IconStyledButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat();
    
    juce::Colour backgroundColour = backgroundNormal;
    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
        backgroundColour = backgroundHighlight;
    
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    juce::Drawable* iconToDraw = normalIcon.get();
    
    if (shouldDrawButtonAsDown && downIcon != nullptr)
        iconToDraw = downIcon.get();
    else if (shouldDrawButtonAsHighlighted && hoverIcon != nullptr)
        iconToDraw = hoverIcon.get();
    
    if (iconToDraw != nullptr)
    {
        auto iconBounds = bounds.reduced(bounds.getHeight() * 0.25f);
        iconToDraw->drawWithin(g, iconBounds, juce::RectanglePlacement::centred, 1.0f);
    }
}

std::unique_ptr<juce::Drawable> IconStyledButton::loadSvgIcon(const char* data, size_t dataSize)
{
    auto svgString = juce::String::fromUTF8(data, static_cast<int>(dataSize));
    auto svgXml = juce::XmlDocument::parse(svgString);
    
    if (svgXml == nullptr)
        return nullptr;
    
    return juce::Drawable::createFromSVG(*svgXml);
}

void IconStyledButton::updateIconColours()
{
    if (originalIcon == nullptr)
        return;
    
    normalIcon = originalIcon->createCopy();
    normalIcon->replaceColour(juce::Colours::black, iconColour);
    
    hoverIcon = originalIcon->createCopy();
    hoverIcon->replaceColour(juce::Colours::black, iconColour.brighter(0.2f));
    
    downIcon = originalIcon->createCopy();
    downIcon->replaceColour(juce::Colours::black, iconColour.darker(0.2f));
}
