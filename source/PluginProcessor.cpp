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
    ,maximumEstimatedSamplesPerBlock(1024),
      m_RmsLevelChannel0(-INFINITY),
      m_RmsLevelChannel1(-INFINITY),
      m_peakLevelChannel0(-INFINITY),
      m_peakLevelChannel1(-INFINITY)
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
    buffer.makeCopyOf(bufferForMeter, false);
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.


    //if the plugin is NOT connected
    if(bufferForMeter.getNumChannels() == 0) // not connected
    {
        bufferForMeter.setSize(specs.numChannels, specs.maximumBlockSize, false, false, false);
        for (auto ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
            buffer.clear (ch, 0, bufferForMeter.getNumSamples());
    }

    //else:


    DBG("#Channels = " + juce::String(bufferForMeter.getNumChannels()));
    DBG("#Samples  = " + juce::String(bufferForMeter.getNumSamples()));
    //>>>
    m_RmsLevelChannel0 = bufferForMeter.getRMSLevel(0, 0, bufferForMeter.getNumSamples());
    m_RmsLevelChannel1 = bufferForMeter.getRMSLevel(1, 0, bufferForMeter.getNumSamples());
    m_peakLevelChannel0 = bufferForMeter.getMagnitude(0, 0, bufferForMeter.getNumSamples());
    m_peakLevelChannel1 = bufferForMeter.getMagnitude(1, 0, bufferForMeter.getNumSamples());
    DBG("levelRmsLeft = " + juce::String(levelRmsLeft));
    DBG("levelRmsRigt = " + juce::String(levelRmsRight));

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


void PluginProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    //empty
}

float PluginProcessor::getLevelValueRms (const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if(channel == 0)
        return m_RmsLevelChannel0;
    else if(channel == 1)
        return m_RmsLevelChannel1;
    else{
        return -12.f;
    }
}
float PluginProcessor::getLevelValuePeak (const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if(channel == 0)
        return m_peakLevelChannel0;
    else if(channel == 1)
        return m_peakLevelChannel1;
    else{
        return -12.f;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor;
}
