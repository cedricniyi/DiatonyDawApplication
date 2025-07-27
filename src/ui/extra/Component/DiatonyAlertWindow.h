#pragma once

#include <JuceHeader.h>

//==============================================================================
class DiatonyAlertWindow : public juce::Component
{
public:
    DiatonyAlertWindow(const juce::String& titleText,
                      const juce::String& messageText,
                      const juce::String& buttonText,
                      std::function<void()> onCloseCallback);

    void paint(juce::Graphics& g) override;
    void resized() override;

    static void show(const juce::String& title,
                    const juce::String& message,
                    const juce::String& buttonText = "OK",
                    std::function<void()> onCloseCallback = nullptr);

private:
    juce::String title;
    juce::String message;
    juce::String buttonLabel;
    juce::TextButton okButton;
    std::function<void()> onClose;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiatonyAlertWindow)
};