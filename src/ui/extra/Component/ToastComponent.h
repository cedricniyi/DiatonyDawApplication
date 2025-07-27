#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ToastComponent : public juce::Component, 
                       private juce::Timer
{
public:
    ToastComponent();
    ~ToastComponent() override;
    
    void showMessage(const juce::String& message, int durationMs = 2000);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    void timerCallback() override;
    void startFadeOut();
    
    juce::String toastMessage;
    float opacity = 1.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToastComponent)
}; 