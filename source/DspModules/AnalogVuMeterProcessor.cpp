/*
  ==============================================================================

    AnalogVuMeterProcessor.cpp
    Created: 19 Mar 2024 4:06:42pm
    Author:  orstphone@github.com
    statespace models simulation code ref :
    https://github.com/AleksandarHaber/Simulation-of-State-Space-Models-of-Dynamical-Systems-in-Cpp--Eigen-Matrix-Library-Tutorial/blob/master/SimulateSystem.cpp
  ==============================================================================
*/
#pragma once

#include "AnalogVuMeterProcessor.h"
//#include "StateSpaceModelSimulation.h"
#include <cmath>

using mat = juce::dsp::Matrix<float>;

//default constructors destructors========================================================
AnalogVuMeterProcessor::AnalogVuMeterProcessor()
{
    // If this class is used without caution and processBlock
    // is called before prepareToPlay, divisions by zero
    // might occure. E.g. if numberOfSamplesInAllBins = 0.
    // To prevent this, prepareToPlay is called here with
    // some arbitrary arguments.
    systemMatrices.setMatrices();
    spec.numChannels = 2;
    spec.maximumBlockSize = 1024;
    spec.sampleRate = 48000;
    prepareToPlay(spec.sampleRate, spec.numChannels,spec.maximumBlockSize);
}

AnalogVuMeterProcessor::~AnalogVuMeterProcessor()
{
    z1.free();
    z2.free();
    z3.free();
    z4.free();
    w1.free();
    w2.free();
    w3.free();
    w4.free();
}

//JUCE related functions ========================================================
void AnalogVuMeterProcessor::prepareToPlay(double sampleRate, int numberOfInputChannels, int estimatedSamplesPerBlock)
{
    //init. spec
    spec.sampleRate =sampleRate;
    spec.numChannels = numberOfInputChannels;
    spec.maximumBlockSize = estimatedSamplesPerBlock;

    //create empty buffer following specs
    bufferForMeasurement.setSize(numberOfInputChannels, estimatedSamplesPerBlock);
    bufferForMeasurement.clear();


    //for state space equation : 4 past values for "next init x0 for system I / II"
    z1.calloc(numberOfInputChannels);
    z2.calloc(numberOfInputChannels);
    z3.calloc(numberOfInputChannels);
    z4.calloc(numberOfInputChannels);
    w1.calloc(numberOfInputChannels);
    w2.calloc(numberOfInputChannels);
    w3.calloc(numberOfInputChannels);
    w4.calloc(numberOfInputChannels);

    //initialize buffer with past values
    initialStateBufferForSystemI.setSize(numberOfInputChannels, sysDim);
    for (int channel = 0; channel < numberOfInputChannels; ++channel)
    {
        float* init = initialStateBufferForSystemI.getWritePointer(channel);
        init[0] = z4[channel];
        init[1] = z3[channel];
        init[2] = z2[channel];
        init[3] = z1[channel];
    }

    initialStateBufferForSystemII.setSize(numberOfInputChannels, sysDim);
    for (int channel = 0; channel < numberOfInputChannels; ++channel)
    {
        float* init = initialStateBufferForSystemII.getWritePointer(channel);
        init[0] = w4[channel];
        init[1] = w3[channel];
        init[2] = w2[channel];
        init[3] = w1[channel];
    }


    //init. every matrix required
    systemMatrices.setMatrices();
    //matrices for system I (voltage to current)
    //matrices for system II(current to galvanometer)


    ssms_v2i.setMatrices(ssm_v2i_A, ssm_v2i_B, ssm_v2i_C, ssm_v2i_D, bufferForMeasurement, outputBufferSystemI, sysDim);
    ssms_i2a.setMatrices(ssm_v2i_A, ssm_v2i_B, ssm_v2i_C, ssm_v2i_D, bufferForMeasurement, outputBufferSystemII, sysDim);
}


