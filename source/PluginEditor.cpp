#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1012, 230);
    juce::ignoreUnused (m_audioProcessor);
    const int refreshRateHz = 30;
        //set the meter options

    LevelMeter::Options meterOptions;
    //meterOptions.faderEnabled = true;
    meterOptions.headerEnabled = false;
    meterOptions.valueEnabled = false;
    meterOptions.refreshRateHz = refreshRateHz;
    meterOptions.useGradient = true;
    meterOptions.showPeakHoldIndicator = false;
    meterOptions.tickMarksEnabled = true;
    meterOptions.tickMarksOnTopOfTheBar = true;
    meterOptions.tickMarksInDecibels = {3.0f, 2.0f, 1.0f, 0.0f, -1.0f, -2.0f, -3.0f, -5.0f, -7.0f, -10.0f, -20.0f}; //from Helpers
    meterOptions.decayTimeMs = 1000.0f;

    //set the meter's segment options...
    m_inputMeters.setOptions(meterOptions);

    juce::Colour coloursForMeterRange_at20 = juce::Colours::steelblue; //colour of the bar under zero dB cross line
    juce::Colour coloursForMeterRange_at0 = juce::Colours::aliceblue; //colour of the bar under zero dB cross line
    juce::Colour coloursForMeterRange_at3 = juce::Colours::orange; //colour of the bar over zero dB cross line
    std::vector<LevelMeter::SegmentOptions> segmentOptions =
                                                            {{{-20.0f, 0.0f}, {0.0f, 0.8696f}, coloursForMeterRange_at20, coloursForMeterRange_at0},
                                                            {{0.0f, 3.0f}, {0.8696f, 1.0f}, coloursForMeterRange_at0, coloursForMeterRange_at3}};

    //use or don't use the label strip in between two meters
    m_inputMeters.setLabelStripPosition(LevelMeter::LabelStripPosition::upper);
    m_inputMeters.setChannelFormat(p.getOutputFormat());
    addAndMakeVisible(m_inputMeters);


    startTimer(static_cast<int>(std::round((1000.0f / refreshRateHz))));
}//constructor

PluginEditor::~PluginEditor() =default;

//void PluginEditor::paint (juce::Graphics& g)
//{
//    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    auto area = getLocalBounds();
//    g.setColour (juce::Colours::white);
//    g.setFont (16.0f);
//    auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
//    g.drawText (helloWorld, area.removeFromTop (150), juce::Justification::centred, false);
//}

void PluginEditor::resized()
{
//    // layout the positions of your child components here
//    m_inputMeters.setBounds(getLocalBounds());
//    //const int cornerResizerSize = 15;
}


void PluginEditor::hiResTimerCallback()
{
    for(int meterIndex = 0; meterIndex < m_inputMeters.getNumChannels(); ++meterIndex)
    {
        //get the level, of the specified meter[ch] from the audio processor.
        auto audioLevelPeak = m_audioProcessor.getPeakLevel (meterIndex);
        //auto audioLevelVu = m_audioProcessor.getVuLevel (meterIndex);
        m_inputMeters.setInputLevel (meterIndex, audioLevelPeak);
        m_inputMeters.refresh();
    }
}