#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

class SimpleVoice : public juce::SynthesiserVoice
{
public:
    SimpleVoice()
    {
        // Configure une enveloppe ADSR basique
        adsr.setParameters(
            juce::ADSR::Parameters(
                0.001f,  // attack
                0.6f,   // decay
                0.5012f,   // sustain
                0.6f    // release
        ));

        // Initialisation du filtre
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(44100.0, 11200.0, 0.1));
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;

        level = velocity * 0.15f;  // Réduit à 0.05 au lieu de 0.15
        // level = (velocity * 0.02f) / 4.0f;  
        
        auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        angleDelta = frequency * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
        
        adsr.noteOn();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
            adsr.reset();
        }
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float)(std::sin(currentAngle) * level);
                auto envelope = adsr.getNextSample();
                
                // Applique l'enveloppe
                currentSample *= envelope;

                // Applique le filtre
                currentSample = filter.processSingleSampleRaw(currentSample);

                
                for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;

                if (!adsr.isActive())
                {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;
                }
            }
        }
    }

    void pitchWheelMoved (int /*newValue*/) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override {}

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
    juce::ADSR adsr;
    juce::IIRFilter filter;

};
