#pragma once

#include "juce_audio_processors/juce_audio_processors.h" //quasi juceheader
#include "juce_dsp/juce_dsp.h"
#include <vector>
#include "StateSpaceModelSimulation.h"
#include "DspModulesHelper.h"
//==============================================================================
class AnalogVuMeterProcessor  : public juce::Component, private DspLine::SystemMatrices
{
public:
    AnalogVuMeterProcessor();
    ~AnalogVuMeterProcessor() override;
    //JUCE functions=========================
    void prepareToPlay(double  SampleRate, int numberOfInputChannels, int estimatedSamplesPerBlock);
    void processBlock(juce::AudioBuffer<float> &buffer);

    //member functions=========================
    using mat = juce::dsp::Matrix<float>;

    void feedToSteadyStateEquation(juce::AudioBuffer<float>& buffer, int systemSize);
 
    void keepPreviousStateForNextInitSystemI();
    void keepPreviousStateForNextInitSystemII();



    void reset();

    juce::AudioBuffer<float> getOutputBuffer();


private:
    //system dimension = 4
    const size_t sysDim = SystemMatrices::systemDim;
    //systemMatricesInit
    DspLine::SystemMatrices systemMatrices;

    //state space model simulation classes for stereo
    StateSpaceModelSimulation ssms;
    StateSpaceModelSimulation ssms_v2i; //for n-channels
    StateSpaceModelSimulation ssms_i2a; //for n-channels

    //memory for System II from system I
    juce::HeapBlock<float> z1;
    juce::HeapBlock<float> z2;
    juce::HeapBlock<float> z3;
    juce::HeapBlock<float> z4;

    //memory for next block of System I from system II
    juce::HeapBlock<float> w1;
    juce::HeapBlock<float> w2;
    juce::HeapBlock<float> w3;
    juce::HeapBlock<float> w4;

    //==============================================================================

    juce::AudioBuffer<float> bufferForMeasurement;
    juce::AudioBuffer<float> outputBufferSytemI;
    juce::AudioBuffer<float> outputBufferSystemII;

    juce::AudioBuffer<float> initialStateBufferForSystemI; //previous 4 samples
    juce::AudioBuffer<float> initialStateBufferForSystemII; //previous 4 samples
    juce::dsp::ProcessSpec spec; //sample rate etc.
    std::vector<float> vuLevelsVector;

    static constexpr float minimalReturnLevelDecibels = DspLine::Constants::kMinimalReturnValue; // virtual -INF


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogVuMeterProcessor)
};

//#endif //__ANALOG_VU_METER_PROCESSOR__