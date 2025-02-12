//
// Created by Cédric Niyikiza on 07/02/2025.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
// #include <juce_audio_basics/midi/juce_MidiMessage.h>
#include "JuceHeader.h"

class MidiProcessor
{
public:
    MidiBuffer processedMidi;

    void process(MidiBuffer& midiMessages)
    {
        processedMidi.clear();
        for (auto metadata : midiMessages)
        {
            auto message = metadata.getMessage();
            const auto time = metadata.samplePosition;

            // if (message.isNoteOnOrOff())
            // {
            DBG(message.getDescription());
            // }
            // if (message.isNoteOnOrOff())
            // {
            //     // DBG(message.getDescription());
            //    message.setNoteNumber(50);
            // }
            processedMidi.addEvent (message, time);
        }
        midiMessages.swapWith (processedMidi);
    }
};
