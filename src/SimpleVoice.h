#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

class SimpleVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;
        auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        angleDelta = frequency * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int /*newValue*/) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);
                    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                        outputBuffer.addSample(channel, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                    tailOff *= 0.99;
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level);
                    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                        outputBuffer.addSample(channel, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};
