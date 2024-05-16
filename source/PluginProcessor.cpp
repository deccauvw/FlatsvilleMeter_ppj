#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
    #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      maximumEstimatedSamplesPerBlock (1024),
      apvts(*this, nullptr, "Parameters", createParameters())
{
//    addParameter(param_gain = new juce::AudioParameterFloat())
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

double PluginProcessor::getTailLengthSeconds() const { return 0.0; }

int PluginProcessor::getNumPrograms() { return 1; }

int PluginProcessor::getCurrentProgram() { return 0; }

void PluginProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void PluginProcessor::releaseResources() {}

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
    #if !JucePlugin_IsSynth
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
    DBG("flag\tPluginProcessor::prepareToPlay");
    this->specs.sampleRate = sampleRate;
    this->specs.maximumBlockSize = samplesPerBlock;
    this->specs.numChannels = 2; //assume and FIX as stereo only plugin
    m_nChannelPeakLevels.resize(specs.numChannels);
    m_nChannelRmsLevels.resize(specs.numChannels);
    m_nChannelVuLevels.resize(specs.numChannels);
    //bufferForMeter.setSize(specs.numChannels,samplesPerBlock);

    m_vuMeterProcessor = std::make_unique<AnalogVuMeterProcessor>(this->specs);
    m_vuMeterProcessor->prepareToPlay(this->specs.sampleRate, this->specs.numChannels, this->specs.maximumBlockSize);

    DBG("flag\tPluginProcessor::prepareToPlay E");

}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

                                                            //============================parameters    >>
    auto loadedParamGain =apvts.getRawParameterValue("GAIN");
    auto g = juce::Decibels::decibelsToGain(loadedParamGain->load());



    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


                                                             //============================parameters   <<
    DBG("flag\tPluginProcessor::processBlock");

    buffer.applyGain(g);
    juce::AudioBuffer<float> bufferForMeter;
    bufferForMeter.makeCopyOf(buffer);
    //if the plugin is NOT connected

    if (isBufferEmpty (buffer)) // not connected
    {
        bufferForMeter = juce::AudioBuffer<float> (specs.numChannels, specs.maximumBlockSize);
        for (auto ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
            bufferForMeter.clear (ch, 0, bufferForMeter.getNumSamples());
        //DBG("flag\tE buffer");
    }
    else
    {
        //DBG("flag\tNE buffer");
        //write everything here if and only if buffer is loaded
        //make copy of scaled buffer
        bufferForMeter.makeCopyOf (buffer);

    }

    //from here the buffer is at least filled with zeros

    auto numSamples = bufferForMeter.getNumSamples();
    auto numChannels = bufferForMeter.getNumChannels();

    //DBG("flag\ttrivial meter level acquisition here");
    for (auto ch = 0; ch < numChannels; ch++)
    {
        auto m_iRmsLevel = bufferForMeter.getRMSLevel(ch, 0, numSamples);
        auto m_iPeakLevel = bufferForMeter.getMagnitude(ch, 0, numSamples);
        m_nChannelPeakLevels.at(ch) = m_iPeakLevel;
        m_nChannelRmsLevels.at(ch) = m_iRmsLevel;
    }
    //DBG("flag\ttrivial meter level acquisition here...E");


    //vu stuff
    //DBG("flag\tVU meter operation from here!!");
    DBG(bufferForMeter.getNumSamples());
    m_vuMeterProcessor->processBlock(bufferForMeter);
    //DBG("flag\tVU meter operation returning result");
    m_nChannelVuLevels = m_vuMeterProcessor->getVuLevelValue();

    DBG("VUvalues::"+juce::String(m_nChannelVuLevels.at(0)) + juce::String("    ") + juce::String(m_nChannelVuLevels.at(1)));
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
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
    juce::ignoreUnused (parameterIndex, starting);
}

void PluginProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    juce::ignoreUnused(parameterIndex, newValue);
}
//==============================================================================
bool PluginProcessor::isBufferEmpty (const juce::AudioBuffer<float>& buffer)
{
    //returns true if Empty
    //returns false if not Empty

    if (buffer.getNumChannels() == 0)
    {
        return true;
    }
    //check if any of the channels contain any data
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        if (buffer.getReadPointer (ch) == nullptr)
            return true;
    }
    return false;
}

float PluginProcessor::getLevelValueRms (const int channel) const
{
    jassert (channel == 0 || channel == 1);

    return m_nChannelRmsLevels.at(channel);
}

float PluginProcessor::getLevelValuePeak (const int channel) const
{
    jassert (channel == 0 || channel == 1);
    return m_nChannelPeakLevels.at(channel);
}

float PluginProcessor::getLevelValueVu (const int channel) const
{
    jassert (channel == 0 || channel == 1);
    return m_nChannelVuLevels.at(channel);
}

APVTS::ParameterLayout PluginProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", -24.0f, +24.0f, 0.0f));
    return{params.begin(), params.end()};
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor;
}


