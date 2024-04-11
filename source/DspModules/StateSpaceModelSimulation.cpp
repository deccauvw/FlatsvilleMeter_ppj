/*
  ==============================================================================

    StateSpaceModelSimulation.cpp
    Created: 25 Mar 2024 8:51:42pm
    Author: Orstphone @ github

  ==============================================================================
*/

#include "StateSpaceModelSimulation.h"

using mat = juce::dsp::Matrix<float>;

StateSpaceModelSimulation::StateSpaceModelSimulation() : numChannels (0),
                                                         m (0),
                                                         n (0),
                                                         r (0),
                                                         timeSamples (0),
                                                         A (1, 1),
                                                         B (1, 1),
                                                         C (1, 1),
                                                         D (1, 1),
                                                         timeRowVector (1, 1)
{
    A.clear();
    B.clear();
    C.clear();
    D.clear();
    timeRowVector.clear();
}

StateSpaceModelSimulation::~StateSpaceModelSimulation() = default;

//setters
//set x0  ===================================================
void StateSpaceModelSimulation::setInitStateBuffer (juce::AudioBuffer<float>& initialBuffer, size_t systemSize)
{
    //creates x0 where dim ~ [0:sysDim)
    //transform initialBuffer to matrices vector where v[ch] == corresponding data
    jassert (initialBuffer.getNumSamples() == systemSize);

    std::vector<mat> initialStateMatrix_multiChannel;
    for (int channel = 0; channel < initialBuffer.getNumChannels(); ++channel)
    { //channel-wise computation
        mat initialStateMatrix_singleChannel (systemSize, 1); //systemSize == 4
        initialStateMatrix_singleChannel.clear();
        const float* in = initialBuffer.getReadPointer (channel);
        //create chains of 1x1 matrix using vector
        for (int i = 0; i < initialBuffer.getNumSamples(); ++i)
        {
            initialStateMatrix_singleChannel (i, 0) = in[i];
        }
        initialStateMatrix_multiChannel.push_back (initialStateMatrix_singleChannel);
    }

    this->x0 = initialStateMatrix_multiChannel;
}
//set x0  ===================================================

void StateSpaceModelSimulation::setSystemSize (size_t systemSize) noexcept
{
    this->sysDim = systemSize;
}

void StateSpaceModelSimulation::setNumChannels (size_t channels) noexcept
{
    this->numChannels = channels;
}

void StateSpaceModelSimulation::setMatrices (mat ssmA, mat ssmB, mat ssmC, mat ssmD, int systemSize)
{
    jassert(sysDim == systemSize);
    this->A = std::move(ssmA);
    this->B = std::move(ssmB);
    this->C = std::move(ssmC);
    this->D = std::move(ssmD);
}


void StateSpaceModelSimulation::set_x0 (juce::AudioBuffer<float>& initialStateBuffer)
{
    jassert (initialStateBuffer.getNumSamples() == sysDim);
    std::vector<mat> x0_multi;
    for (int channel = 0; channel < numChannels; ++channel)
    {
        mat x0_mono (sysDim, 1);
        const float* in = initialStateBuffer.getReadPointer (channel);
        for (size_t i = 0; i < sysDim; i++)
        {
            x0_mono (i, 1) = in[i];
        }
        x0_multi.push_back (x0_mono);
    }

    this->x0 = x0_multi;
}

//set u from audioBuffer<float> : set u[n] : set inputSequence =====================================
void StateSpaceModelSimulation::set_x (juce::AudioBuffer<float>& buffer)
{
    auto numSamples = buffer.getNumSamples();
    jassert (buffer.getNumChannels() == numChannels);

    std::vector<mat> x_multi;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* in = buffer.getReadPointer (channel);
        mat u_inputTrain (1, numSamples);
        for (int i = 0; i < numSamples; ++i)
        {
            u_inputTrain (1, i) = in[i];
        }
        x_multi.push_back (u_inputTrain);
    }

    this->u = x_multi;
}


//run matrix operation with given four matrices =====================================
void StateSpaceModelSimulation::runSimulation (juce::AudioBuffer<float>& buffer)
{
    auto buffer_ = buffer;


    for(int channel = 0;channel<numChannels;++channel)
    {
        auto& x_ = x_sim.at (channel);
        auto& y_ = y_sim.at (channel);
    }


    for (int j = 0; j < timeSamples; j++) //j is the "time" index.
    {
        auto nRow = y_.getNumColumns(); //systemSize

        if (j == 0)
        {
            for (int row = 0; row < nRow; row++) //essentially when u = 0
            {
                auto _x0 = x0.at (channel);
                x_ (row, j) = _x0 (row, j);
                y_ (row, j) = (C * _x0) (row, j);
            }
        }
        else
        {
            for (int row = 0; row < nRow; row++)
            {
                x_ (row, j) = (A * x_ + B * u.at (channel) (0, j)) (row, j);
                y_ (row, j) = (C * x_ + D * u.at (channel) (0, j)) (row, j);
            }
        }
    }
}

std::vector<mat> StateSpaceModelSimulation::getSimulatedOutputMatrix()
{
    return y_sim;
}

std::vector<std::vector<float>> StateSpaceModelSimulation::convertBufferToVector (juce::AudioBuffer<float>& buffer)
{
    std::vector<std::vector<float>> multipleChannelVectors; //n-channel vector<float> arrays
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        std::vector<float> singleChannelVector;
        const float* in = buffer.getReadPointer (ch);

        for (int i = 0; i < numSamples; i++)
        {
            singleChannelVector.push_back (in[i]);
        }
        multipleChannelVectors.push_back (singleChannelVector);

        return multipleChannelVectors;
    }
}

mat StateSpaceModelSimulation::convertArrayTo2dMatrix (float arr[], int row, int col)
{
    //jassert(sizeof(arr) / sizeof(float) == row * col);
    mat Out = mat (row, col, arr);
    return Out;
}
