/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A1StarterAudioProcessorEditor::A1StarterAudioProcessorEditor (A1StarterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 580);
    
    // these define the parameters of our slider object
    arpSlider.setRange (0.0, 1.0, 0.05);
    arpSlider.setTextValueSuffix ("x Speed");
    arpSlider.setValue(0.5);
    // this function adds the slider to the editor
    addAndMakeVisible (&arpSlider);
    arpSlider.addListener (this);
    // function label
    addAndMakeVisible(arpLabel);
    arpLabel.setText("Arpeggiator Speed", juce::dontSendNotification);
    arpLabel.attachToComponent(&arpSlider, false); // [4]

    // add items to the combo-box
    addAndMakeVisible(orderMenu);
    orderMenu.addItem("Ascending", 1);
    orderMenu.addItem("Descending", 2);
    orderMenu.addItem("Ascending and Descending", 3);
    orderMenu.addItem("Inward and Outward", 4);
    //orderMenu.addItem("Outward", 5);
    orderMenu.addItem("Random Repeating Order", 6);
    orderMenu.addItem("Random Non-Repeating Order", 7);
    orderMenu.addItem("True Random", 8);
    orderMenu.onChange = [this] { orderMenuChanged(); };
    orderMenu.setSelectedId(1);

    addAndMakeVisible(orderLabel);
    orderLabel.setText("Order", juce::dontSendNotification);
    orderLabel.attachToComponent(&orderMenu, false); // [4]

    // add items to the combo-box
    addAndMakeVisible(octaveMenu);
    //octaveMenu.addItem("+4 Octaves", 1);
    octaveMenu.addItem("+3 Octaves", 2);
    octaveMenu.addItem("+2 Octaves", 3);
    octaveMenu.addItem("+1 Octave", 4);
    octaveMenu.addItem("+0 Octaves", 5);
    octaveMenu.addItem("-1 Octave", 6);
    octaveMenu.addItem("-2 Octaves", 7);
    octaveMenu.addItem("-3 Octaves", 8);
    //octaveMenu.addItem("-4 Octaves", 9);
    octaveMenu.onChange = [this] { octaveMenuChanged(); };
    octaveMenu.setSelectedId(5);

    addAndMakeVisible(octaveLabel);
    octaveLabel.setText("Octave Span", juce::dontSendNotification);
    octaveLabel.attachToComponent(&octaveMenu, false); // [4]

    // add items to the combo-box
    addAndMakeVisible(repeatMenu);
    repeatMenu.addItem("No Repeated Note", 1);
    repeatMenu.addItem("Repeat 1st Note", 2);
    repeatMenu.addItem("Repeat 2nd Note", 3);
    repeatMenu.addItem("Repeat 3rd Note", 4);
    repeatMenu.addItem("Repeat 4th Note", 5);
    repeatMenu.addItem("Repeat 5th Note", 6);
    repeatMenu.addItem("Repeat 6th Note", 7);
    repeatMenu.addItem("Repeat 7th Note", 8);
    repeatMenu.addItem("Repeat 8th Note", 9);
    repeatMenu.addItem("Repeat 9th Note", 10);
    repeatMenu.addItem("Repeat 10th Note", 11);
    repeatMenu.onChange = [this] { repeatMenuChanged(); };
    repeatMenu.setSelectedId(1);

    addAndMakeVisible(repeatLabel);
    repeatLabel.setText("Repeated Note", juce::dontSendNotification);
    repeatLabel.attachToComponent(&repeatMenu, false); // [4]

    auto min = 0.25;
    auto max = 2.0;
    auto interval = 0.25;
    // these define the parameters of our slider object
    speed0.setRange(min, max, interval);
    speed0.setTextValueSuffix("x Speed");
    speed0.setValue(1.0);
    addAndMakeVisible(&speed0);
    speed0.addListener(this);
    addAndMakeVisible(speed0Label);
    speed0Label.setText("Override 1st Note Speed", juce::dontSendNotification);
    speed0Label.attachToComponent(&speed0, false); // [4]
    // these define the parameters of our slider object
    speed1.setRange(min, max, interval);
    speed1.setTextValueSuffix("x Speed");
    speed1.setValue(1.0);
    addAndMakeVisible(&speed1);
    speed1.addListener(this);
    addAndMakeVisible(speed1Label);
    speed1Label.setText("Override 2nd Note Speed", juce::dontSendNotification);
    speed1Label.attachToComponent(&speed1, false); // [4]
    // these define the parameters of our slider object
    speed2.setRange(min, max, interval);
    speed2.setTextValueSuffix("x Speed");
    speed2.setValue(1.0);
    addAndMakeVisible(&speed2);
    speed2.addListener(this);
    addAndMakeVisible(speed2Label);
    speed2Label.setText("Override 3rd Note Speed", juce::dontSendNotification);
    speed2Label.attachToComponent(&speed2, false); // [4]
    // these define the parameters of our slider object
    speed3.setRange(min, max, interval);
    speed3.setTextValueSuffix("x Speed");
    speed3.setValue(1.0);
    addAndMakeVisible(&speed3);
    speed3.addListener(this);
    addAndMakeVisible(speed3Label);
    speed3Label.setText("Override 4th Note Speed", juce::dontSendNotification);
    speed3Label.attachToComponent(&speed3, false); // [4]
    // these define the parameters of our slider object
    speed4.setRange(min, max, interval);
    speed4.setTextValueSuffix("x Speed");
    speed4.setValue(1.0);
    addAndMakeVisible(&speed4);
    speed4.addListener(this);
    addAndMakeVisible(speed4Label);
    speed4Label.setText("Override 5th Note Speed", juce::dontSendNotification);
    speed4Label.attachToComponent(&speed4, false); // [4]
    // these define the parameters of our slider object
    speed5.setRange(min, max, interval);
    speed5.setTextValueSuffix("x Speed");
    speed5.setValue(1.0);
    addAndMakeVisible(&speed5);
    speed5.addListener(this);
    addAndMakeVisible(speed5Label);
    speed5Label.setText("Override 6th Note Speed", juce::dontSendNotification);
    speed5Label.attachToComponent(&speed5, false); // [4]
    // these define the parameters of our slider object
    speed6.setRange(min, max, interval);
    speed6.setTextValueSuffix("x Speed");
    speed6.setValue(1.0);
    addAndMakeVisible(&speed6);
    speed6.addListener(this);
    addAndMakeVisible(speed6Label);
    speed6Label.setText("Override 7th Note Speed", juce::dontSendNotification);
    speed6Label.attachToComponent(&speed6, false); // [4]
    // these define the parameters of our slider object
    speed7.setRange(min, max, interval);
    speed7.setTextValueSuffix("x Speed");
    speed7.setValue(1.0);
    addAndMakeVisible(&speed7);
    speed7.addListener(this);
    addAndMakeVisible(speed7Label);
    speed7Label.setText("Override 8th Note Speed", juce::dontSendNotification);
    speed7Label.attachToComponent(&speed7, false); // [4]
    // these define the parameters of our slider object
    speed8.setRange(min, max, interval);
    speed8.setTextValueSuffix("x Speed");
    speed8.setValue(1.0);
    addAndMakeVisible(&speed8);
    speed8.addListener(this);
    addAndMakeVisible(speed8Label);
    speed8Label.setText("Override 9th Note Speed", juce::dontSendNotification);
    speed8Label.attachToComponent(&speed8, false); // [4]
    // these define the parameters of our slider object
    speed9.setRange(min, max, interval);
    speed9.setTextValueSuffix("x Speed");
    speed9.setValue(1.0);
    addAndMakeVisible(&speed9);
    speed9.addListener(this);
    addAndMakeVisible(speed9Label);
    speed9Label.setText("Override 10th Note Speed", juce::dontSendNotification);
    speed9Label.attachToComponent(&speed9, false); // [4]

    addAndMakeVisible(resetButton);
    resetButton.setButtonText("Reset");
    resetButton.onClick = [this] { resetPressed();};
}

