/*
  ==============================================================================

    MyVoice.cpp
    Created: 4 Jan 2021 4:41:57pm
    Author:  Charlie Todd

  ==============================================================================
*/

#include "MyVoice.h"
#include "MySound.h"
#include <JuceHeader.h>

MyVoice::MyVoice()
{
}

bool MyVoice::canPlaySound(juce::SynthesiserSound*)
{
    return true;
}

void MyVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    osc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber + 1), true); //setting force to True was necessary to get rid of the slide
    adsr.noteOn();
}

void MyVoice::stopNote(float velocity, bool allowTailOff)
{
    adsr.noteOff();
    if (!allowTailOff || !adsr.isActive())
    {
        clearCurrentNote();
    }
}

void MyVoice::pitchWheelMoved(int newPitchWheelValue)
{

}

void MyVoice::controllerMoved(int controllerNumber, int newControllerValue)
{

}

void MyVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numOutputChannels)
{
    adsr.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = numOutputChannels;

    osc.prepare(spec);
}

void MyVoice::renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive())
        return;

    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();
    juce::dsp::AudioBlock<float> audioBlock { synthBuffer };
    
    osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());

    for (int i = 0; i < outputBuffer.getNumChannels(); i++)
    {
        //We have created a new synthBuffer to contain the newly created audio data from the keyboard.  We then 
        //add this signal to the existing one that we are given (outputBuffer), starting at the startSample given for the existing buffer,
        //reading from 0 to the end of the newly created synth buffer.  AKA copy the whole new sound into the old signal starting from the spot where the new signal 
        //was created
        outputBuffer.addFrom(i, startSample, synthBuffer, i, 0.0f, numSamples);

        if (!adsr.isActive())
        {
            clearCurrentNote();
        }
    }
}

juce::ADSR MyVoice::getAdsr()
{
    return adsr;
}

void MyVoice::setAdsrParams(juce::ADSR::Parameters p)
{
    adsr.setParameters(p);
}
