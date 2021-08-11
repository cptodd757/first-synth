/*
  ==============================================================================

    MyVoice.h
    Created: 4 Jan 2021 4:42:13pm
    Author:  Charlie Todd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MySound.h"

class MyVoice : public juce::SynthesiserVoice
{
public:
    MyVoice();
    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels);

    juce::ADSR getAdsr();

    void setAdsrParams(juce::ADSR::Parameters p);
private:
    juce::ADSR adsr;

    juce::dsp::Oscillator<float> osc{ [](float x) { return std::sin(x); } };

    juce::AudioBuffer<float> synthBuffer;

};