A1StarterAudioProcessorEditor::~A1StarterAudioProcessorEditor()
{
}

//==============================================================================
void A1StarterAudioProcessorEditor::paint (juce::Graphics& g)
{
   // fill the whole window white
    g.fillAll (juce::Colours::black);
 
    // set the current drawing colour to black
    g.setColour (juce::Colours::white);
 
    // set the font size and draw text to the screen
    g.setFont (30.0f);
    g.drawFittedText ("Arpeggiator", 10, 10, getWidth()-10, 30, juce::Justification::centred, 1);

    // set the font size and draw text to the screen
    g.setFont(15.0f);
    g.drawFittedText("Note Speed Overrides", 10, 250, getWidth() - 10, 30, juce::Justification::left, 1);
}

void A1StarterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // sets the position and size of the slider with arguments (x, y, width, height)
    auto sliderLeft = 10;
    auto sliderTop = 40;
    arpSlider.setBounds(sliderLeft, sliderTop + 30, getWidth() - sliderLeft - 10, 20);
    orderMenu.setBounds(sliderLeft, sliderTop + 80, getWidth() - sliderLeft - 10, 20);
    octaveMenu.setBounds(sliderLeft, sliderTop + 130, getWidth() - sliderLeft - 10, 20);
    repeatMenu.setBounds(sliderLeft, sliderTop + 180, getWidth() - sliderLeft - 10, 20);
    speed0.setBounds(sliderLeft, sliderTop + 260, getWidth()/2 - sliderLeft - 10, 20);
    speed1.setBounds(sliderLeft + getWidth() / 2, sliderTop + 260, getWidth() / 2 - sliderLeft - 10, 20);
    speed2.setBounds(sliderLeft, sliderTop + 310, getWidth() / 2 - sliderLeft - 10, 20);
    speed3.setBounds(sliderLeft + getWidth() / 2, sliderTop + 310, getWidth() / 2 - sliderLeft - 10, 20);
    speed4.setBounds(sliderLeft, sliderTop + 360, getWidth() / 2 - sliderLeft - 10, 20);
    speed5.setBounds(sliderLeft + getWidth() / 2, sliderTop + 360, getWidth() / 2 - sliderLeft - 10, 20);
    speed6.setBounds(sliderLeft, sliderTop + 410, getWidth() / 2 - sliderLeft - 10, 20);
    speed7.setBounds(sliderLeft + getWidth() / 2, sliderTop + 410, getWidth() / 2 - sliderLeft - 10, 20);
    speed8.setBounds(sliderLeft, sliderTop + 460, getWidth() / 2 - sliderLeft - 10, 20);
    speed9.setBounds(sliderLeft + getWidth() / 2, sliderTop + 460, getWidth() / 2 - sliderLeft - 10, 20);
    resetButton.setBounds(2 * (getWidth() / 3) - sliderLeft, sliderTop + 500, getWidth() / 3, 25);
}

