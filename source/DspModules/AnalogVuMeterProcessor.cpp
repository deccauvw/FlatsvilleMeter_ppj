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
AnalogVuMeterProcessor::AnalogVuMeterProcessor() = default;

AnalogVuMeterProcessor::AnalogVuMeterProcessor(juce::dsp::ProcessSpec spec)
{
    // If this class is used without caution and processBlock
    // is called before prepareToPlay, divisions by zero
    // might occur. E.g. if numberOfSamplesInAllBins = 0.
    // To prevent this, prepareToPlay is called here with
    // some arbitrary arguments.
    m_systemMatrices.setMatrices();
    this->m_spec = spec;
    this->systemSize = DspLine::Constants::kSystemOrder;
    prepareToPlay(m_spec.sampleRate,m_spec.numChannels,m_spec.maximumBlockSize); //arbitrary input
}

AnalogVuMeterProcessor::~AnalogVuMeterProcessor()
{
    //past 4 values for v2i System I
    z1.free();
//    z2.free();
//    z3.free();
//    z4.free();

    //past 4 values for i2a system II
    w1.free();
//    w2.free();
//    w3.free();
//    w4.free();
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
    m_buffer.setSize(numberOfInputChannels, systemSize + estimatedSamplesPerBlock);
    m_buffer.clear();

    //for state space equation : 4 past values for "next init x0 for system I / II"
    //index will be done like `channelNumber * numberOfInputChannels + desiredIndex`
    z1.calloc(numberOfInputChannels * systemSize, sizeof(float));
//    z2.calloc(numberOfInputChannels * systemSize, sizeof(float));
//    z3.calloc(numberOfInputChannels * systemSize, sizeof(float));
//    z4.calloc(numberOfInputChannels * systemSize, sizeof(float));
    w1.calloc(numberOfInputChannels * systemSize, sizeof(float));
//    w2.calloc(numberOfInputChannels * systemSize, sizeof(float));
//    w3.calloc(numberOfInputChannels * systemSize, sizeof(float));
//    w4.calloc(numberOfInputChannels * systemSize, sizeof(float));
    //calloc == malloc + zeroize

    //initialize buffer with 4 more spaces
    m_ssms_v2i = std::make_unique<StateSpaceModelSimulation>(m_spec);
    m_ssms_i2a = std::make_unique<StateSpaceModelSimulation>(m_spec);
    m_ssms_v2i->setMatrices(m_systemMatrices.returnAllSystemMatrices(1), systemSize);
    m_ssms_i2a->setMatrices(m_systemMatrices.returnAllSystemMatrices(2), systemSize);


    m_initialStateBufferForSystemI.setSize(numberOfInputChannels, estimatedSamplesPerBlock + systemSize);
    m_initialStateBufferForSystemII.setSize(numberOfInputChannels, estimatedSamplesPerBlock + systemSize);
    m_initialStateBufferForSystemI.clear();
    m_initialStateBufferForSystemII.clear();
}
//prepareToPlay End

//from outsize this will be called for the first time.
void AnalogVuMeterProcessor::processBlock (juce::AudioBuffer<float>& buffer)
{
    //assume everything is "cleared" from here.
    //m_buffer must follow and be prepared to deal with the input buffer : matching the dimension accordingly;

    //channel wise filling m_heap with 4 previous values
    initializeBufferWithHeapBlock(m_buffer, z1); //modifies m_buffer
    feedToSteadyStateModel(*m_ssms_v2i, w1);
    initializeBufferWithHeapBlock(m_buffer, w1);
    feedToSteadyStateModel(*m_ssms_i2a, z1);
}
// ====================================================
//HeapBlock is zero padded at first but is not afterwards.
void AnalogVuMeterProcessor::initializeBufferWithHeapBlock(juce::AudioBuffer<float>& buffer, juce::HeapBlock<float>& z)
{
    auto sysDim = DspLine::Constants::kSystemOrder;
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
    auto heap = z.getData();
    auto _heapIndexer = [&](int i, int channel){return buffer.getNumChannels() * channel + i;};


    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto sampleIn = buffer.getWritePointer(channel);
        auto m_sample = m_buffer.getWritePointer(channel);

        //filling 4 beginning values into the sample
        for(auto n = 0; n < sysDim ; ++n) //systemSize == 4
            m_sample[n] = heap[_heapIndexer(n, channel)];

        //filling everything else
        for(auto n = sysDim; n < numSamples + sysDim; ++n)
        {
            m_sample[n] = sampleIn[n];
        }
    }
}

//takes in systemI or II and corresponding heapBlock for recording last 4 outputs.
void AnalogVuMeterProcessor::feedToSteadyStateModel(StateSpaceModelSimulation& ssms,  juce::HeapBlock<float> &h)
{
    //this should output buffer for next system.
    //class m_buffer dimension = systemSize + numSamples
    auto dim = systemSize;
    const int numberOfChannels = m_buffer.getNumChannels();
//    const int numberOfSamples = m_buffer.getNumSamples();
    const double sr = m_spec.sampleRate;

    //prepare everything
    ssms.set_x0(m_buffer);
    ssms.set_x(m_buffer);

    for (int ch = 0; ch < numberOfChannels; ++ch)
    {
        //channel-wise simulation will be stored in the class as m_buffer
        ssms.runSimulation(ch);
    }
    recordHeapForNextSystem(m_buffer, h);
}

void AnalogVuMeterProcessor::recordHeapForNextSystem(juce::AudioBuffer<float>& buffer, juce::HeapBlock<float> &z)
{
    //buffer here should be processed by matrix op.
    //last 4 values will be stored in buffer `z`
    auto sysDim = DspLine::Constants::kSystemOrder;
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
    auto heap = z.getData();
    auto _heapIndexer = [&](int i, int channel){return buffer.getNumChannels() * channel + i;};

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* sample = buffer.getWritePointer(ch);
        for(int n = numSamples; n < numSamples + sysDim; ++n)
        {
            heap[_heapIndexer(n, ch)] = sample[n];
        }
    }
}



void AnalogVuMeterProcessor::reset()
{
    auto numChannels = m_spec.numChannels;

    for (auto ch = 0; ch < numChannels; ++ch)
    {
        z1.clear(ch);
        w1.clear(ch);
    }

}

juce::AudioBuffer<float> AnalogVuMeterProcessor::getOutputBuffer()
{
    return m_buffer;
}
