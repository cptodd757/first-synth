/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MyVoice.h"
#include "MySound.h"

//==============================================================================
FirstsynthAudioProcessor::FirstsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), params (initParams())
    , vtree (*this, nullptr, "Vtree", createParamLayout())
#endif
{
    synth.addSound(new MySound());
    for (int i = 0; i < NUM_VOICES; i++)
    {
        synth.addVoice(new MyVoice());
    }
}

FirstsynthAudioProcessor::~FirstsynthAudioProcessor()
{
}

//==============================================================================
const juce::String FirstsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FirstsynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FirstsynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FirstsynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FirstsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FirstsynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FirstsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FirstsynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FirstsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void FirstsynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FirstsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto* voice = dynamic_cast<MyVoice*> (synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getNumOutputChannels();

    gain.prepare(spec);
    isPrepared = true;
}

void FirstsynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FirstsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FirstsynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert(isPrepared == true);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = 0; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<MyVoice*>(synth.getVoice(i)))
        {
            // Osc controls
            // ADSR
            // LFO
            auto attack = vtree.getRawParameterValue("ATTACK");
            auto decay = vtree.getRawParameterValue("DECAY");
            auto sustain = vtree.getRawParameterValue("SUSTAIN");
            auto release = vtree.getRawParameterValue("RELEASE");
            auto adsrParams = juce::ADSR::Parameters();
            adsrParams.attack = attack->load();
            adsrParams.decay = decay->load();
            adsrParams.sustain = sustain->load();
            adsrParams.release = release->load();
            voice->setAdsrParams(adsrParams);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    gain.setGainLinear(*vtree.getRawParameterValue("GAIN"));
    gain.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float> { buffer }));
}

//==============================================================================
bool FirstsynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FirstsynthAudioProcessor::createEditor()
{
    return new FirstsynthAudioProcessorEditor (*this);
}

//==============================================================================
void FirstsynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FirstsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FirstsynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout FirstsynthAudioProcessor::createParamLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> vec;
    for (int i = 0; i < params.size(); i++)
    {
        vec.push_back(std::move(params.at(i)->param));
    }
    return { vec.begin(), vec.end() };

    //std::vector<std::unique_ptr<juce::AudioParameterFloat>> vec;
    //vec.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, .5f));
    ///*auto temp = std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, .5f);
    //vec.push_back(temp);*/
    //return { vec.begin(), vec.end() };  
}

std::vector<std::shared_ptr<AudioParameterWrapper>> FirstsynthAudioProcessor::initParams()
{
    auto vertical = juce::Slider::SliderStyle::LinearBarVertical;
    auto gainParam = std::make_shared<AudioParameterWrapper>("GAIN", "Gain", 0.0f, 1.0f, .2f, vertical);
    auto attackParam = std::make_shared<AudioParameterWrapper>("ATTACK", "Attack", 0.0f, 1.0f, .2f, vertical);
    auto decayParam = std::make_shared<AudioParameterWrapper>("DECAY", "Decay", 0.0f, 1.0f, .2f, vertical);
    auto sustainParam = std::make_shared<AudioParameterWrapper>("SUSTAIN", "Sustain", 0.0f, 1.0f, .2f, vertical);
    auto releaseParam = std::make_shared<AudioParameterWrapper>("RELEASE", "Release", 0.0f, 1.0f, .2f, vertical);

    std::vector<std::shared_ptr<AudioParameterWrapper>> tempParams;
    tempParams.push_back(gainParam);
    tempParams.push_back(std::move(attackParam));
    tempParams.push_back(std::move(decayParam));
    tempParams.push_back(std::move(sustainParam));
    tempParams.push_back(std::move(releaseParam));

    for (int i = 0; i < tempParams.size(); i++)
    {
        tempParams.at(i)->setBounds(i * 60, 100, 30, 90);
    }
    return tempParams;
}


AudioParameterWrapper::AudioParameterWrapper()
{

}
//float type constructor
AudioParameterWrapper::AudioParameterWrapper(juce::String paramID, juce::String paramName, float min, float max, float defaultValue, juce::Slider::SliderStyle style)
{
    name = paramName;
    id = paramID;
    param = std::make_unique<juce::AudioParameterFloat>(paramID, paramName, min, max, defaultValue);
    slider = std::make_shared<juce::Slider>(style, juce::Slider::TextEntryBoxPosition::TextBoxBelow);
    slider->setRange(min, max, (max - min) / 100);
}

AudioParameterWrapper::~AudioParameterWrapper()
{
    
}

void AudioParameterWrapper::setBounds(int xx, int yy, int ww, int hh)
{
    x = xx;
    y = yy;
    w = ww;
    h = hh;
}

void AudioParameterWrapper::resized()
{
    slider->setBounds(x, y, w, h);
}

juce::String AudioParameterWrapper::getName()
{
    return name;
}

juce::String AudioParameterWrapper::getId()
{
    return id;
}






