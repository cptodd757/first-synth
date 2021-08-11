/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//New class to streamline the baggage of setting up parameters, their attachments, and their corresponding UI components.
//Current JUCE framework requires a lot of different classes to do all this in different places throughout processor and editor
class AudioParameterWrapper
{
public:
    AudioParameterWrapper();
    AudioParameterWrapper(juce::String paramID, juce::String paramName, float min, float max, float defaultValue, juce::Slider::SliderStyle style);
    ~AudioParameterWrapper();

    std::unique_ptr<juce::RangedAudioParameter> param; //has to be a pointer so we can use an abstract class
    std::shared_ptr<juce::Slider> slider; //slider or button

    void setBounds(int xx, int yy, int ww, int hh);
    void resized();

    juce::String getName();
    juce::String getId();

private:
    int x;
    int y;
    int w;
    int h;

    juce::String name;
    juce::String id;
};

//==============================================================================
/**
*/
class FirstsynthAudioProcessor  : public juce::AudioProcessor
{
public:
    float gainLevel;
    //==============================================================================
    FirstsynthAudioProcessor();
    ~FirstsynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::vector<std::shared_ptr<AudioParameterWrapper>> params;
    std::vector<std::shared_ptr<AudioParameterWrapper>> initParams();

    juce::AudioProcessorValueTreeState vtree;
    juce::AudioProcessorValueTreeState::ParameterLayout createParamLayout();

private:
    //==============================================================================
    juce::Synthesiser synth;
    const int NUM_VOICES = 3;
    juce::dsp::Gain<float> gain;
    bool isPrepared;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirstsynthAudioProcessor)
};

