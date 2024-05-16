#pragma once

#include "juce_dsp/juce_dsp.h"
#include <vector>
#include "StateSpaceModelSimulation.h"
#include "DspModulesHelper.h"
//==============================================================================
class AnalogVuMeterProcessor  : public juce::Component//, private DspLine::SystemMatrices
{
public:
    AnalogVuMeterProcessor();
    AnalogVuMeterProcessor(juce::dsp::ProcessSpec spec);
    ~AnalogVuMeterProcessor() override;
    //JUCE functions=========================
    void prepareToPlay(double  SampleRate, int numberOfInputChannels, int estimatedSamplesPerBlock);
    void processBlock(juce::AudioBuffer<float> &buffer);

    //member functions=========================
    using mat = juce::dsp::Matrix<float>;
    void initializeBufferWithHeapBlock(juce::AudioBuffer<float>& buffer, juce::HeapBlock<float>& z);
    void feedToSteadyStateModel(StateSpaceModelSimulation& ssms,  juce::HeapBlock<float> &h);
 
    static void recordHeapForNextSystem(juce::AudioBuffer<float>& buffer, juce::HeapBlock<float> &z);

    void reset();

    juce::AudioBuffer<float> getOutputBuffer();


private:
    //system dimension = 4
    const int m_sysDim = static_cast<int>(DspLine::Constants::kSystemOrder);
    //systemMatricesInit
    DspLine::SystemMatrices m_systemMatrices;

    //state space model simulation classes for stereo
    StateSpaceModelSimulation m_ssms;
//    StateSpaceModelSimulation m_ssms_v2i; //for n-channels
//    StateSpaceModelSimulation m_ssms_i2a; //for n-channels
    std::unique_ptr<StateSpaceModelSimulation> m_ssms_v2i;
    std::unique_ptr<StateSpaceModelSimulation> m_ssms_i2a;

    //memory for System II from system I
    juce::HeapBlock<float> z1;
//    juce::HeapBlock<float> z2;
//    juce::HeapBlock<float> z3;
//    juce::HeapBlock<float> z4;

    //memory for next block of System I from system II
    juce::HeapBlock<float> w1;
//    juce::HeapBlock<float> w2;
//    juce::HeapBlock<float> w3;
//    juce::HeapBlock<float> w4;

    //==============================================================================
    int systemSize;
    juce::AudioBuffer<float> m_buffer; //modified extended buffer for simulation. (4 more samples)
    juce::AudioBuffer<float> m_outputBufferSystemI;
    juce::AudioBuffer<float> m_outputBufferSystemII;

    juce::AudioBuffer<float> m_initialStateBufferForSystemI; //previous 4 samples
    juce::AudioBuffer<float> m_initialStateBufferForSystemII; //previous 4 samples
    juce::dsp::ProcessSpec m_spec; //sample rate etc.
   //std::vector<float> m_needlePointsValuesVector; //channel-wise values vector

    static constexpr float m_minimalReturnLevelDecibels = DspLine::Constants::kMinimalReturnValue; // virtual -INF


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogVuMeterProcessor)
};

//#endif //__ANALOG_VU_METER_PROCESSOR__