/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FirstsynthAudioProcessorEditor  : public juce::AudioProcessorEditor//,
    //private juce::Slider::Listener
{
public:
    FirstsynthAudioProcessorEditor (FirstsynthAudioProcessor&);
    ~FirstsynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    //void sliderValueChanged(juce::Slider* slider) override;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FirstsynthAudioProcessor& audioProcessor;

    juce::Slider gainSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;

    //NOTE: try using vector of pointers if this doesnt work.  why shouldnt it though? these objects will last for the duration of the editor cuz
    //they are member variables, right
    std::vector<std::shared_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirstsynthAudioProcessorEditor)
};
