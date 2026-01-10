#include "SimpleToastComponent.h"

SimpleToastComponent::SimpleToastComponent()
{
    setAlwaysOnTop(true);
    setInterceptsMouseClicks(false, false);
    setVisible(false);
}

SimpleToastComponent::~SimpleToastComponent()
{
    stopTimer();
}

void SimpleToastComponent::showMessage(const juce::String& message, int durationMs)
{
    toastMessage = message;
    opacity = 1.0f;
    setVisible(true);
    stopTimer();
    startTimer(durationMs);
    repaint();
}

void SimpleToastComponent::paint(juce::Graphics& g)
{
    if (!isVisible() || toastMessage.isEmpty()) return;
    
    auto bounds = getLocalBounds().reduced(20, 0);
    
    juce::Font font(juce::FontOptions(14.0f));
    int textWidth = juce::GlyphArrangement::getStringWidthInt(font, toastMessage);
    int toastWidth = textWidth + 40;
    
    auto toastBounds = juce::Rectangle<int>(
        bounds.getRight() - toastWidth,
        bounds.getY() + 20,
        toastWidth,
        30
    );
    
    g.setColour(juce::Colour(0xCC333333).withAlpha(opacity));
    g.fillRoundedRectangle(toastBounds.toFloat(), 6.0f);
    
    g.setColour(juce::Colour(0xFF666666).withAlpha(opacity));
    g.drawRoundedRectangle(toastBounds.toFloat(), 6.0f, 1.0f);
    
    g.setColour(juce::Colours::white.withAlpha(opacity));
    g.setFont(font);
    g.drawText(toastMessage, toastBounds, juce::Justification::centred, false);
}

void SimpleToastComponent::resized() {}

void SimpleToastComponent::timerCallback()
{
    stopTimer();
    startFadeOut();
}

void SimpleToastComponent::startFadeOut()
{
    opacity = 0.0f;
    setVisible(false);
    repaint();
}
