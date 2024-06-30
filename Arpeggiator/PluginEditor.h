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
class A1StarterAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    A1StarterAudioProcessorEditor (A1StarterAudioProcessor&);
    ~A1StarterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider) override;
    void orderMenuChanged();
    void octaveMenuChanged();
    void repeatMenuChanged();
    void resetPressed();
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    A1StarterAudioProcessor& audioProcessor;

    juce::Label arpLabel;
    juce::Slider arpSlider; // [1]
    juce::Label orderLabel;
    juce::ComboBox orderMenu;
    juce::Label octaveLabel;
    juce::ComboBox octaveMenu;
    juce::Label repeatLabel;
    juce::ComboBox repeatMenu;
    juce::Label speed0Label;
    juce::Slider speed0;
    juce::Label speed1Label;
    juce::Slider speed1;
    juce::Label speed2Label;
    juce::Slider speed2;
    juce::Label speed3Label;
    juce::Slider speed3;
    juce::Label speed4Label;
    juce::Slider speed4;
    juce::Label speed5Label;
    juce::Slider speed5;
    juce::Label speed6Label;
    juce::Slider speed6;
    juce::Label speed7Label;
    juce::Slider speed7;
    juce::Label speed8Label;
    juce::Slider speed8;
    juce::Label speed9Label;
    juce::Slider speed9;
    juce::TextButton resetButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A1StarterAudioProcessorEditor)
};
