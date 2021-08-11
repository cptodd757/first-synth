/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>

//==============================================================================
FirstsynthAudioProcessorEditor::FirstsynthAudioProcessorEditor (FirstsynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 400);

    //gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.vtree, "GAIN", gainSlider);

    for (int i = 0; i < audioProcessor.params.size(); i++)
    {
        auto& param = audioProcessor.params.at(i);
        auto newAttachment = std::make_shared<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.vtree, param->getId(), *param->slider);
        attachments.push_back(newAttachment);
    }

    // //these define the parameters of our slider object
    //gainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    //gainSlider.setRange(0.0, 1.0f, 0.01f);
    //gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    //gainSlider.setPopupDisplayEnabled(true, false, this);
    //gainSlider.setTextValueSuffix(" Gain");
    //gainSlider.setValue(1.0);

    for (int i = 0; i < audioProcessor.params.size(); i++)
    {
        addAndMakeVisible(*audioProcessor.params.at(i)->slider);
    }

    audioProcessor.gainLevel = .5f;
}

FirstsynthAudioProcessorEditor::~FirstsynthAudioProcessorEditor()
{
}

//==============================================================================
void FirstsynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void FirstsynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    for (int i = 0; i < audioProcessor.params.size(); i++)
    {
        audioProcessor.params.at(i)->resized();
    }
}
