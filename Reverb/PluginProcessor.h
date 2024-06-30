/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSPDelayLineTutorial_02.h"

//==============================================================================
/**
*/
class A3AudioProcessor  : public juce::AudioProcessor
{
public:
    juce::dsp::Convolution convolution;
    juce::File loaded;
    juce::AudioBuffer<float> buffer;

    int rate;
    Delay<float, 2>  d1;
    Delay<float, 2>  d2;
    Delay<float, 2>  d3;
    Delay<float, 2>  d4;
    Delay<float, 2>  d5;
    juce::dsp::StateVariableTPTFilter<float> f1;

    //==============================================================================
    A3AudioProcessor();
    ~A3AudioProcessor() override;

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
    
    void updateFX();
    void updateBuffer(int numChannels);
    void updateParameters ();
    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::dsp::StateVariableTPTFilter<float> stateVariableFilter;
    bool bypassFilter = false;
    
    enum { phaserIndex, gainIndex };
    juce::dsp::ProcessorChain<juce::dsp::Phaser<float>,juce::dsp::Gain<float>, juce::dsp::Reverb> fxChain;
    bool bypassPhaser = false;

    bool reverb4on = false;

    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A3AudioProcessor)
};
