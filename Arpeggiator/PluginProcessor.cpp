/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
A1StarterAudioProcessor::A1StarterAudioProcessor()
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

A1StarterAudioProcessor::~A1StarterAudioProcessor()
{
}

//==============================================================================
const juce::String A1StarterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A1StarterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool A1StarterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool A1StarterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double A1StarterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A1StarterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int A1StarterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void A1StarterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String A1StarterAudioProcessor::getProgramName (int index)
{
    return {};
}

void A1StarterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void A1StarterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    notes.clear();                          // [1]
    currentNote = -1;                        // [2]
    lastNoteValue = -1;                     // [3]
    time = 0;                               // [4]

    rate = static_cast<float> (sampleRate); // [5]
    arpSpeed = 0.5;
    std::cout << arpSpeed;

    order = 0;
    std::cout << order;
    order2 = 0;
    octave = 0;
    std::cout << octave;
    repeat = -1;
    std::cout << repeat;
    rCount = 0;
    back = -1;
    front = -1;
    order3 = 0;
    repeatSwitch = 0;
    currentOctave = 0;
    firstTime = 0;

    while (durations.size() <= 10) {
        durations.add(1.0);
    }
}

void A1StarterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A1StarterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void A1StarterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
 
    // We use the audio buffer to get timing information
    auto numSamples = buffer.getNumSamples();                                                       // [7]

    // get note duration
    auto noteDuration = static_cast<int> (std::ceil (rate * 0.25f * (0.1f + (1.0f - (arpSpeed)))));   // [8]

    for (const auto metadata : midi)                                                                // [9]
    {
        const auto msg = metadata.getMessage();

        if      (msg.isNoteOn())  notes.add (msg.getNoteNumber());
        else if (msg.isNoteOff()) {
            notes.removeValue(msg.getNoteNumber());
            currentNote = -1; //added for reset
            back = -1;
            front = -1;
            rCount = 0;
            repeatSwitch = 0;
            currentOctave = 0;
            firstTime = 0;

            order2 = 0;
            order3 = 0;
        }
    }

    midi.clear();// [10]
    if (repeatSwitch == 1 && currentNote != repeat) {
        noteDuration = noteDuration * durations[repeat];
    }else if (currentNote > -1) {
        noteDuration = noteDuration * durations[currentNote];
    }

    if ((time + numSamples) >= noteDuration)                                                        // [11]
    {
        auto offset = juce::jmax (0, juce::jmin ((int) (noteDuration - time), numSamples - 1));     // [12]

        if (lastNoteValue > 0)                                                                      // [13]
        {
            midi.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
            lastNoteValue = -1;
        }

        if (notes.size() > 0)                                                                       // [14]
        {
            if (repeat > -1 && repeat < notes.size() && repeatSwitch == 0)
            {
                repeatSwitch = 1;
            }
            else if (order == 0)
            {
                //currentNote = (currentNote + 1) % notes.size();
                currentNote = currentNote + 1;
                if (currentNote > notes.size() - 1) 
                {
                    currentNote = 0;
                    if (octave > 0) {
                        currentOctave = currentOctave + 1;
                        if (currentOctave > octave) {
                            currentOctave = 0;
                        }
                    }
                    else if (octave < 0) {
                        currentOctave = currentOctave - 1;
                        if (currentOctave < octave) {
                            currentOctave = 0;
                        }
                    }
                }
                if (repeat != currentNote) repeatSwitch = 0;
            }
            else if(order == 1)
            {
                //currentNote = (notes.size() - 1) - ((currentNote + 1) % notes.size());
                currentNote = currentNote - 1;

                if (currentNote < 0 && firstTime == 1) {
                    if (octave > 0) {
                        currentOctave = currentOctave + 1;
                        if (currentOctave > octave) {
                            currentOctave = 0;
                        }
                    }
                    else if (octave < 0) {
                        currentOctave = currentOctave - 1;
                        if (currentOctave < octave) {
                            currentOctave = 0;
                        }
                    }
                }

                if (currentNote < 0) currentNote = notes.size() - 1;
                if (repeat != currentNote) repeatSwitch = 0;

                firstTime = 1;
            } 
            else if (order == 2) 
            {
                if (order2 == 0) {
                    currentNote = currentNote + 1;
                    if (currentNote > notes.size() - 1) 
                    {
                        currentNote = currentNote - 1;
                        order2 = 1;
                    }
                }
                else{
                    currentNote = currentNote - 1;
                    if (currentNote < 0) 
                    {
                        currentNote = currentNote + 1;
                        order2 = 0;
                        if (octave > 0) {
                            currentOctave = currentOctave + 1;
                            if (currentOctave > octave) {
                                currentOctave = 0;
                            }
                        }
                        else if (octave < 0) {
                            currentOctave = currentOctave - 1;
                            if (currentOctave < octave) {
                                currentOctave = 0;
                            }
                        }
                    }
                }
                DBG("order a + d = " << order2);
                if (repeat != currentNote) repeatSwitch = 0;
            }
            else if (order == 3)
            {
                if (order3 == 0) {
                    front = front + 1;

                    if (front > notes.size() - 1) {
                        if (octave > 0) {
                            currentOctave = currentOctave + 1;
                            if (currentOctave > octave) {
                                currentOctave = 0;
                            }
                        }
                        else if (octave < 0) {
                            currentOctave = currentOctave - 1;
                            if (currentOctave < octave) {
                                currentOctave = 0;
                            }
                        }
                    }

                    if (front > notes.size() - 1) {
                        front = 0;
                        //firstTime = 1;
                    }
                    currentNote = front;
                    order3 = 1;
                }
                else {
                    back = back - 1;
                    if (back < 0) back = notes.size() - 1;
                    currentNote = back;
                    order3 = 0;
                }
                if (repeat != currentNote) repeatSwitch = 0;
            }
            else if (order == 5)
            {
                if (randomNotes.size() != notes.size()) {
                    rCount = 0;
                    randomNotes.clear();
                    while (randomNotes.size() != notes.size()) {
                        auto randNum = rnd.nextInt(notes.size());
                        if (!randomNotes.contains(randNum)) {
                            randomNotes.add(randNum);
                        }
                    }
                }
                else {
                    rCount = rCount + 1;
                    if (rCount > notes.size() - 1) {
                        rCount = 0;
                        if (octave > 0) {
                            currentOctave = currentOctave + 1;
                            if (currentOctave > octave) {
                                currentOctave = 0;
                            }
                        }
                        else if (octave < 0) {
                            currentOctave = currentOctave - 1;
                            if (currentOctave < octave) {
                                currentOctave = 0;
                            }
                        }
                    }
                }
                currentNote = randomNotes[rCount];
                if (repeat != currentNote) repeatSwitch = 0;
            }
            else if (order == 6)
            {
                if (randomNotes.size() != notes.size()) {
                    rCount = 0;
                    randomNotes.clear();
                    while (randomNotes.size() != notes.size()) {
                        auto randNum = rnd.nextInt(notes.size());
                        if (!randomNotes.contains(randNum)) {
                            randomNotes.add(randNum);
                        }
                    }
                }
                else {
                    rCount = rCount + 1;
                    if (rCount > notes.size() - 1) {
                        rCount = 0;
                        randomNotes.clear();
                        while (randomNotes.size() != notes.size()) {
                            auto randNum = rnd.nextInt(notes.size());
                            if (!randomNotes.contains(randNum)) {
                                randomNotes.add(randNum);
                            }
                        }
                        if (octave > 0) {
                            currentOctave = currentOctave + 1;
                            if (currentOctave > octave) {
                                currentOctave = 0;
                            }
                        }
                        else if (octave < 0) {
                            currentOctave = currentOctave - 1;
                            if (currentOctave < octave) {
                                currentOctave = 0;
                            }
                        }
                    }
                }
                //DBG("r = " << rCount);
                //DBG("o = " << currentOctave);
                currentNote = randomNotes[rCount];
                if (repeat != currentNote) repeatSwitch = 0;
            }
            else if (order == 7)
            {
                currentNote = rnd.nextInt(notes.size());
                
                if (rCount > notes.size() - 1) 
                {
                    if (octave > 0) {
                        currentOctave = currentOctave + 1;
                        if (currentOctave > octave) {
                            currentOctave = 0;
                        }
                    }
                    else if (octave < 0) {
                        currentOctave = currentOctave - 1;
                        if (currentOctave < octave) {
                            currentOctave = 0;
                        }
                    }
                    rCount = 0;
                }
                rCount = rCount + 1;

                if (repeat != currentNote) repeatSwitch = 0;
            }

            if (repeatSwitch == 1 && currentNote != repeat){
                lastNoteValue = notes[repeat] + 12 * currentOctave;
                DBG("current num (repeat) = " << repeat);
                DBG("current note (repeat) = " << lastNoteValue);
                DBG("current octave (repeat) = " << currentOctave);
            }
            else {
                lastNoteValue = notes[currentNote] + 12 * currentOctave;
                DBG("current num = " << currentNote);
                DBG("current note = " << lastNoteValue);
                DBG("current octave = " << currentOctave);
            }
            //DBG("number = " << lastNoteValue);
            midi.addEvent (juce::MidiMessage::noteOn  (1, lastNoteValue, (juce::uint8) 127), offset);
        }
    }

    time = (time + numSamples) % noteDuration;                                                      // [15]
}

//==============================================================================
bool A1StarterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* A1StarterAudioProcessor::createEditor()
{
    return new A1StarterAudioProcessorEditor (*this);
}

//==============================================================================
void A1StarterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void A1StarterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A1StarterAudioProcessor();
}
