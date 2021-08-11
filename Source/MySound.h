/*
  ==============================================================================

    MySound.h
    Created: 4 Jan 2021 4:41:35pm
    Author:  Charlie Todd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MySound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};