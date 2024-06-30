/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class A1StarterAudioProcessor  : public juce::AudioProcessor
{
public:
     float arpSpeed;
     int order;
     int octave;
     int repeat;
     juce::Array<float> durations;
    //==============================================================================
    A1StarterAudioProcessor();
    ~A1StarterAudioProcessor() override;

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

private:
    //==============================================================================
    int currentNote, lastNoteValue;
    int time;
    float rate;
    int order2;
    juce::Random rnd;
    juce::Array<int> randomNotes;
    int rCount;
    int back, front;
    int order3;
    int repeatSwitch;
    int currentOctave;
    int firstTime;
    juce::SortedSet<int> notes;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A1StarterAudioProcessor)
};
