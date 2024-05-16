/*
  ==============================================================================

    AnalogVuMeterProcessor.cpp
    Created: 19 Mar 2024 4:06:42pm
    Author:  orstphone@github.com
    state space models simulation code ref :
    https://github.com/AleksandarHaber/Simulation-of-State-Space-Models-of-Dynamical-Systems-in-Cpp--Eigen-Matrix-Library-Tutorial/blob/master/SimulateSystem.cpp
  ==============================================================================
*/
#pragma once

#include "AnalogVuMeterProcessor.h"

using mat = juce::dsp::Matrix<float>;

//default constructors destructors========================================================
AnalogVuMeterProcessor::AnalogVuMeterProcessor() : systemOrder(DspLine::Constants::kSystemOrder),
                                                   m_spec (juce::dsp::ProcessSpec { 0, 0, 0 })
{
}

AnalogVuMeterProcessor::AnalogVuMeterProcessor (juce::dsp::ProcessSpec spec) : m_spec (spec)
{
    // If this class is used without caution and processBlock
    // is called before prepareToPlay, divisions by zero
    // might occur. E.g. if numberOfSamplesInAllBins = 0.
    // To prevent this, prepareToPlay is called here with
    // some arbitrary arguments.
    m_systemMatrices.setMatrices();
    this->systemOrder = DspLine::Constants::kSystemOrder;
    prepareToPlay (m_spec.sampleRate, m_spec.numChannels, m_spec.maximumBlockSize); //arbitrary input
}

AnalogVuMeterProcessor::~AnalogVuMeterProcessor()
{
    //past 4 values for v2i System I
    //    z1.free();
    //    w1.free();
    bufferPreSysI.free();
    bufferPreSysII.free();

}

juce::AudioBuffer<float> AnalogVuMeterProcessor::getOutputBuffer()
{
    return m_bufferProcessResult;
}

//JUCE related functions ========================================================
//this prepareToPlay is called exactly once. Will be called right off the bat when the object is created.
void AnalogVuMeterProcessor::prepareToPlay (double sampleRate, int numberOfInputChannels, int estimatedSamplesPerBlock)
{
    //init. spec
    m_spec.sampleRate = sampleRate;
    m_spec.numChannels = numberOfInputChannels;
    m_spec.maximumBlockSize = estimatedSamplesPerBlock;

    //create empty buffer following specs
    DBG ("flag\tVU\tprepareToPlay setSize");
    m_bufferProcessResult.setSize (numberOfInputChannels, systemOrder + estimatedSamplesPerBlock);
    m_bufferProcessResult.clear();

    //for state space equation : 4 past values for "next init x0 for system I / II"
    //index will be done like `channelNumber * numberOfInputChannels + desiredIndex`

    bufferPreSysI.calloc (numberOfInputChannels * systemOrder, sizeof (float));
    bufferPreSysII.calloc (numberOfInputChannels * systemOrder, sizeof (float));

    //initialize buffer with 4 more spaces
    m_ssms_v2i = std::make_unique<StateSpaceModelSimulation> (m_spec);
    m_ssms_i2a = std::make_unique<StateSpaceModelSimulation> (m_spec);
    m_ssms_v2i->setMatrices (m_systemMatrices.returnAllSystemMatrices (1), systemOrder);
    m_ssms_i2a->setMatrices (m_systemMatrices.returnAllSystemMatrices (2), systemOrder);


}//prepareToPlay End




//from outsize this will be called for the first time.
void AnalogVuMeterProcessor::processBlock (juce::AudioBuffer<float>& rawBuffer)
{
    //assume everything is "cleared" from here.
    //m_buffer must follow and be prepared to deal with the input buffer : matching the dimension accordingly;
    //buffer is the RAW buffer for og signal retaining.
    //channel wise filling m_heap with 4 previous value
    DBG ("VU\tprocessBlock...");
    //bufferPreSysI->makeCopyOf(rawBuffer);
    DBG ("VU\t\tbufferPost1...");
    auto bufferPostI = feedToSteadyStateModel (*m_ssms_v2i, rawBuffer, bufferPreSysI);
    DBG ("VU\t\tbufferPost2...");

    auto bufferPostII = feedToSteadyStateModel (*m_ssms_i2a, bufferPostI, bufferPreSysII);
    m_bufferProcessResult = bufferPostII;
}
// ====================================================
//this function will be used inside feedtoSteadyStateModel() fn.
juce::AudioBuffer<float> AnalogVuMeterProcessor::generateAugmentedBufferWithHeapBlock (juce::AudioBuffer<float>& rawBuffer, juce::HeapBlock<float>& h)
{
    //m_buffer :: extended Buffer
    auto sysDim = DspLine::Constants::kSystemOrder;
    auto numChannels = rawBuffer.getNumChannels();
    auto numSamples = rawBuffer.getNumSamples();

    juce::AudioBuffer<float> returningBuffer;
    returningBuffer.setSize (numChannels, numSamples + sysDim);
    returningBuffer.clear();
    DBG("\t\t\t\t\tset size and clear returning buffer");

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* sample = returningBuffer.getWritePointer(channel);
        DBG("\t\t\t\t\t\tusing heap");
        for (int i = 0; i < numChannels; ++i)
        { //range :: -4 ~ 0 : acquire from the heap `h`
            sample[i] = h[_heapIdx(i, channel)];
        }
        DBG("\t\t\t\t\t\tusing rest");

        for (int i = 0; i < numChannels; ++i)
        { //range :: 0 ~ numChannels
            returningBuffer.copyFrom (i, systemOrder, rawBuffer, channel , systemOrder, numSamples - systemOrder);
        }
    }

    //store [-4 : -1] values from returningBuffer. updating heap `h` with output before return
    for(int channel = 0; channel < numChannels; ++channel)
    {
        const float* sample = returningBuffer.getReadPointer(channel);
        for(int i = 0; i < systemOrder; ++i)
        {
            auto j = numSamples - 4 + i;
            h[_heapIdx(i, channel)] = sample[j];
        }
    }
    return returningBuffer;
}

//takes in systemI or II and corresponding heapBlock for recording last 4 outputs.
juce::AudioBuffer<float> AnalogVuMeterProcessor::feedToSteadyStateModel (
    StateSpaceModelSimulation& ssms,
    juce::AudioBuffer<float>& rawBuffer,
    juce::HeapBlock<float>& h)
{


    // rawBuffer dimension = numSamples
    // augBuffer dimension = systemSize + numSamples
    const int numberOfChannels = rawBuffer.getNumChannels();
    const int numberOfSamples = rawBuffer.getNumSamples();
    DBG ("\t\t\tfeed to SSM...");
    DBG ("\t\t\tcreate augBuffer...");

    // build makeshift sysOrder buffer for set_x0 : augBuffer
    auto augBuffer = generateAugmentedBufferWithHeapBlock(rawBuffer, h);

    DBG ("\t\t\tcreate augBuffer...E");

    for (int ch = 0; ch < numberOfChannels; ++ch)
    {
        DBG ("\t\t\t\tset the x0");
        ssms.set_x0 (augBuffer, ch);
        DBG ("\t\t\t\tprocess augBuffer");
        ssms.processBlock (augBuffer);
        DBG ("\t\t\t\tprocess augBuffer...E");

    }
    DBG ("\t\t\t\tretrieving buffer from augBuffer...");
    auto outputBuffer = ssms.retrieveSystemResultBuffer();
    DBG ("\t\t\t\tretrieving buffer from augBuffer...E");
    return outputBuffer;
}
