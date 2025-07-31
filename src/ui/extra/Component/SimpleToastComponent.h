#pragma once

#include <JuceHeader.h>

class SimpleToastComponent : public juce::Component, 
                            private juce::Timer
{
public:
    SimpleToastComponent();
    ~SimpleToastComponent() override;
    
    void showMessage(const juce::String& message, int durationMs = 2000);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    void timerCallback() override;
    void startFadeOut();
    
    juce::String toastMessage;
    float opacity = 1.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleToastComponent)
}; 