void A1StarterAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &arpSlider) audioProcessor.arpSpeed = arpSlider.getValue();
    else if (slider == &speed0) audioProcessor.durations.set(0, 1 / speed0.getValue());
    else if (slider == &speed1) audioProcessor.durations.set(1, 1 / speed1.getValue());
    else if (slider == &speed2) audioProcessor.durations.set(2, 1 / speed2.getValue());
    else if (slider == &speed3) audioProcessor.durations.set(3, 1 / speed3.getValue());
    else if (slider == &speed4) audioProcessor.durations.set(4, 1 / speed4.getValue());
    else if (slider == &speed5) audioProcessor.durations.set(5, 1 / speed5.getValue());
    else if (slider == &speed6) audioProcessor.durations.set(6, 1 / speed6.getValue());
    else if (slider == &speed7) audioProcessor.durations.set(7, 1 / speed7.getValue());
    else if (slider == &speed8) audioProcessor.durations.set(8, 1 / speed8.getValue());
    else if (slider == &speed9) audioProcessor.durations.set(9, 1 / speed9.getValue());
}
void A1StarterAudioProcessorEditor::orderMenuChanged()
{
    switch (orderMenu.getSelectedId())
    {
        case 1: audioProcessor.order = 0;  break;
        case 2: audioProcessor.order = 1;  break;
        case 3: audioProcessor.order = 2;  break;
        case 4: audioProcessor.order = 3;  break;
        case 5: audioProcessor.order = 4;  break;
        case 6: audioProcessor.order = 5;  break;
        case 7: audioProcessor.order = 6;  break;
        case 8: audioProcessor.order = 7;  break;
        default: break;
    }
}
void A1StarterAudioProcessorEditor::octaveMenuChanged()
{
    switch (octaveMenu.getSelectedId())
    {
        case 1: audioProcessor.octave = 4;  break;
        case 2: audioProcessor.octave = 3;  break;
        case 3: audioProcessor.octave = 2;  break;
        case 4: audioProcessor.octave = 1;  break;
        case 5: audioProcessor.octave = 0;  break;
        case 6: audioProcessor.octave = -1;  break;
        case 7: audioProcessor.octave = -2;  break;
        case 8: audioProcessor.octave = -3;  break;
        case 9: audioProcessor.octave = -4;  break;
        default: break;
    }
}

void A1StarterAudioProcessorEditor::repeatMenuChanged()
{
    switch (repeatMenu.getSelectedId())
    {
    case 1: audioProcessor.repeat = -1;  break;
    case 2: audioProcessor.repeat = 0;  break;
    case 3: audioProcessor.repeat = 1;  break;
    case 4: audioProcessor.repeat = 2;  break;
    case 5: audioProcessor.repeat = 3;  break;
    case 6: audioProcessor.repeat = 4;  break;
    case 7: audioProcessor.repeat = 5;  break;
    case 8: audioProcessor.repeat = 6;  break;
    case 9: audioProcessor.repeat = 7;  break;
    case 10: audioProcessor.repeat = 8;  break;
    case 11: audioProcessor.repeat = 9;  break;
    default: break;
    }
}

void A1StarterAudioProcessorEditor::resetPressed()
{
    arpSlider.setValue(0.5);
    orderMenu.setSelectedId(1);
    octaveMenu.setSelectedId(5);
    repeatMenu.setSelectedId(1);
    speed0.setValue(1.0);
    speed1.setValue(1.0);
    speed2.setValue(1.0);
    speed3.setValue(1.0);
    speed4.setValue(1.0);
    speed5.setValue(1.0);
    speed6.setValue(1.0);
    speed7.setValue(1.0);
    speed8.setValue(1.0); 
    speed9.setValue(1.0);
}

