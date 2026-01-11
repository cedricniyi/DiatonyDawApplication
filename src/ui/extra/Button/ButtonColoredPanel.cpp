#include "ButtonColoredPanel.h"

ButtonColoredPanel::ButtonColoredPanel(juce::Colour color) 
    : juce::Button(""),
      baseColor(color),
      isSelected(false),
      contentType(PanelContentType::Section),
      displayText(""),
      showText(true)
{
    setButtonText("");
}

void ButtonColoredPanel::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        if (onRightClick)
            onRightClick();
        return;
    }
    
    juce::Button::mouseDown(e);
}

void ButtonColoredPanel::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool)
{
    auto bounds = getLocalBounds().toFloat();
    
    juce::Colour currentColor = isSelected ? juce::Colours::red /*Sélectionné*/: baseColor/*Normal*/;
    if (shouldDrawButtonAsHighlighted)
        currentColor = currentColor.brighter(0.15f);
    
    g.setColour(currentColor);
    g.fillRect(bounds);
    
    if (showText && displayText.isNotEmpty())
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(12.0f));
        g.drawText(displayText, bounds.toNearestInt(), juce::Justification::centred, true);
    }
}

void ButtonColoredPanel::setSelected(bool selected)
{
    if (isSelected != selected) 
    {
        isSelected = selected;
        repaint();
    }
}

bool ButtonColoredPanel::getSelected() const { return isSelected; }

void ButtonColoredPanel::setColor(juce::Colour color)
{
    baseColor = color;
    repaint();
}

juce::Colour ButtonColoredPanel::getColor() const { return baseColor; }
void ButtonColoredPanel::setUserData(const juce::var& data) { userData = data; }
juce::var ButtonColoredPanel::getUserData() const { return userData; }
void ButtonColoredPanel::setContentType(PanelContentType type) { contentType = type; }
PanelContentType ButtonColoredPanel::getContentType() const { return contentType; }

void ButtonColoredPanel::setDisplayText(const juce::String& text)
{
    if (displayText != text)
    {
        displayText = text;
        repaint();
    }
}

juce::String ButtonColoredPanel::getDisplayText() const { return displayText; }

void ButtonColoredPanel::setShowText(bool shouldShowText)
{
    if (showText != shouldShowText)
    {
        showText = shouldShowText;
        repaint();
    }
}

bool ButtonColoredPanel::getShowText() const { return showText; } 