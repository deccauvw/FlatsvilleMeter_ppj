/*
  ==============================================================================

    StateSpaceModelSimulation.cpp
    Created: 25 Mar 2024 8:51:42pm
    Author: Orstphone @ github

  ==============================================================================
*/

#include "StateSpaceModelSimulation.h"

using mat = juce::dsp::Matrix<float>;

StateSpaceModelSimulation::StateSpaceModelSimulation() : m_numChannels (0),
                                                         m_sysDim(DspLine::Constants::kSystemOrder),
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
                                                         m_numChannels (spec.numChannels),
                                                         m_sysDim(DspLine::Constants::kSystemOrder),
                                                         m (m_sysDim),
                                                         n (m_sysDim),
                                                         r (m_sysDim),
                                                         timeSamples (spec.maximumBlockSize),
                                                         A (m_sysDim, m_sysDim),
                                                         B (m_sysDim, 1),
                                                         C (1, m_sysDim),
                                                         D (1, 1),
                                                         timeRowVector (1, m_sysDim)
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
//void SateSpaceModelSimulation::prepareEverything(juce::AudioBuffer<float>& buffer);
juce::AudioBuffer<float> StateSpaceModelSimulation::retrieveSystemResultBuffer()
{
    return this->m_outputBuffer;
}
//set x0  ===================================================
void StateSpaceModelSimulation::setBufferInitialState (juce::AudioBuffer<float>& rawBuffer)
{
    //creates x0 where dim ~ [0:sysDim)
    //transform initialBuffer to matrices vector where v[ch] == corresponding data

    auto numSamples = rawBuffer.getNumSamples();
    auto numChannels = rawBuffer.getNumChannels();
    auto dim = DspLine::Constants::kSystemOrder;

    auto augumentedBuffer = juce::AudioBuffer<float>{numChannels, numSamples + DspLine::Constants::kSystemOrder};

    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* rawSample = rawBuffer.getReadPointer(ch);
        float* augSample = augumentedBuffer.getWritePointer(ch);

        for(int i = 0; i<dim;++i)
        {
            augSample[i] = rawSample[i];
        }
        for(int i = dim; i < dim + numSamples ; ++i)
        {
            augSample[i] = rawSample[i - dim];
        }
    }
}
//  ===================================================



void StateSpaceModelSimulation::setSystemSize (size_t systemSize) noexcept
{
    this->m_sysDim = systemSize;
}

void StateSpaceModelSimulation::setNumChannels (size_t channels) noexcept
{
    this->m_numChannels = channels;
}

//must come after `setEverything` :: buffer here should NOT contain init state data.
void StateSpaceModelSimulation::processBlock (juce::AudioBuffer<float>& buffer)
{
   //auto sysDim = DspLine::Constants::kSystemOrder;
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
   juce::AudioBuffer<float> outputBuffer;
   outputBuffer.makeCopyOf(buffer);

    for(auto ch = 0 ; ch < numChannels ; ++ch)
    {
        juce::AudioBuffer<float> bufferMono(1, numSamples);
        bufferMono.copyFrom(1, 0, buffer, ch, 0, numSamples);
        set_x0 (bufferMono, ch);
        set_u (bufferMono, ch);
        auto simulatedBufferMono  = runSimulation(bufferMono, ch);

        auto monoOut = simulatedBufferMono.getWritePointer(0);
        auto out = outputBuffer.getWritePointer(ch);

        for(auto t = 0; t<numSamples;++t)
        {
            out[t] = monoOut[t];
        }
    }//end ch iter

    //class member output buffer set
    m_outputBuffer = std::move(outputBuffer);
}


void StateSpaceModelSimulation::setMatrices (std::vector<mat> matrixSupplierVector, int sysDim)
{
    jassert(this->m_sysDim == sysDim);
    auto& v = matrixSupplierVector;
    this->A = v.at(0);
    this->B = v.at(1);
    this->C = v.at(2);
    this->D = v.at(3);
}

//this will logically require buffers form heap.
void StateSpaceModelSimulation::set_x0 (juce::AudioBuffer<float>& buffer, int channel)
{
    std::vector<mat> x0_multi;
    for (int ch = 0; ch < m_numChannels; ++ch)
    {
        mat x0_mono (m_sysDim, 1);
        const float* in = buffer.getReadPointer (channel);
        for (size_t i = 0; i < m_sysDim; i++)
        {
            x0_mono (i, 0) = in[i];
        }
        x0_multi.push_back (x0_mono);
    }

    this->x0 = x0_multi;
}

//set u from audioBuffer<float> : set u[n] : set inputSequence =====================================
void StateSpaceModelSimulation::set_u (juce::AudioBuffer<float>& buffer, int channel)
{
    auto numSamples = buffer.getNumSamples();
    //jassert (buffer.getNumChannels() == m_numChannels);

    std::vector<mat> u_multi;

   const float* in = buffer.getReadPointer (channel);
    mat u_inputTrain (1, numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        u_inputTrain (0, i) = in[i];
    }
    u_multi.push_back (u_inputTrain);


    this->u = u_multi;
}

//run matrix operation with given four matrices =====================================
juce::AudioBuffer<float> StateSpaceModelSimulation::runSimulation(juce::AudioBuffer<float>& bufferMono, int channel)
{

    //output matrix to store result. each matrix has collumns for time steps. 1st row should be the buffer
        auto& x_o = x_sim.at (channel); //this was done from heap to mat by set_x0 fn.
        auto& y_o = y_sim.at (channel); //this will be used for generating reeturning buffer
    //input matrix U to excite the system.
        set_x0( bufferMono, channel);
        set_u(bufferMono, channel);


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
    //entire buffer excluding first 4
    //getSimulatedOutput...() will use the y_sim to generate "bufferOut"
    auto bufferOut = getSimulatedOutputBufferFromMatrixMono(channel);
    return bufferOut;
}//run simulation End
//==============================================================================

juce::AudioBuffer<float> StateSpaceModelSimulation::getSimulatedOutputBufferFromMatrixMono(int channel)
{   //output will replace using result::makeCopyOf(buffer_copy)
    auto numSamples = static_cast<int>(y_sim.at(0).getNumColumns());
    auto numChannels = static_cast<int>(y_sim.size());
    auto bufferOut = juce::AudioBuffer<float>(numChannels, numSamples);


    float* in = bufferOut.getWritePointer(channel);
    auto y_sim_mono = y_sim.at(channel);
    for (int t = 0; t < numSamples; ++t)
    {
        in[t] = y_sim_mono(0,t);
    }

    return bufferOut;
}
////=======================================================================
