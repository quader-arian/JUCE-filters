/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A3AudioProcessor::A3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

A3AudioProcessor::~A3AudioProcessor()
{
}

//==============================================================================
const juce::String A3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool A3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool A3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double A3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int A3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void A3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String A3AudioProcessor::getProgramName (int index)
{
    return {};
}

void A3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void A3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    rate = static_cast<float>(sampleRate);
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();
    
    stateVariableFilter.reset();
    fxChain.reset();
    stateVariableFilter.prepare(spec);
    fxChain.prepare(spec);

    reverb.reset();
    reverb.prepare(spec);

    convolution.reset();
    convolution.prepare(spec);

    buffer.setSize(2, 6.0 * rate);
    buffer.clear();

    d1.reset();
    d2.reset();
    d3.reset();
    d4.reset();
    d5.reset();
    f1.reset();
    d1.prepare(spec);
    d2.prepare(spec);
    d3.prepare(spec);
    d4.prepare(spec);
    d5.prepare(spec);
    f1.prepare(spec);
    d1.setWetLevel(0.9f);
    d2.setWetLevel(0.7f);
    d3.setWetLevel(0.5f);
    d4.setWetLevel(0.3f);
    d5.setWetLevel(0.1f);

    updateFX();
}

void A3AudioProcessor::updateFX()
{
    int reverbEnable = *apvts.getRawParameterValue("REVERBENABLE");
    float roomSize = *apvts.getRawParameterValue("ROOMSIZE");
    float damping = *apvts.getRawParameterValue("DAMPING");
    float wet = *apvts.getRawParameterValue("WET");
    float dry = *apvts.getRawParameterValue("DRY");
    float width = *apvts.getRawParameterValue("WIDTH");
    float freeze = *apvts.getRawParameterValue("FREEZE");

    float time1 = *apvts.getRawParameterValue("DELAY1");
    float time2 = *apvts.getRawParameterValue("DELAY2");
    float time3 = *apvts.getRawParameterValue("DELAY3");
    float time4 = *apvts.getRawParameterValue("DELAY4");
    float time5 = *apvts.getRawParameterValue("DELAY5");
    float cutoff = *apvts.getRawParameterValue("CUTOFF");


    if (reverbEnable != 2) reverb.setEnabled(false);
    if (reverbEnable == 2) reverb.setEnabled(true);
    reverbParams.roomSize = roomSize;
    reverbParams.damping = damping;
    reverbParams.wetLevel = wet;
    reverbParams.dryLevel = dry;
    reverbParams.freezeMode = freeze;
    reverbParams.width = width;
    reverb.setParameters(reverbParams);

    if (reverbEnable == 5) {
        f1.setCutoffFrequency(cutoff);
        f1.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        d1.setDelayTime(0, time1);
        d1.setDelayTime(1, time1);
        d2.setDelayTime(0, time2);
        d2.setDelayTime(1, time2);
        d3.setDelayTime(0, time3);
        d3.setDelayTime(1, time3);
        d4.setDelayTime(0, time4);
        d4.setDelayTime(1, time4);
        d5.setDelayTime(0, time5);
        d5.setDelayTime(1, time5);
        reverb4on = true;
    }
    else {
        reverb4on = false;
    }
}

void A3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void A3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateFX();
    juce::dsp::AudioBlock<float> block (buffer);
    
    if (reverb4on) {
        juce::dsp::ProcessContextNonReplacing<float> context(block, block);
        f1.process(context);
        d1.process(context);
        d2.process(context);
        d3.process(context);
        d4.process(context);
        d5.process(context);
    }
    else {
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverb.process(context);
        convolution.process(context);
    }
}

//==============================================================================
bool A3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* A3AudioProcessor::createEditor()
{
    return new A3AudioProcessorEditor (*this);
}

//==============================================================================
void A3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void A3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A3AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout A3AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterInt> ("REVERBENABLE", "Enable Reverb", 1, 5, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ROOMSIZE", "Room Size", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DAMPING", "Damping", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WET", "Wet Level", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DRY", "Dry Level", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WIDTH", "Width", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FREEZE", "Freeze Mode", 0.0f, 1.0f, 0.0f));

    //layout.add(std::make_unique<juce::AudioParameterInt>("IMPULSE", "Impulse Response Types", 1, 11, 1));
    //layout.add(std::make_unique<juce::AudioParameterFloat>("IMPULSELEN", "Constructed Impulse Response Length", 0.0f, 6.0f, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("DELAY1", "Delay Time 1", 0.0f, 1.0f, 0.075f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DELAY2", "Delay Time 2", 0.0f, 1.0f, 0.075f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DELAY3", "Delay Time 3", 0.0f, 1.0f, 0.075f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DELAY4", "Delay Time 3", 0.0f, 1.0f, 0.075f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("DELAY5", "Delay Time 3", 0.0f, 1.0f, 0.075f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CUTOFF", "Cutoff Frequency", 20.0f, 20000.0f, 1000.0f));
    return layout;
}
