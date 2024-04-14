/*
  ==============================================================================

    StateSpaceModelSimulation.h
    Created: 25 Mar 2024 8:51:42pm
    Author:  prmir

  ==============================================================================
*/

#pragma once

#include "juce_audio_processors/juce_audio_processors.h" //quasi juceheader
#include "juce_dsp/juce_dsp.h"
#include "../DspModules/DspModulesHelper.h"

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

    ~StateSpaceModelSimulation() override;

    //==============================================================================

    void setSystemSize(size_t systemSize) noexcept;

    void setNumChannels(size_t channels) noexcept;

    //void createInitialStateBuffer(juce::AudioBuffer<float>& initialStateBuffer, juce::HeapBlock<float> z1, juce::HeapBlock<float> z2, juce::HeapBlock<float> z3, juce::HeapBlock<float> z4 );


    //sets x0 with buffer
    void setInitStateBuffer(juce::AudioBuffer<float> &initialBuffer, size_t systemSize);
    
    //sets x0 with matrix vector
    void set_x0(juce::AudioBuffer<float> &initialStateBuffer);

    //Audio buffer to (1x*samples)*ch mat vector u
    void set_x(juce::AudioBuffer<float>& buffer);

    void setMatrices(mat ssmA, mat ssmB, mat ssmC, mat ssmD, int sysDim);

    //stateSpaceModel for mono channel
    void runSimulation(int channel);


    std::vector<mat> getSimulatedOutputMatrix();

    //matrix to buffer
    juce::AudioBuffer<float> getSimulatedOutputBuffer();


    //audioBuffer to n channel vector 
    //std::vector<std::vector<float>> convertBufferToVector(juce::AudioBuffer<float>& buffer);

    //float arr[] for juce::dsp::Matrix constructor input argument.



    //==============================================================================
private:
    size_t stateSpaceModelChannels;
    size_t sysDim;
    size_t m, n, r, timeSamples;
    mat A, B, C, D;
    std::vector<mat> x0; //init state vector for multichannel matrix input
    std::vector<mat> u; //input sequence vectors for multi channel scalar input

    std::vector<mat> x_sim; //state vector simulated
    std::vector<mat> y_sim; //ouput matrix simulated
    mat timeRowVector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StateSpaceModelSimulation)
};
