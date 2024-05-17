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
    startTimerHz(DspLine::Constants::kInitialRefreshRateHz);
}

AnalogVuMeterProcessor::~AnalogVuMeterProcessor()
{
    //past 4 values for v2i System I
    //    z1.free();
    //    w1.free();
    bufferPreSysI.free();
    bufferPreSysII.free();

}

std::vector<float> AnalogVuMeterProcessor::getVuLevelValue()
{
    auto &b =  m_bufferProcessResult; // is the result of the system
    auto numChannels = static_cast<int>(m_spec.numChannels);
    for (int ch = 0; ch < numChannels ; ++ch)
    {
        auto value =  b.getRMSLevel(ch, 0, b.getNumSamples());
        m_levelValuesVu.at(ch) = value;
    }
    return m_levelValuesVu;
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
    //DBG ("flag\tVU\tprepareToPlay setSize");
    m_bufferProcessResult.setSize (numberOfInputChannels, systemOrder + estimatedSamplesPerBlock);
    m_bufferProcessResult.clear();

    //for state space equation : 4 past values for "next init x0 for system I / II"
    //index will be done like `channelNumber * numberOfInputChannels + desiredIndex`

    bufferPreSysI.calloc (numberOfInputChannels * systemOrder, sizeof (float));
    bufferPreSysII.calloc (numberOfInputChannels * systemOrder, sizeof (float));



    //initialize buffer with 4 more spaces
    m_ssms_v2i = std::make_unique<StateSpaceModelSimulation>(m_spec);
    m_ssms_i2a = std::make_unique<StateSpaceModelSimulation>(m_spec);

    m_ssms_v2i->setMatrices (m_systemMatrices.returnAllSystemMatrices (1), systemOrder);
    m_ssms_i2a->setMatrices (m_systemMatrices.returnAllSystemMatrices (2), systemOrder);



}//prepareToPlay End




//from outsize this will be called for the first tim.
void AnalogVuMeterProcessor::processBlock (juce::AudioBuffer<float>& rawBuffer)
{
    auto gainCalibration = juce::Decibels::decibelsToGain(-1.0f * DspLine::Constants::kVuReferencePointInDb);
    rawBuffer.applyGain(gainCalibration); // VU standard gain reference point

    //assume everything is "cleared" from here.
    //m_buffer must follow and be prepared to deal with the input buffer : matching the dimension accordingly;
    //buffer is the RAW buffer for og signal retaining.
    //channel wise filling m_heap with 4 previous value
    auto bufferPostI = feedToSteadyStateModel (*m_ssms_v2i, rawBuffer, bufferPreSysI);
    recordHeapForNextSystem(bufferPostI, bufferPreSysI);

    auto bufferPostII = feedToSteadyStateModel (*m_ssms_i2a, bufferPostI, bufferPreSysII);
    recordHeapForNextSystem(bufferPostII, bufferPreSysII);

    m_bufferProcessResult = std::move(bufferPostII);
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
    returningBuffer.setSize (numChannels, numSamples + sysDim, false, true);
    //returningBuffer.clear();


    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* sample = returningBuffer.getWritePointer(channel);
        for (int i = 0; i < numChannels; ++i)
        { //range :: -4 ~ 0 : acquire from the heap `h`
            sample[i] = h[_heapIdx(i, channel)];
        }
        for (int i = 0; i < numChannels; ++i)
        { //range :: 0 ~ numChannels
            returningBuffer.copyFrom (i, systemOrder, rawBuffer, channel , systemOrder, numSamples - systemOrder);
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

    const int numberOfChannels = rawBuffer.getNumChannels();
    //const int numberOfSamples = rawBuffer.getNumSamples();

    m_augmentedBuffer = generateAugmentedBufferWithHeapBlock (rawBuffer, h);

    for (int ch = 0; ch < numberOfChannels; ++ch)
    {
        ssms.set_x0 (m_augmentedBuffer, ch);
        ssms.set_u (m_augmentedBuffer, ch);
    }

    ssms.processBlock (m_augmentedBuffer); //augBuffer is NOT modified from here


    auto outputBuffer = ssms.retrieveSystemResultBuffer();
    //DBG(juce::String("_probe_") + juce::String(outputBuffer.getMagnitude(0, 0,outputBuffer.getNumSamples() )) + juce::String("\t\t\t") + juce::String(outputBuffer.getMagnitude(1, 0,outputBuffer.getNumSamples() )));

    return outputBuffer;
}
void AnalogVuMeterProcessor::timerCallback()
{
    getVuLevelValue();
}
void AnalogVuMeterProcessor::recordHeapForNextSystem (juce::AudioBuffer<float>& buffer, juce::HeapBlock<float>& h)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();

    //store [-4 : -1] values from returningBuffer. updating heap `h` with output before return
    for(int channel = 0; channel < numChannels; ++channel)
    {
        const float* sample = buffer.getReadPointer(channel);
        for(int i = 0; i < systemOrder; ++i)
        {
            auto j = numSamples - 4 + i;
            h[_heapIdx(i, channel)] = sample[j];
        }
    }
}
