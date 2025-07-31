#pragma once

#include <JuceHeader.h>

// Forward declarations
class FontManager;

//==============================================================================
class WelcomeView : public juce::Component
{
public:
    WelcomeView();
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::SharedResourcePointer<FontManager> fontManager;
    
    juce::Label welcomeIconLabel;
    juce::Label welcomeTitleLabel;
    juce::Label welcomeDescLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WelcomeView)
};