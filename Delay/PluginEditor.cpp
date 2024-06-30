/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A2StarterAudioProcessorEditor::A2StarterAudioProcessorEditor (A2StarterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 335);

    // these define the parameters of our slider object
    delaySlider.setRange(0.0, 3.0, 0.05);
    delaySlider.setTextValueSuffix(" Sec");
    delaySlider.setValue(1.5);
    // this function adds the slider to the editor
    addAndMakeVisible(&delaySlider);
    delaySlider.addListener(this);
    // function label
    addAndMakeVisible(delayLabel);
    delayLabel.setText("Time Interval", juce::dontSendNotification);
    delayLabel.attachToComponent(&delaySlider, false); // [4]

    // these define the parameters of our slider object
    drySlider.setRange(0.0, 100.0, 1);
    drySlider.setTextValueSuffix("%");
    drySlider.setValue(100.0);
    // this function adds the slider to the editor
    addAndMakeVisible(&drySlider);
    drySlider.addListener(this);
    // function label
    addAndMakeVisible(dryLabel);
    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.attachToComponent(&drySlider, false); // [4]

    // these define the parameters of our slider object
    wetSlider.setRange(0.0, 100.0, 1);
    wetSlider.setTextValueSuffix("%");
    wetSlider.setValue(100.0);
    // this function adds the slider to the editor
    addAndMakeVisible(&wetSlider);
    wetSlider.addListener(this);
    // function label
    addAndMakeVisible(wetLabel);
    wetLabel.setText("Wet", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, false); // [4]

    // these define the parameters of our slider object
    feedbackSlider.setRange(0.0, 100.0, 1);
    feedbackSlider.setTextValueSuffix("%");
    feedbackSlider.setValue(0.0);
    // this function adds the slider to the editor
    addAndMakeVisible(&feedbackSlider);
    feedbackSlider.addListener(this);
    // function label
    addAndMakeVisible(feedbackLabel);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false); // [4]

    // add items to the combo-box
    addAndMakeVisible(pingpongBox);
    pingpongBox.addItem("Disabled", 1);
    pingpongBox.addItem("Enabled", 2);
    pingpongBox.onChange = [this] { pingpongValueChanged(); };
    pingpongBox.setSelectedId(1);
    addAndMakeVisible(pingpongLabel);
    pingpongLabel.setText("Ping Pong", juce::dontSendNotification);
    pingpongLabel.attachToComponent(&pingpongBox, false); // [4]

    addAndMakeVisible(resetButton);
    resetButton.setButtonText("Reset");
    resetButton.onClick = [this] { resetPressed(); };
}

A2StarterAudioProcessorEditor::~A2StarterAudioProcessorEditor()
{
}

//==============================================================================
void A2StarterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // fill the whole window white
    g.fillAll(juce::Colours::black);

    // set the current drawing colour to black
    g.setColour(juce::Colours::white);

    // set the font size and draw text to the screen
    g.setFont(30.0f);
    g.drawFittedText("Digital Delay", 10, 10, getWidth() - 10, 30, juce::Justification::centred, 1);
}

void A2StarterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // sets the position and size of the slider with arguments (x, y, width, height)
    auto sliderLeft = 10;
    auto sliderWidth = getWidth() - sliderLeft * 2;
    auto sliderTop = 70;
    auto sliderHeight = 20;
    auto space = 45;
    delaySlider.setBounds(sliderLeft, sliderTop + space, sliderWidth, sliderHeight);
    drySlider.setBounds(sliderLeft, sliderTop + 2 * space, sliderWidth, sliderHeight);
    wetSlider.setBounds(sliderLeft, sliderTop + 3 * space, sliderWidth, sliderHeight);
    feedbackSlider.setBounds(sliderLeft, sliderTop + 4 * space, sliderWidth, sliderHeight);
    pingpongBox.setBounds(sliderLeft, sliderTop, sliderWidth, sliderHeight);
    resetButton.setBounds((getWidth()/3)*2 - sliderLeft, sliderTop + 5 * space, getWidth() / 3, sliderHeight*1.25);
}

void A2StarterAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &delaySlider) audioProcessor.timeLimit = static_cast<float>(delaySlider.getValue());
    else if (slider == &drySlider) audioProcessor.dry = static_cast<float>(drySlider.getValue() / 100);
    else if (slider == &wetSlider) audioProcessor.wet = static_cast<float>(wetSlider.getValue() / 100);
    else if (slider == &feedbackSlider) audioProcessor.feedback = static_cast<float>(feedbackSlider.getValue() / 100);
}

void A2StarterAudioProcessorEditor::pingpongValueChanged()
{
    switch (pingpongBox.getSelectedId())
    {
        case 1: audioProcessor.pingpong = false;  break;
        case 2: audioProcessor.pingpong = true;  break;
        default: break;
    }
    audioProcessor.pingpongSwitch = false;
}

void A2StarterAudioProcessorEditor::resetPressed()
{
    delaySlider.setValue(1.5);
    drySlider.setValue(100.0);
    wetSlider.setValue(100.0);
    feedbackSlider.setValue(0.0);
    pingpongBox.setSelectedId(1);
}
