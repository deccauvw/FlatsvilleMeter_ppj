#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
,maximumEstimatedSamplesPerBlock(1024) //arbitrary number to prevent div 0 error
{
}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
//==============================================================================

double PluginProcessor::getTailLengthSeconds() const {return 0.0;}

int PluginProcessor::getNumPrograms(){return 1;}

int PluginProcessor::getCurrentProgram(){return 0;}

void PluginProcessor::setCurrentProgram (int index){    juce::ignoreUnused (index);}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}


void PluginProcessor::releaseResources(){}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//==============================================================================================================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //juce::ignoreUnused (sampleRate, samplesPerBlock);
    this->specs.sampleRate = sampleRate;
    this->specs.maximumBlockSize = samplesPerBlock;
    this->specs.numChannels = 2; //presume stereo
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
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

    //>>>
    if(!(getBus(false, 0)->isEnabled() && getBus(true,0)->isEnabled())) return;
    auto mainOutput = getBusBuffer(buffer, false, 0); //if we have audio busses at all,
    auto mainInput = getBusBuffer(buffer, true, 0);// they're now main output and input.

    UiToAudioMessage uim;
    while(uiToAudio.pop(uim))
    {
        switch(uim.what)
        {
            case UiToAudioMessage::NEW_VALUE: params[uim.which]->setValueNotifyingHost(params[uim.which]->convertTo0to1(uim.newValue)); break;
            case UiToAudioMessage::BEGIN_EDIT: params[uim.which]->beginChangeGesture(); break;
            case UiToAudioMessage::END_EDIT: params[uim.which]->endChangeGesture(); break;
        }
    }//handle inbound messages from the ui thread

    double rmsSize = (1881.0/44100.0)*getSampleRate(); //higher is slower with larger RMS buffers
    double zeroCrossScale = (1.0/getSampleRate())*44100.0;

    for(int i = 0;i<buffer.getNumSamples();++i)
    {
        auto outL = mainOutput.getWritePointer(0, i);
        auto outR = mainOutput.getWritePointer(1, i);
        auto inL = mainInput.getReadPointer(0, i);
        auto inR = mainInput.getReadPointer(1, i);; //specified we can only be stereo and never mono.
        float currentslewL = (fabs(*inL-(float)previousLeft)/28000.0f)*(float)getSampleRate();
        float currentslewR = (fabs(*inR-(float)previousRight)/28000.0f)*(float)getSampleRate();
        if (currentslewL > slewLeft) slewLeft = currentslewL;
        if (currentslewR > slewRight) slewRight = currentslewR;
        previousLeft = *inL;
        previousRight = *inR;
        //slew measurement is NOT rectified

    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}
void PluginProcessor::parameterGestureChanged (int parameterIndex, bool starting)
{
    juce::ignoreUnused(parameterIndex, starting);
}

void PluginProcessor::processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&)
{
    AudioProcessor::processBlock (<unnamed>, <unnamed>);
}
void PluginProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    AudioToUiMessage msg;
    msg.what = AudioToUiMessage::NEW_VALUE;
    msg.which = (PluginProcessor::Parameters)parameterIndex;
    msg.newValue = params[parameterIndex]->convertFrom0to1(newValue);
    audioToUi.push(msg);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
