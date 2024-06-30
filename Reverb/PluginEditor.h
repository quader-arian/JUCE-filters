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
class A3AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    A3AudioProcessorEditor (A3AudioProcessor&);
    ~A3AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    A3AudioProcessor& audioProcessor;

    juce::ComboBox reverbEnable;
    juce::Label reverbEnableLbl;
    juce::Slider roomSize;
    juce::Label roomSizeLbl;
    juce::Slider damping;
    juce::Label dampingLbl;
    juce::Slider wet;
    juce::Label wetLbl;
    juce::Slider dry;
    juce::Label dryLbl;
    juce::Slider width;
    juce::Label widthLbl;
    juce::Slider freeze;
    juce::Label freezeLbl;
    juce::ComboBox impulse;
    juce::Label impulseLbl;
    juce::Slider impulseLen;
    juce::Label impulseLenLbl;
    juce::TextButton load;
    juce::Slider d1;
    juce::Label d1Lbl;
    juce::Slider d2;
    juce::Label d2Lbl;
    juce::Slider d3;
    juce::Label d3Lbl;
    juce::Slider d4;
    juce::Label d4Lbl;
    juce::Slider d5;
    juce::Label d5Lbl;
    juce::Slider f;
    juce::Label fLbl;


    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> reverbEnableValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampingValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freezeValue;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> impulseValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> impulseLenValue;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> d1V;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> d2V;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> d3V;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> d4V;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> d5V;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fV;

    std::unique_ptr<juce::FileChooser> fileChoice;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (A3AudioProcessorEditor)
};