//virtually the main function ====================================================
void AnalogVuMeterProcessor::createInitialStateBuffer(juce::AudioBuffer<float>& initialStateBuffer, juce::HeapBlock<float> p1, juce::HeapBlock<float> p2, juce::HeapBlock<float> p3, juce::HeapBlock<float> p4 )
{
    auto numChannels = initialStateBuffer.getNumChannels();
    for(int channel = 0;channel<numChannels;++channel)
    {
        float* in = initialStateBuffer.getWritePointer(channel);
        in[0] = z4[channel];
        in[1] = z3[channel];
        in[2] = z2[channel];
        in[3] = z1[channel];
    }
}


void AnalogVuMeterProcessor::feedToSteadyStateModel(juce::AudioBuffer<float>& buffer)
{
    /*buffer := audio buffer input
     * systemSize (=4) := order of the steady state model
     */

    //initialize everything
    //sysDim = systemSize (=4)
    const int numberOfChannels = buffer.getNumChannels();
    const int numberOfSamples = buffer.getNumSamples();
    const double sr = spec.sampleRate;

    //*********************************************************  System I Voltage To Current
    createInitialStateBuffer(initialStateBufferForSystemI, z1, z2, z3, z4);
    ssms_v2i.setInitStateBuffer(initialStateBufferForSystemI, sysDim);
    ssms_v2i.setInputSequence(buffer);

    for (int ch = 0; ch < numberOfChannels; ++ch)
    {
        //channel-wise simulation will be stored in the class
        ssms_v2i.runSimulation(ch);
    }

    //save prev 4 values for all channels
    auto outputPostSystemI = ssms_v2i.getSimulatedOutputMatrix();

    keepPreviousStateForNextInitSystemI();

    DBG("System I ends here");

    //*********************************************************  System II Current to Needlepoint Angle
    outputPostSystemI = ssms_v2i.getSimulatedOutputMatrix();

    //prep x and x0
    ssms_i2a.setInitStateVector(bufferForInitialStateSystemII, sysDim);
    ssms_i2a.setInputSequence(outputPostSystemI);

    for (int ch = 0; ch < numberOfChannels; ++ch)
    {
        //channel-wise simulation will be stored in the class
        ssms_i2a.runSimulation(ch);
    }

    outputPostSystemII = ssms_i2a.getSimulatedOutputMatrix();
    keepPreviousStateForNextInitSystemII(); //for next block; : prepareToPlay will be skipped next time.
    DBG("System II ends here");

}



void AnalogVuMeterProcessor::keepPreviousStateForNextInitSystemI()
{
    auto numChannels = bufferForInitialStateSystemI.getNumChannels();
    auto numSamples = bufferForInitialStateSystemI.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* init = bufferForInitialStateSystemI.getWritePointer(ch);
        init[0] = z4[ch];
        init[1] = z3[ch];
        init[2] = z2[ch];
        init[3] = z1[ch];
    }
}


void AnalogVuMeterProcessor::keepPreviousStateForNextInitSystemII()
{
    auto numChannels = bufferForInitialStateSystemII.getNumChannels();
    auto numSamples = bufferForInitialStateSystemII.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* init = bufferForInitialStateSystemII.getWritePointer(ch);
        init[0] = w4[ch];
        init[1] = w3[ch];
        init[2] = w2[ch];
        init[3] = w1[ch];
    }
}


//==============================================================================



void AnalogVuMeterProcessor::processBlock(juce::AudioBuffer<float>& buffer)
{

    //feedToSteadyStateEquation(buffer, sysDim);
    DBG("Single processBlock finished...");
}




void AnalogVuMeterProcessor::reset()
{
    auto numChannels = spec.numChannels;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        z1.clear(ch);
        z2.clear(ch);
        z3.clear(ch);
        z4.clear(ch);

        w1.clear(ch);
        w2.clear(ch);
        w3.clear(ch);
        w4.clear(ch);
    }

}

juce::AudioBuffer<float> AnalogVuMeterProcessor::getOutputBuffer()
{
    return bufferForMeasurement;
}
