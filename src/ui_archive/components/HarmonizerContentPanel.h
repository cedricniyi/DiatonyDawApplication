#pragma once

#include <JuceHeader.h>

//==============================================================================
class HarmonizerContentPanel : public juce::Component
{
public:
    HarmonizerContentPanel()
    {
        setOpaque(true);
    }

    ~HarmonizerContentPanel() override = default;

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF2A2A2A));

        g.setColour(juce::Colours::green.withAlpha(0.8f));
        g.setFont(20.0f);
        g.drawText("Mode Harmonizer", getLocalBounds(),
                   juce::Justification::centred, true);
    }

    void resized() override
    {
        // Pour l'instant, pas besoin de layout particulier
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HarmonizerContentPanel)
}; 