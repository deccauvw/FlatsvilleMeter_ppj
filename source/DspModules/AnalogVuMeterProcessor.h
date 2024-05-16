#pragma once

#include "juce_dsp/juce_dsp.h"
#include <vector>
#include "StateSpaceModelSimulation.h"
#include "DspModulesHelper.h"
//==============================================================================
class AnalogVuMeterProcessor  : public juce::Component//, private DspLine::SystemMatrices
{
    using mat = juce::dsp::Matrix<float>;

public:
    AnalogVuMeterProcessor();
    AnalogVuMeterProcessor(juce::dsp::ProcessSpec spec);
    ~AnalogVuMeterProcessor() override;
    //JUCE functions=========================
    void prepareToPlay(double  SampleRate, int numberOfInputChannels, int estimatedSamplesPerBlock);
    void processBlock(juce::AudioBuffer<float> &buffer);


    std::vector<float> getVuLevelValue();


private:
    //system dimension = 4
    const int m_sysDim = static_cast<int>(DspLine::Constants::kSystemOrder);
    //systemMatricesInit
    DspLine::SystemMatrices m_systemMatrices;

    //state space model simulation classes for stereo
    //StateSpaceModelSimulation m_ssms;
//    StateSpaceModelSimulation m_ssms_v2i; //for n-channels
//    StateSpaceModelSimulation m_ssms_i2a; //for n-channels
    std::unique_ptr<StateSpaceModelSimulation> m_ssms_v2i;
    std::unique_ptr<StateSpaceModelSimulation> m_ssms_i2a;

    //memory for System II from system I
    juce::HeapBlock<float> bufferPreSysI;
    juce::HeapBlock<float> bufferPreSysII;
    int _heapIdx(int idx, int channel){return systemOrder * channel + idx;}

    //member functions=========================
    juce::AudioBuffer<float> generateAugmentedBufferWithHeapBlock(juce::AudioBuffer<float>& buffer, juce::HeapBlock<float>& h);
    juce::AudioBuffer<float> feedToSteadyStateModel(StateSpaceModelSimulation& ssms, juce::AudioBuffer<float>& rawBuffer,  juce::HeapBlock<float>& h);

    //static void recordHeapForNextSystem(juce::AudioBuffer<float>& buffer, juce::HeapBlock<float> &z);

    //==============================================================================
    int systemOrder;
    juce::AudioBuffer<float> m_bufferProcessResult; //every last buffer result will be stored in this location refreshed.
    std::vector<float> m_levelValuesVu = {0.0f, 0.0f};
    juce::dsp::ProcessSpec m_spec; //sample rate etc.
   //std::vector<float> m_needlePointsValuesVector; //channel-wise values vector

    static constexpr float m_minimalReturnLevelDecibels = DspLine::Constants::kMinimalReturnValue; // virtual -INF


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogVuMeterProcessor)
};

//#endif //__ANALOG_VU_METER_PROCESSOR__