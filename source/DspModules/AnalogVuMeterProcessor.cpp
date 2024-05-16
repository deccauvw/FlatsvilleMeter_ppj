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
AnalogVuMeterProcessor::AnalogVuMeterProcessor():m_spec(juce::dsp::ProcessSpec{0, 0, 0})
{}

AnalogVuMeterProcessor::AnalogVuMeterProcessor(juce::dsp::ProcessSpec spec): m_spec(spec)
{
    // If this class is used without caution and processBlock
    // is called before prepareToPlay, divisions by zero
    // might occur. E.g. if numberOfSamplesInAllBins = 0.
    // To prevent this, prepareToPlay is called here with
    // some arbitrary arguments.
    m_systemMatrices.setMatrices();
    this->systemOrder = DspLine::Constants::kSystemOrder;
    prepareToPlay(m_spec.sampleRate,m_spec.numChannels,m_spec.maximumBlockSize); //arbitrary input
}

AnalogVuMeterProcessor::~AnalogVuMeterProcessor()
{
    //past 4 values for v2i System I
//    z1.free();
//    w1.free();
    bufferPreSysI.free();
    bufferPreSysII.free();
//    z2.free();
//    z3.free();
//    z4.free();

    //past 4 values for i2a system II
//    w2.free();
//    w3.free();
//    w4.free();
}

juce::AudioBuffer<float> AnalogVuMeterProcessor::getOutputBuffer()
{
    return m_bufferProcessResult;
}

//JUCE related functions ========================================================
//this prepareToPlay is called exactly once. Will be called right off the bat when the object is created.
void AnalogVuMeterProcessor::prepareToPlay(double sampleRate, int numberOfInputChannels, int estimatedSamplesPerBlock)
{
    //init. spec
    m_spec.sampleRate =sampleRate;
    m_spec.numChannels = numberOfInputChannels;
    m_spec.maximumBlockSize = estimatedSamplesPerBlock;

    //create empty buffer following specs
    m_bufferProcessResult.setSize(numberOfInputChannels, systemOrder + estimatedSamplesPerBlock);
    m_bufferProcessResult.clear();

    //for state space equation : 4 past values for "next init x0 for system I / II"
    //index will be done like `channelNumber * numberOfInputChannels + desiredIndex`
    z1.calloc(numberOfInputChannels * systemOrder, sizeof(float));
    w1.calloc(numberOfInputChannels * systemOrder, sizeof(float));
    bufferPreSysI.calloc(1, sizeof(juce::AudioBuffer<float>));
    bufferPreSysII.calloc(1, sizeof(juce::AudioBuffer<float>));

    //initialize buffer with 4 more spaces
    m_ssms_v2i = std::make_unique<StateSpaceModelSimulation>(m_spec);
    m_ssms_i2a = std::make_unique<StateSpaceModelSimulation>(m_spec);
    m_ssms_v2i->setMatrices(m_systemMatrices.returnAllSystemMatrices(1), systemOrder);
    m_ssms_i2a->setMatrices(m_systemMatrices.returnAllSystemMatrices(2), systemOrder);


//    m_initialStateBufferForSystemI.setSize(numberOfInputChannels, estimatedSamplesPerBlock + systemOrder);
//    m_initialStateBufferForSystemII.setSize(numberOfInputChannels, estimatedSamplesPerBlock + systemOrder);
//    m_initialStateBufferForSystemI.clear();
//    m_initialStateBufferForSystemII.clear();
}
//prepareToPlay End

//from outsize this will be called for the first time.
void AnalogVuMeterProcessor::processBlock (juce::AudioBuffer<float>& rawBuffer)
{
    //assume everything is "cleared" from here.
    //m_buffer must follow and be prepared to deal with the input buffer : matching the dimension accordingly;
    //buffer is the RAW buffer for og signal retaining.
    //channel wise filling m_heap with 4 previous value
    bufferPreSysI->makeCopyOf(rawBuffer);
    auto bufferPostI = feedToSteadyStateModel(*m_ssms_v2i, rawBuffer, bufferPreSysI);
    auto bufferPostII = feedToSteadyStateModel(*m_ssms_i2a, bufferPostI, bufferPreSysII);
    m_bufferProcessResult = bufferPostII;

}
// ====================================================
//this function will be used inside feedtoSteadyStateModel() fn.
juce::AudioBuffer<float> AnalogVuMeterProcessor::initializeBufferWithHeapBlock(juce::AudioBuffer<float>& rawBuffer, juce::HeapBlock<juce::AudioBuffer<float>>& b)
{
    //m_buffer :: extended Buffer
    auto sysDim = DspLine::Constants::kSystemOrder;
    auto numChannels = rawBuffer.getNumChannels();
    auto numSamples = rawBuffer.getNumSamples();

    juce::AudioBuffer<float> returningBuffer;
    returningBuffer.setSize(numChannels, numSamples);
    returningBuffer.clear();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto sampleIn = rawBuffer.getWritePointer(channel);
        auto sampleOut = returningBuffer.getWritePointer(channel);
        auto heap = b->getWritePointer(channel);

        //filling 4 beginning values into the sample
        for(auto n = 0; n < sysDim ; ++n) //systemSize == 4
            sampleOut[n] = heap[numSamples - sysDim + n];

        //filling everything else
        for(auto n = sysDim; n < numSamples + sysDim; ++n)
        {
            sampleOut[n] = sampleIn[n - sysDim];
        }
    }
    b->makeCopyOf(returningBuffer); //updating heap `b` with output before return
    return returningBuffer;
}

//takes in systemI or II and corresponding heapBlock for recording last 4 outputs.
juce::AudioBuffer<float> AnalogVuMeterProcessor::feedToSteadyStateModel(
    StateSpaceModelSimulation& ssms,
    juce::AudioBuffer<float>& rawBuffer,
    juce::HeapBlock<juce::AudioBuffer<float>> &h)
{
    //this should output buffer for next system.
    //class m_buffer dimension = systemSize + numSamples
    const int numberOfChannels = rawBuffer.getNumChannels();
//    const int numberOfSamples = m_buffer.getNumSamples();
    printf("runS imlation of the  buffer..\n");

    for(int ch = 0 ; ch<numberOfChannels;++ch)
    {
        ssms.set_x0 (*h, ch);
       ssms.processBlock (rawBuffer);
    }
    auto outputBuffer = ssms.retrieveSystemResultBuffer();
    return outputBuffer;
}



