
#if (MSVC)
#include "ipps.h"
#endif
//
//#ifndef LEVELMETER_H
//#define LEVELMETER_H
//#endif
#pragma once
#include<juce_audio_processors/juce_audio_processors.h>

//==============================================================================


class PluginProcessor : public juce::AudioProcessor
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
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //added member fns>>>
    juce::AudioChannelSet getOutputFormat() const noexcept {return m_outputFormat;}
    void setPeakLevel(int channelIndex, float peakLevel);
    float getPeakLevel(int channelIndex);

private:
    juce::AudioChannelSet m_outputFormat;
    std::array<std::atomic<float>, 128> m_peakLevels;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
