/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A3AudioProcessorEditor::A3AudioProcessorEditor (A3AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 500);
   
    addAndMakeVisible(&reverbEnable);
    reverbEnable.addItem("None", 1);
    reverbEnable.addItem("Reverb 1", 2);
    reverbEnable.addItem("Reverb 2", 3);
    reverbEnable.addItem("Reverb 3", 4);
    reverbEnable.addItem("Reverb 4", 5);
    addAndMakeVisible(&reverbEnableLbl);
    reverbEnableLbl.setText("Reverb Type", juce::dontSendNotification);
    reverbEnableLbl.attachToComponent(&reverbEnable, false);
    reverbEnable.onChange = [this] {
        if (reverbEnable.getSelectedId() != 3 && reverbEnable.getSelectedId() != 4 && impulse.getSelectedId() != 1) impulse.setSelectedId(1);
        if (reverbEnable.getSelectedId() == 4 && impulse.getSelectedId() != 11) impulse.setSelectedId(11);
    };

    addAndMakeVisible(&roomSize);
    addAndMakeVisible(&roomSizeLbl);
    roomSizeLbl.setText("Room Size", juce::dontSendNotification);
    roomSizeLbl.attachToComponent(&roomSize, false);

    addAndMakeVisible(&damping);
    addAndMakeVisible(&dampingLbl);
    dampingLbl.setText("Damping", juce::dontSendNotification);
    dampingLbl.attachToComponent(&damping, false);

    addAndMakeVisible(&wet);
    addAndMakeVisible(&wetLbl);
    wetLbl.setText("Wet Level", juce::dontSendNotification);
    wetLbl.attachToComponent(&wet, false);

    addAndMakeVisible(&dry);
    addAndMakeVisible(&dryLbl);
    dryLbl.setText("Dry Level", juce::dontSendNotification);
    dryLbl.attachToComponent(&dry, false);
    
    addAndMakeVisible(&width);
    addAndMakeVisible(&widthLbl);
    widthLbl.setText("Width", juce::dontSendNotification);
    widthLbl.attachToComponent(&width, false);

    addAndMakeVisible(&freeze);
    addAndMakeVisible(&freezeLbl);
    freezeLbl.setText("Freeze Mode", juce::dontSendNotification);
    freezeLbl.attachToComponent(&freeze, false);

    addAndMakeVisible(&impulse);
    addAndMakeVisible(&impulseLbl);
    impulse.addItem("None", 1);
    impulse.addItem("Bass Ambience", 2);
    impulse.addItem("Car Park", 3);
    impulse.addItem("Crystal Plate", 4);
    impulse.addItem("Kick Chamber", 5);
    impulse.addItem("Large Hall", 6);
    impulse.addItem("Medium Hall", 7);
    impulse.addItem("Small Hall", 8);
    impulse.addItem("Studio", 9);
    impulse.addItem("Imported Response", 10);
    impulse.addItem("Constructed Response", 11);
    impulse.setSelectedId(1);
    impulseLbl.setText("Impulse Response Types", juce::dontSendNotification);
    impulseLbl.attachToComponent(&impulse, false);
    impulse.onChange = [this] {
        juce::File location;
        if(impulse.getSelectedId() != 1 && reverbEnable.getSelectedId() != 3) reverbEnable.setSelectedId(3);
        switch (impulse.getSelectedId())
        {
            case 2: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("BassAmbience.wav"); break;
            case 3: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("CarPark.wav"); break;
            case 4: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("CrystalPlate.wav"); break;
            case 5: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("KickChamber.wav"); break;
            case 6: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("LargeHall.wav"); break;
            case 7: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("MediumHall.wav"); break;
            case 8: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("SmallHall.wav"); break;
            case 9: location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources").getChildFile("Studio.wav"); break;
            case 10: 
            {
                location = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Resources");
                fileChoice = std::make_unique<juce::FileChooser>("Choose Impulse Response", location, "*.wav");
                fileChoice->launchAsync(juce::FileBrowserComponent::openMode && juce::FileBrowserComponent::canSelectFiles, [this](const juce::FileChooser& chooser) {
                    audioProcessor.loaded = chooser.getResult();
                    audioProcessor.convolution.loadImpulseResponse(chooser.getResult(), juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                    });
                break;
            }
            case 11:
            {
                if(reverbEnable.getSelectedId() != 4) reverbEnable.setSelectedId(4);
                int len = static_cast <int>(audioProcessor.rate * impulseLen.getValue());
                if (len != audioProcessor.buffer.getNumSamples()) {
                    audioProcessor.buffer.setSize(2, len);
                    audioProcessor.buffer.clear(0, audioProcessor.buffer.getNumSamples());
                    audioProcessor.buffer.clear(1, audioProcessor.buffer.getNumSamples());
                    //DBG(len << " max");
                    //DBG(audioProcessor.rate << " rate");
                    //DBG(audioProcessor.buffer.getNumSamples() << " numSamples");
                    float* data1 = audioProcessor.buffer.getWritePointer(0);
                    float* data2 = audioProcessor.buffer.getWritePointer(1);

                    for (int i = 0; i < audioProcessor.buffer.getNumSamples(); ++i) {
                        float value = static_cast <float>( - (1.0 / len));
                        value *= (i - len);
                        float value1 = static_cast <float>(value - value * (1.0 - value) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
                        float value2 = static_cast <float>(value - value * (1.0 - value) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
                       // if (value2 < 0 || value1 > 1) value2 = 0;

                        data1[i] = value1;
                        data2[i] = value2;
                        //DBG(data1[i] << " val1");
                        //DBG(data2[i] << " val2");
                    }
                }
                audioProcessor.convolution.loadImpulseResponse(std::move(audioProcessor.buffer), static_cast<double>(audioProcessor.rate), juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, juce::dsp::Convolution::Normalise::no);
                break;
            }
            default: break;
        }
        if (impulse.getSelectedId() != 10 && impulse.getSelectedId() != 11) audioProcessor.convolution.loadImpulseResponse(location, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
        if (impulse.getSelectedId() == 1) audioProcessor.convolution.reset(); 
    };

    addAndMakeVisible(load);
    load.setButtonText("Import Response");
    load.onClick = [this] {
        impulse.setSelectedId(1);
        impulse.setSelectedId(10);
    };

    addAndMakeVisible(&impulseLen);
    impulseLen.setRange(0.0, 6.0, 0.1);
    impulseLen.setValue(0.0);
    addAndMakeVisible(&impulseLenLbl);
    impulseLenLbl.setText("Constructed Response Length", juce::dontSendNotification);
    impulseLenLbl.attachToComponent(&impulseLen, false);
    impulseLen.onValueChange = [this] {
        impulse.setSelectedId(1);
        impulse.setSelectedId(11);
    };

    addAndMakeVisible(&d1);
    addAndMakeVisible(&d1Lbl);
    d1Lbl.setText("Delay 1 Time", juce::dontSendNotification);
    d1Lbl.attachToComponent(&d1, false);

    addAndMakeVisible(&d2);
    addAndMakeVisible(&d2Lbl);
    d2Lbl.setText("Delay 2 Time", juce::dontSendNotification);
    d2Lbl.attachToComponent(&d2, false);

    addAndMakeVisible(&d3);
    addAndMakeVisible(&d3Lbl);
    d3Lbl.setText("Delay 3 Time", juce::dontSendNotification);
    d3Lbl.attachToComponent(&d3, false);

    addAndMakeVisible(&d4);
    addAndMakeVisible(&d4Lbl);
    d4Lbl.setText("Delay 4 Time", juce::dontSendNotification);
    d4Lbl.attachToComponent(&d4, false);
    addAndMakeVisible(&d1);

    addAndMakeVisible(&d5);
    addAndMakeVisible(&d5Lbl);
    d5Lbl.setText("Delay 5 Time", juce::dontSendNotification);
    d5Lbl.attachToComponent(&d5, false);

    addAndMakeVisible(&f);
    addAndMakeVisible(&fLbl);
    fLbl.setText("Frequency Cutoff", juce::dontSendNotification);
    fLbl.attachToComponent(&f, false);
    
    reverbEnableValue = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "REVERBENABLE", reverbEnable);
    roomSizeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ROOMSIZE", roomSize);
    dampingValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DAMPING", damping);
    wetValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WET", wet);
    dryValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRY", dry);
    widthValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WIDTH", width);
    freezeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREEZE", freeze);

    d1V = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY1", d1);
    d2V = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY2", d2);
    d3V = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY3", d3);
    d4V = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY4", d4);
    d5V = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY5", d5);
    fV = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "CUTOFF", f);

    //impulseValue = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "IMPULSE", impulse);
    //impulseLenValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "IMPULSELEN", impulseLen);
}

A3AudioProcessorEditor::~A3AudioProcessorEditor()
{
}

//==============================================================================
void A3AudioProcessorEditor::paint (juce::Graphics& g)
{
    // fill the whole window white
    g.fillAll(juce::Colours::black);

    // set the current drawing colour to black
    g.setColour(juce::Colours::white);

    // set the font size and draw text to the screen
    g.setFont(30.0f);
    g.drawFittedText("Digital Reverb", 10, 10, getWidth() - 10, 30, juce::Justification::centred, 1);
}

void A3AudioProcessorEditor::resized()
{
    auto sliderLeft = 10;
    auto sliderWidth = getWidth() - sliderLeft*2;
    auto sliderTop = 70;
    auto sliderHeight = 20;
    auto space = 45;
    reverbEnable.setBounds(sliderLeft, sliderTop, sliderWidth, sliderHeight);
    roomSize.setBounds(sliderLeft, sliderTop + space, sliderWidth/2, sliderHeight);
    damping.setBounds(sliderLeft + sliderWidth / 2, sliderTop + space, sliderWidth/2, sliderHeight);
    wet.setBounds(sliderLeft, sliderTop + 2 * space, sliderWidth / 2, sliderHeight);
    dry.setBounds(sliderLeft + sliderWidth / 2, sliderTop + 2 * space, sliderWidth / 2, sliderHeight);
    width.setBounds(sliderLeft, sliderTop + 3 * space, sliderWidth / 2, sliderHeight);
    freeze.setBounds(sliderLeft + sliderWidth / 2, sliderTop + 3 * space, sliderWidth / 2, sliderHeight);
    impulse.setBounds(sliderLeft, sliderTop + 4 * space, (sliderWidth / 3)*2, sliderHeight);
    load.setBounds(sliderLeft + (sliderWidth / 3) * 2 + 10, sliderTop + 4 * space, sliderWidth / 3 - 10, sliderHeight);
    impulseLen.setBounds(sliderLeft, sliderTop + 5 * space, sliderWidth, sliderHeight);

    d1.setBounds(sliderLeft, sliderTop + 6 * space, sliderWidth / 2, sliderHeight);
    d2.setBounds(sliderLeft + sliderWidth / 2, sliderTop + 6 * space, sliderWidth / 2, sliderHeight);
    d3.setBounds(sliderLeft, sliderTop + 7 * space, sliderWidth / 2, sliderHeight);
    d4.setBounds(sliderLeft + sliderWidth / 2, sliderTop + 7 * space, sliderWidth / 2, sliderHeight);
    d5.setBounds(sliderLeft, sliderTop + 8 * space, sliderWidth / 2, sliderHeight);
    f.setBounds(sliderLeft + sliderWidth / 2, sliderTop + 8 * space, sliderWidth / 2, sliderHeight);
}
