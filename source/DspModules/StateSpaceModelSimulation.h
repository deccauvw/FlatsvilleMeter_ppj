/*
  ==============================================================================

    StateSpaceModelSimulation.h
    Created: 25 Mar 2024 8:51:42pm
    Author:  deccauvw@github

    reference :
    https://github.com/AleksandarHaber/Simulation-of-State-Space-Models-of-Dynamical-Systems-in-Cpp--Eigen-Matrix-Library-Tutorial/blob/master/SimulateSystem.cpp
  ==============================================================================
*/

#pragma once

#include "juce_audio_processors/juce_audio_processors.h" //quasi juceheader
#include "juce_dsp/juce_dsp.h"
#include "../DspModules/DspModulesHelper.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"
//==============================================================================
/*

    A, B, C, D state space matrices
    x0 : initialState
    inputSequence = buffer :

    n = A.rows ~ input dimension
    m = B.cols ~ state dimension
    r = C.rows ~ output dimension
    timeSamples = inputSequence.cols ~ number of time samples
    initstate : `x0' (dim, 1)
    stateSequenceSimulated : simulated state sequence `x` (dim, samples)
    outp.SequenceSimulated : simulated output sequence `y` (samples)

*/

using mat = juce::dsp::Matrix<float>;

class StateSpaceModelSimulation  : public juce::Component
{
public:
    StateSpaceModelSimulation();
    explicit StateSpaceModelSimulation(juce::dsp::ProcessSpec& spec);
    ~StateSpaceModelSimulation() override;
    //==============================================================================

    void setSystemSize(size_t systemSize) noexcept;

    void setNumChannels(size_t channels) noexcept;

    void processBlock(juce::AudioBuffer<float> &buffer); //buffer is here for spec reference purpose.

    //==============================================================================

    //sets x0 with buffer
    void setBufferInitialState (juce::AudioBuffer<float> &buffer);
    
    //sets x0 with matrix vector
    void set_x0(juce::AudioBuffer<float> &buffer, int channel);

    //Audio buffer to (1x*samples)*ch mat vector u

    void set_u(juce::AudioBuffer<float>& bufferInput, int channel);

    void setMatrices(std::vector<mat> matrixSupplierVector, int sysDim);

    //==============================================================================

    //stateSpaceModel for mono channel
    juce::AudioBuffer<float> runSimulation(int channel);

    //matrix to buffer
    juce::AudioBuffer<float> getSimulatedOutputBufferFromMatrixMono(int channel);

    juce::AudioBuffer<float> retrieveSystemResultBuffer();

    //==============================================================================
private:
    juce::AudioBuffer<float> m_outputBuffer;

    size_t m_numChannels;
    size_t m_sysDim;
    //size_t timeSamples; //timeSamples := the `[n]` part //m, n, r,
    mat A, B, C, D;
    std::vector<mat> x0; //init state vector for mono channels matrix inputs time sequenced col wise
    std::vector<mat> u; //input sequence vectors for mono channels matrix inputs time sequenced col wise

    std::vector<mat> x_sim; //state vector simulated channelwise
    std::vector<mat> y_sim; //output matrix simulated channelwise
    std::unique_ptr<std::vector<mat>> x_sim_ptr;
    std::unique_ptr<std::vector<mat>> y_sim_ptr;


    mat timeRowVector; // the `'python linspace()`

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StateSpaceModelSimulation)
};
