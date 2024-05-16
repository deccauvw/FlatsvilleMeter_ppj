/*
  ==============================================================================

    StateSpaceModelSimulation.cpp
    Created: 25 Mar 2024 8:51:42pm
    Author: Orstphone @ github

  ==============================================================================
*/

#include "StateSpaceModelSimulation.h"

using mat = juce::dsp::Matrix<float>;

StateSpaceModelSimulation::StateSpaceModelSimulation() : stateSpaceModelChannels (0),
                                                         sysDim(DspLine::Constants::kSystemOrder),
                                                         m (1),
                                                         n (1),
                                                         r (1),
                                                         timeSamples (1),
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
StateSpaceModelSimulation::StateSpaceModelSimulation(juce::dsp::ProcessSpec &spec) :
                                                         stateSpaceModelChannels (spec.numChannels),
                                                         sysDim(DspLine::Constants::kSystemOrder),
                                                         m (sysDim),
                                                         n (sysDim),
                                                         r (sysDim),
                                                         timeSamples (spec.maximumBlockSize),
                                                         A (sysDim, sysDim),
                                                         B (sysDim, 1),
                                                         C (1, sysDim),
                                                         D (1, 1),
                                                         timeRowVector (1, sysDim)
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
void prepareEverything(juce::AudioBuffer<float>& buffer);
//set x0  ===================================================
void StateSpaceModelSimulation::setInitStateBuffer (juce::AudioBuffer<float>& initialBuffer, size_t systemSize)
{
    //creates x0 where dim ~ [0:sysDim)
    //transform initialBuffer to matrices vector where v[ch] == corresponding data
    jassert (initialBuffer.getNumSamples() == systemSize);

    std::vector<mat> initialStateMatrix_multiChannel;
    for (int channel = 0; channel < initialBuffer.getNumChannels(); ++channel)
    { //channel-wise computation
        mat initialStateMatrix_singleChannel (systemSize, 0); //systemSize == 4
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
//  ===================================================



void StateSpaceModelSimulation::setSystemSize (size_t systemSize) noexcept
{
    this->sysDim = systemSize;
}

void StateSpaceModelSimulation::setNumChannels (size_t channels) noexcept
{
    this->stateSpaceModelChannels = channels;
}

//must come after `setEverything`
void StateSpaceModelSimulation::processBlock (juce::AudioBuffer<float>& buffer)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
   juce::AudioBuffer<float> outputBuffer;
   outputBuffer.makeCopyOf(buffer);

    for(auto ch = 0 ; ch < numChannels ; ++ch)
    {
        auto sample = buffer.getWritePointer(ch);
        juce::AudioBuffer<float> bufferMono(1, numSamples);
        bufferMono.copyFrom(1, 0, buffer, ch, 0, numSamples);
        set_x0 (bufferMono, ch);
        set_x (bufferMono, ch);
        runSimulation(bufferMono, ch);
        getSimulatedOutputBufferFromMatrixMono(ch);
    }

}


void StateSpaceModelSimulation::setMatrices (std::vector<mat> matrixSupplierVector, int sysDim)
{
    jassert(this->sysDim == sysDim);
    auto& v = matrixSupplierVector;
    this->A = std::move(v.at(0));
    this->B = std::move(v.at(1));
    this->C = std::move(v.at(2));
    this->D = std::move(v.at(3));
}


void StateSpaceModelSimulation::set_x0 (juce::AudioBuffer<float>& buffer, int channel)
{
    jassert (buffer.getNumSamples() == sysDim);
    std::vector<mat> x0_multi;
    for (int channel = 0; channel < stateSpaceModelChannels; ++channel)
    {
        mat x0_mono (sysDim, 1);
        const float* in = buffer.getReadPointer (channel);
        for (size_t i = 0; i < sysDim; i++)
        {
            x0_mono (i, 0) = in[i];
        }
        x0_multi.push_back (x0_mono);
    }

    this->x0 = x0_multi;
}

//set u from audioBuffer<float> : set u[n] : set inputSequence =====================================
void StateSpaceModelSimulation::set_x (juce::AudioBuffer<float>& buffer, int channel)
{
    auto numSamples = buffer.getNumSamples();
    jassert (buffer.getNumChannels() == stateSpaceModelChannels);

    std::vector<mat> x_multi;

    for (int channel = 0; channel < stateSpaceModelChannels; ++channel)
    {
        const float* in = buffer.getReadPointer (channel);
        mat u_inputTrain (1, numSamples);
        for (int i = 0; i < numSamples; ++i)
        {
            u_inputTrain (0, i) = in[i];
        }
        x_multi.push_back (u_inputTrain);
    }

    this->u = x_multi;
}


//run matrix operation with given four matrices =====================================
void StateSpaceModelSimulation::runSimulation(juce::AudioBuffer<float>& bufferMono, int channel)
{
    //output matrix to store result.
        auto& x_o = x_sim.at (channel);
        auto& y_o = y_sim.at (channel);


    for (int t = 0; t < timeSamples; t++) //t is the "time" index.
    {
        auto nRow = y_o.getNumColumns(); //systemSize ~= 4

        if (t == 0)
        {
            for (int row = 0; row < nRow; row++) //essentially when u = 0
            {
                auto _x0 = x0.at (channel);
                x_o(row, t) = _x0 (row, t);
                y_o(row, t) = (C * _x0) (row, t);
            }
        }
        else
        {
            for (int row = 0; row < nRow; row++)
            {
                x_o (row, t) = (A * x_o + B * u.at (channel) (0, t)) (row, t);
                y_o (row, t) = (C * x_o + D * u.at (channel) (0, t)) (row, t);
            }
        }
    }
    //last 4 samples from x_o need to be stored in heapboxes
}//run simulation End

std::vector<mat> StateSpaceModelSimulation::getSimulatedOutputMatrix()
{
    return y_sim; // 1 x 1 x timeseq
}

juce::AudioBuffer<float> StateSpaceModelSimulation::getSimulatedOutputBufferFromMatrixMono()
{   //output will replace using result::makeCopyOf(buffer_copy)
    auto numSamples = static_cast<int>(y_sim.at(0).getNumColumns());
    auto numCh = static_cast<int>(y_sim.size());
    auto bufferOut = juce::AudioBuffer<float>(numCh, numSamples);

    for(int ch = 0;ch<numCh;++ch)
    {
        float* in = bufferOut.getWritePointer(ch);
        auto y_sim_mono = y_sim.at(ch);
        for (int t = 0; t < numSamples; ++t)
        {
            in[t] = y_sim_mono(0,t);
        }
    }
    return bufferOut;
}
////=======================================================================
void StateSpaceModelSimulation::prepareEverything (juce::AudioBuffer<float>& buffer)
{
    setSystemSize(DspLine::Constants::kSystemOrder);
    setNumChannels(buffer.getNumChannels());
    setInitStateBuffer(buffer, DspLine::Constants::kSystemOrder);
    set_x0(buffer);
    set_x(buffer);
    //setmatrices()

}
