
#pragma once

#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"

#include "LevelMeter.h"
//==============================================================================
class PluginEditor :
    public juce::AudioProcessorEditor, //has component class included
    private juce::HighResolutionTimer
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    //void paint (juce::Graphics&) override;
    void resized() override;

    void setChannelFormat(const juce::AudioChannelSet& metersFormat)
    {
        m_inputMeters.setChannelFormat(metersFormat);
    }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& m_audioProcessor;
    LevelMeter::HorizontalBarMetersComponent m_inputMeters;
    //Field type 'LevelMeter::HorizontalBarMetersComponent' is an abstract class
    juce::ComponentBoundsConstrainer m_sizeConstrainer;
    //juce::ResizableCornerComponent m_cornerResizer;

    void hiResTimerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
