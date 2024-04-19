
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
//#include "DspModules/AnalogVuMeterProcessor.h"
#if (MSVC)
#include "ipps.h"
#endif

//==============================================================================


class PluginProcessor : public juce::AudioProcessor, public::juce::AudioProcessorParameter::Listener
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
    enum Parameters
    {
        BLANK
    };
    static constexpr int n_params = 1;
    std::array<juce::AudioParameterFloat*, n_params> params;
    //this is where defining things that go into plugins interface is done.

    struct UiToAudioMessage
    {
        enum What
        {
            NEW_VALUE,
            BEGIN_EDIT,
            END_EDIT
        } what{NEW_VALUE};
        Parameters which;
        float newValue = 0.0;
    };
    //this is things the interface can tell the audio thead about.

    struct AudioToUiMessage
    {
        enum What {
            NEW_VALUE,
            RMS_LEFT,
            RMS_RIGHT,
            PEAK_LEFT,
            PEAK_RIGHT,
            SLEW_LEFT,
            SLEW_RIGHT,
            ZERO_LEFT,
            ZERO_RIGHT,
            VU_LEFT,
            VU_RIGHT,
            INCREMENT
        } what{NEW_VALUE};
        Parameters which;
        float newValue = 0.0;
    };
    //This is kinds of info. the audio thread can give the interface.

    template <typename T, int qSize = 4096> class LockFreeQueue
    {
    public:
        LockFreeQueue() : af(qSize){}
        bool push(const T &ad)
        {
            auto ret = false;
            int start1, size1, start2, size2;
            af.prepareToWrite (1, start1, size1, start2, size2);
            if (size1 > 0)
            {
                dq[start1] = ad;
                ret = true;
            }
            af.finishedWrite (size1 + size2);
            return ret;
        }

        bool pop(T &ad)
        {
            bool ret = false;
            int start1, size1, start2, size2;
            af.prepareToRead (1, start1, size1, start2, size2);
            if (size1 > 0)
            {
                ad = dq[start1];
                ret = true;
            }
            af.finishedRead (size1 + size2);
            return ret;
        }

        juce::AbstractFifo af;
        T dq[qSize];

    };

    LockFreeQueue<UiToAudioMessage> uiToAudio;
    LockFreeQueue<AudioToUiMessage> audioToUi;

    double rmsLeft = 0.0;
    double rmsRight = 0.0;
    double vuRight = 0.0;
    double vuLeft = 0.0;
    double peakRight = 0.0;
    double peakLeft = 0.0;
    double slewRight = 0.0;
    double slewLeft = 0.0;
    double previousRight = 0.0;
    double previousLeft = 0.0;
    double zeroLeft = 0.0;
    double zeroRight = 0.0;
    double longestZeroLeft = 0.0;
    double longestZeroRight = 0.0;
    bool wasPositiveL = false;
    bool wasPositiveR = false;
    int rmsCount = 0;



//    juce::AudioChannelSet getOutputFormat() const noexcept {return m_outputFormat;}
//    void setPeakLevel(int channelIndex, float peakLevel);
//    float getPeakLevel(int channelIndex);
//
//    void setVuLevel(int channelIndex, float vuLevel);
//    float getVuLevel(int channelIndex);

private:
    juce::dsp::ProcessSpec specs;
    int maximumEstimatedSamplesPerBlock;
    juce::AudioChannelSet m_outputFormat;
//    std::array<std::atomic<float>, 128> m_peakLevels; //each index is channel number : 128 max channel
//    std::array<std::atomic<float>, 128> m_vuLevels;

//    AnalogVuMeterProcessor vuMeterProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
