#pragma once

#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

#include "LevelMeter.h"
#include "Components/HorizontalBarMetersComponent.h"
#include "DspModules/AnalogVuMeterProcessor.h"
//#include "Components/HorizontalBarMeter.h"


//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void setChannelFormat(juce::AudioChannelSet metersFormat);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    AudioProcessor& m_audioProcessor;
    LevelMeter::HorizontalBarMetersComponent m_inputMeters;
    //juce::ResizableCornerComponent m_cornerResizer;
    juce::ComponentBoundsConstrainer m_sizeConstrainer;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
