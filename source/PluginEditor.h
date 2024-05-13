
#pragma once
#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "LevelMeter.h"
//==============================================================================
using APVTS = juce::AudioProcessorValueTreeState;
struct ControlParameters
{
    ControlParameters(APVTS& state, juce::String paramID): attachment(state, paramID, slider){}
    juce::Slider slider;
    APVTS::SliderAttachment attachment;
};


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
    void sliderValueChanged(juce::Slider* sliderLastChanged) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& m_audioProcessor;
    PluginProcessor& getProcessor() const
    {
        return static_cast<PluginProcessor&> (m_audioProcessor);
    }

    //controllable parameters
    juce::Slider sliderGain;
    std::unique_ptr<APVTS::SliderAttachment>sliderGainAttachment;

    //Gui components
    Gui::BarMeterComponent barMeterComponent; //everything that "moves"
    Face::FacePlateGui facePlateGui; //replacing the opaque background : default skin

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
