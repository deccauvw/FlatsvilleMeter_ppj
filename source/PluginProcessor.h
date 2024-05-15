
#if (MSVC)
#include "ipps.h"
#endif
//
//#ifndef LEVELMETER_H
//#define LEVELMETER_H
//#endif

#pragma once
#include<juce_audio_processors/juce_audio_processors.h>
#include<juce_dsp/juce_dsp.h>
#include "LevelMeter.h"
#include "DspModules/DspModulesHelper.h"

#if (MSVC)
#include "ipps.h"
#endif

//==============================================================================
class AnalogVuMeterProcessor;

struct Parameters
{
    float param_gain;
    int param_meterBallisticsType;
    Parameters(): param_gain(0.0f), param_meterBallisticsType(3){}

};

using APVTS = juce::AudioProcessorValueTreeState;
//==============================================================================


class PluginProcessor : public juce::AudioProcessor,
                        public juce::AudioProcessorParameter::Listener,
                        public Parameters
{
    public:

    PluginProcessor();
    ~PluginProcessor() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
    //added member fns>>>

    //static std::unique_ptr<AnalogVuMeterProcessor> m_vuMeterDspProcessorPtr;
    AnalogVuMeterProcessor m_vuMeterDspProcessor;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool starting) override;

    std::vector<float> m_nChannelRmsLevels {};
    std::vector<float> m_nChannelPeakLevels {};
    std::vector<float> m_nChannelVuLevels {};
    float m_RmsLevelChannel0 = 0.0;
    float m_RmsLevelChannel1= 0.0;
    float m_peakLevelChannel0 = 0.0;
    float m_peakLevelChannel1 = 0.0;
    float m_vuLevelChannel0 = 0.0f;
    float m_vuLevelChannel1 = 0.0f;


    float getLevelValueRms (int channel) const;
    float getLevelValuePeak (int channel) const;
    float getLevelValueVu(int  channel) const;

    APVTS apvts;
    Parameters parameters;

private:
    APVTS::ParameterLayout createParameters();

    bool isBufferEmpty(const juce::AudioBuffer<float>& buffer);
    juce::AudioBuffer<float> bufferForMeter;


    float levelRmsLeft = 0.0f;
    float levelRmsRight = 0.0f;
    juce::dsp::ProcessSpec specs;
    int maximumEstimatedSamplesPerBlock;
    juce::AudioChannelSet m_outputFormat;





//    AnalogVuMeterProcessor vuMeterProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

