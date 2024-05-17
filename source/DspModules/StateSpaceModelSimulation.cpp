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
void StateSpaceModelSimulation::processBlock (juce::AudioBuffer<float>& refBuffer)
{
   //auto sysDim = DspLine::Constants::kSystemOrder;
    int numChannels = refBuffer.getNumChannels();
    int numSamples = refBuffer.getNumSamples();

   juce::AudioBuffer<float> outputBuffer;
   outputBuffer.setSize(numChannels, numSamples - m_sysDim);
   outputBuffer.clear();


   //channel wise matrix to buffer
    for(auto ch = 0 ; ch < numChannels ; ++ch)
    {
        juce::AudioBuffer<float> bufferMono(1, numSamples);

        auto simulatedBufferMono  = runSimulation(ch);

        auto monoOut = simulatedBufferMono.getWritePointer(ch);
        auto out = outputBuffer.getWritePointer(ch);

        for(auto t = 0; t < numSamples;++t)
        {

            out[t] = monoOut[t];
        }
    }//end channel wise iter


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
    auto numChannels = buffer.getNumChannels();


    mat x0_mono (m_sysDim, 1);
    const float* in = buffer.getReadPointer (channel);

    for (size_t i = 0; i < m_sysDim; i++)
    {
        x0_mono (i, 0) = in[i];
    }
    x0_multi.push_back (x0_mono);

    this->x0 = x0_multi;
}

//set u from audioBuffer<float> : set u[n] : set inputSequence =====================================
void StateSpaceModelSimulation::set_u (juce::AudioBuffer<float>& buffer, int channel)
{
    int numSamples = buffer.getNumSamples(); //blocksize + sysorder

    std::vector<mat> u_multi;

    const float* in = buffer.getReadPointer (channel);

    for (int i = m_sysDim; i < numSamples; ++i) // in[] :: m_sysdim(4) to "End + m_sysDim"
    {
        mat u_inputTrain(1, 1);
        u_inputTrain (0, 0) = in[i];
        u_multi.push_back(u_inputTrain);
    }


    this->u = u_multi;

}

//run matrix operation with given four matrices ===================================
//outputs mono buffer
juce::AudioBuffer<float> StateSpaceModelSimulation::runSimulation(int channel)
{
    //output matrix to store result. each matrix has collumns for time steps. 1st row should be the buffer

    //init state matrix
    //do we have a stereo pair of these? probably not
    // because we initialize beforehand everything right off the bat after channel changes.
    auto X0 = x0.at(channel); //this was done from heap to mat by set_x0 fn.
    //auto U = u.at (channel); //this will be used for generating reeturning buffer
    auto nTimeStep = u.size();

    //diomension of Y must be : (1, nTimestep)

    mat X(m_sysDim, nTimeStep);
    mat Y(1, nTimeStep);
    X.clear();
    Y.clear();

        //loop under

    //input matrix U to excite the system.

    for (int t = 0; t < nTimeStep; t++) //t is the "time" index. : u will iterate
    {
        if (t == 0)
        {
            for (int row = 0; row < m_sysDim; ++row)
                X(row, t) = X0 (row, t);
            Y(0, t) = (C * X0) (0, 0);
        }
        else // t > 0
        {
            float dpx = 0.0;
            for (int i = 0; i < m_sysDim; ++i)
            {
                for (int j = 0; j < m_sysDim; ++j)
                    dpx += A (i, j) * X(i, t-1) + B(i, 0) * u[t-1](0, 0);

                X(i, t) = dpx;
            }

            float dpy = 0.0;
            for (int k = 0; k < m_sysDim; ++k)
            {
                dpy += C (0, k) * X (k, t) +
                       D (0, 0) * u[t](0, 0);
            }
            Y (0, t) = dpy;
        }
    } //end of timestep


    auto bufferOut = getSimulatedOutputBufferFromMatrixMono(channel);
    return bufferOut;
}//run simulation End
//==============================================================================

juce::AudioBuffer<float> StateSpaceModelSimulation::getSimulatedOutputBufferFromMatrixMono(int channel)
{   //output will replace using result::makeCopyOf(buffer_copy)
    int numSamples = static_cast<int>(this->u.size());
    int numChannels = m_numChannels;
    auto bufferOut = juce::AudioBuffer<float>(numChannels, numSamples);
    bufferOut.clear();

    float* in = bufferOut.getWritePointer(channel);

    for(int t = 0;t<numSamples;++t)
    {
        in[t] = u[t](0,0);
    }


    return bufferOut;
}

////=======================================================================
