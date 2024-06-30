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
class A2StarterAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    A2StarterAudioProcessorEditor (A2StarterAudioProcessor&);
    ~A2StarterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider) override;
    void pingpongValueChanged();
    void resetPressed();
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    A2StarterAudioProcessor& audioProcessor;

    juce::Slider delaySlider; // [1]
    juce::Label delayLabel;
    juce::Slider drySlider; // [1]
    juce::Label dryLabel;
    juce::Slider wetSlider; // [1]
    juce::Label wetLabel;
    juce::Slider feedbackSlider; // [1]
    juce::Label feedbackLabel;
    juce::ComboBox pingpongBox;
    juce::Label pingpongLabel;
    juce::TextButton resetButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A2StarterAudioProcessorEditor)
};
