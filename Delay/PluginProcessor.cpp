/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A2StarterAudioProcessor::A2StarterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

A2StarterAudioProcessor::~A2StarterAudioProcessor()
{
}

//==============================================================================
const juce::String A2StarterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A2StarterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool A2StarterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool A2StarterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double A2StarterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A2StarterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int A2StarterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void A2StarterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String A2StarterAudioProcessor::getProgramName (int index)
{
    return {};
}

void A2StarterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void A2StarterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rate = static_cast<float> (sampleRate);
    
    // Need to change this value to a number that corresponds to 3 seconds
    delayBufferLength = 3.1 * rate;
    timeLimit = 3.0;
    time = 0;
    wet = 1.0;
    dry = 1.0;
    feedback = 0.0;
    pingpong = false;
    pingpongSwitch = false;

    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();

}

void A2StarterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A2StarterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void A2StarterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* data = buffer.getWritePointer (channel,0);
        int numSamples = buffer.getNumSamples();
        
        // Get a pointer to write in the delayBuffer
        float* delayData = delayBuffer.getWritePointer(channel);
        int delayNumSamples = delayBuffer.getNumSamples();

        // Write something to the delayBuffer
        if (delayNumSamples > numSamples + time) { 
            for (int i = 0; i < numSamples; ++i) {
                delayData[i + time] = data[i];
                data[i] = data[i] * dry;
            }
        }else {
            int timeLeft = delayNumSamples - time;
            for (int i = 0; i < timeLeft; ++i) {
                delayData[i + time] = data[i];
                data[i] = data[i] * dry;
            }
            int timeWrapped = numSamples - timeLeft;
            for (int i = 0; i < timeWrapped; ++i) {
                delayData[i] = data[i + timeLeft];
                data[i + timeLeft] = data[i] * dry;
            }
        }

        // Must also write to the output buffer
        int play = time - timeLimit*rate;
        if (play < 0) {
            play += delayNumSamples;
            if (pingpong) pingpongSwitch = true; 
        }else {
            if (pingpong) pingpongSwitch = false;
        }
        bool pingpongCheck = (pingpong && ((pingpongSwitch && channel % totalNumInputChannels == 0) || (!pingpongSwitch && channel % totalNumInputChannels != 0))) || !pingpong;

        if (play + numSamples < delayNumSamples) {
            for (int i = 0; i < numSamples; ++i) {
                data[i] += delayData[i + play] * wet;
            }
        }else {
            int timeLeft = delayNumSamples - play;
            for (int i = 0; i < timeLeft; ++i) {
                data[i] += delayData[i + play] * wet;
            }
            int timeWrapped = numSamples - timeLeft;
            for (int i = 0; i < timeWrapped; ++i) {
                data[i + timeWrapped] += delayData[i] * wet;
            }
        }

        // Write output to the delayBuffer
        if (delayNumSamples > numSamples + time) {
            for (int i = 0; i < numSamples; ++i) {
                delayData[i + time] += data[i] * feedback;
            }
        }else {
            int timeLeft = delayNumSamples - time;
            for (int i = 0; i < timeLeft; ++i) {
                delayData[i + time] += data[i] * feedback;
            }
            int timeWrapped = numSamples - timeLeft;
            for (int i = 0; i < timeWrapped; ++i) {
                delayData[i] += data[i + timeLeft] * feedback;
            }
        }

        // remove channel for pingpong
        if (!pingpongCheck) {
            if (play + numSamples < delayNumSamples) {
                for (int i = 0; i < numSamples; ++i) {
                    data[i] -= delayData[i + play] * wet;
                }
            }
            else {
                int timeLeft = delayNumSamples - play;
                for (int i = 0; i < timeLeft; ++i) {
                    data[i] -= delayData[i + play] * wet;
                }
                int timeWrapped = numSamples - timeLeft;
                for (int i = 0; i < timeWrapped; ++i) {
                    data[i + timeWrapped] -= delayData[i] * wet;
                }
            }
        }
    }
    time = (time + buffer.getNumSamples()) % delayBuffer.getNumSamples();
}

//==============================================================================
bool A2StarterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* A2StarterAudioProcessor::createEditor()
{
    return new A2StarterAudioProcessorEditor (*this);
}

//==============================================================================
void A2StarterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void A2StarterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A2StarterAudioProcessor();
}
