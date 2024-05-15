
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "LevelMeter.h"
//==============================================================================
using APVTS = juce::AudioProcessorValueTreeState;
//using namespace juce;
struct ControlParameters
{
    ControlParameters(APVTS& state, juce::String paramID): attachment(state, paramID, slider){}
    juce::Slider slider;
    APVTS::SliderAttachment attachment;
};


//==============================================================================
class PluginEditor :
    public juce::AudioProcessor,
    public juce::AudioProcessorEditor, //has component class included
    private juce::Timer,
    public juce::Slider::Listener,
    public juce::ComboBox::Listener,
    private juce::ValueTree::Listener
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;
    //PluginProcessor* AudioProcessorEditor;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* sliderLastChanged) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& m_audioProcessor;
    Gui::Helpers helpers;
//    [[nodiscard]] PluginProcessor& getProcessor() const
//    {
//        return static_cast<PluginProcessor&> (m_audioProcessor);
//    }
    static int MeterBallisticsTypeKey; // the MBT key -> value [key]

    //controllable parameters
    std::unique_ptr<APVTS::SliderAttachment>sliderGainAttachment;
    juce::Slider sliderGain;
    juce::ComboBox comboBoxMeterType;
    juce::Label labelMeterType;
    //Gui components
    Gui::BarMeterComponent barMeterComponent; //everything that "moves"
    Face::FacePlateGui facePlateGui; //replacing the opaque background : default skin

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
