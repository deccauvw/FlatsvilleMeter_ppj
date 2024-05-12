
#pragma once
#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "LevelMeter.h"
//==============================================================================
class PluginEditor :
    public juce::AudioProcessorEditor, //has component class included
    private juce::Timer,
    public juce::Slider::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueUpdated(juce::Slider* sliderLastUpdated);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& m_audioProcessor;
    PluginProcessor& getProcessor() const
    {
        return static_cast<PluginProcessor&> (m_audioProcessor);
    }

    using APVTS = juce::AudioProcessorValueTreeState;


    //Gui components
    Gui::BarMeterComponent barMeterComponent; //everything that "moves"
    Face::FacePlateGui facePlateGui; //replacing the opaque background : default skin

    //int timerCallbackCount = 0;
    //bool forceRefresh = true;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
