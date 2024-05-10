//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent (PluginProcessor& p): audioProcessor(p),
                                             horizontalMeterBar0(0, [&](){return audioProcessor.getLevelValuePeak(0);}),
                                             horizontalMeterBar1(1, [&](){return audioProcessor.getLevelValuePeak(1);}),
                                             channelInfoTextBox0(0, [&](){return audioProcessor.getLevelValuePeak(0);}),
                                             channelInfoTextBox1(1, [&](){return audioProcessor.getLevelValuePeak(1);}),
                                             tinyStripComponent([&](){return juce::String("holyshit");}),
                                                                useInternalTimer(true)
    {
        //useInternalTiming(useInternalTimer); //startTimerHz
        //setChannelFormat()
        addAndMakeVisible(tinyStripComponent);
        addAndMakeVisible(horizontalMeterBar0);
        addAndMakeVisible(horizontalMeterBar1);
        addAndMakeVisible(channelInfoTextBox0);
        addAndMakeVisible(channelInfoTextBox1);
    }

    BarMeterComponent::~BarMeterComponent()
    {
        stopTimer();
    }

    //===================================================================

    void BarMeterComponent::paint(juce::Graphics &g)
    {
        drawEverything(g);
    }

    void BarMeterComponent::addAndMakeVisibleEverything()
    {
        addAndMakeVisible(horizontalMeterBar0);
        addAndMakeVisible(horizontalMeterBar1);
        addAndMakeVisible(channelInfoTextBox0);
        addAndMakeVisible(channelInfoTextBox1);
        addAndMakeVisible(tinyStripComponent);
    }
    std::vector<juce::Component*> BarMeterComponent::addAndMakeVisibleEverythingThrower()
    {
        std::vector<juce::Component*> listOfComponent = {&horizontalMeterBar0, &horizontalMeterBar1, &channelInfoTextBox0, &channelInfoTextBox1, &tinyStripComponent};
        return listOfComponent;
    }

    void BarMeterComponent::drawEverything(juce::Graphics &g)
    {
        horizontalMeterBar0.paint(g);
        horizontalMeterBar1.paint(g);
        channelInfoTextBox0.paint(g);
        channelInfoTextBox1.paint(g);
        tinyStripComponent.paint(g);
    }
    void BarMeterComponent::updateEverything()
    {
        //    updateBarFigure(useInternalTimer);
        /*
     * In the timerCallback function,
     * you're resetting the meters and then setting the level values again.
     * This approach is inefficient and can cause flickering on the UI.
     * Instead, you should directly update the meter values
     * based on the incoming audio levels in this callback.
     */
        //setLevelValues(...); //done outside the scope
//        printf("BarMeterComponent::timerCallback called::");
//        printf("::%3f\t%3f\n", m_levelValues.at(0), m_levelValues.at(1));

        horizontalMeterBar0.updateBarFigure(juce::Decibels::gainToDecibels(m_levelValues.at(0)));
        horizontalMeterBar1.updateBarFigure(juce::Decibels::gainToDecibels(m_levelValues.at(1)));

        channelInfoTextBox0.setChannelName(juce::String("L"));
        channelInfoTextBox1.setChannelName(juce::String("R"));
//        const auto numChannelNames = static_cast<int> (channelNames.size());
//        const auto numMeters = static_cast<int> (Gui::Constants::kChannelSize);
//        auto defaultMeterWidth = static_cast<float> (Constants::kMeterBarWidth);
//
//        channelInfoTextBox0.setChannelName(channelNames[static_cast<size_t>(0)]);
//        channelInfoTextBox1.setChannelName(channelNames[static_cast<size_t>(1)]);
//

        tinyStripComponent.setStringContent(juce::String(M_RANDOMVALUEFORDEBUGGING));
        M_RANDOMVALUEFORDEBUGGING++;

    }
    void BarMeterComponent::repaintEverything()
    {
        horizontalMeterBar0.repaint();
        horizontalMeterBar1.repaint();
        channelInfoTextBox0.repaint();
        channelInfoTextBox1.repaint();
        tinyStripComponent.repaint();
    }
    //===================================================================
    //===================================================================
    //===================================================================
    //===================================================================

    void BarMeterComponent::setInputMeterLevelValueDecibels (const std::vector<float>& values)
    {
        m_levelValues = values;

        //repaint everything
        //repaint();

    }
    //===================================================================
    void BarMeterComponent::reset()
    {
        m_channelFormat = juce::AudioChannelSet::stereo();
    }
    //===================================================================
    //===================================================================
    void BarMeterComponent::resetPeakHold()
    {
        horizontalMeterBar0.resetPeakHold();
        horizontalMeterBar1.resetPeakHold();
    }
    //===================================================================
    void BarMeterComponent::setChannelFormat (const juce::AudioChannelSet& channelSet, const std::vector<juce::String>& channelNames)
    {
        if(channelSet.size() <= 0)
            return;
        m_channelFormat = channelSet;

        if(channelSet.size() != Gui::Constants::kChannelSize)
        {
            jassert("channelSize is not eq. to Stereo In Stereo Out");
        }
        setChannelNames(channelNames);
    }
    //===================================================================
    void BarMeterComponent::setChannelNames (const std::vector<juce::String>& channelNames)
    {

    }
    //===================================================================
    void BarMeterComponent::setRefreshRate (float refreshRate)
    {
        meterOptions.refreshRateHz = refreshRate;
    }
    //===================================================================
//    void BarMeterComponent::useInternalTiming (bool useInternalTiming) noexcept
//    {
//        useInternalTimer = useInternalTiming;
//        if (useInternalTiming)
//        {
//            //stopTimer();
//            startTimerHz (static_cast<int> (std::round (meterOptions.refreshRateHz)));
//            printf("using internal timing...\n");
//        }
//    }
    //===================================================================
    void BarMeterComponent::setColours()
    {
        m_backgroundColour = meterColours.colourBackground;
    }
    //===================================================================
    void BarMeterComponent::timerCallback()
    {
        updateEverything();
        repaintEverything();
    }
    //===================================================================
    void BarMeterComponent::setLevelValues(std::vector<float>& levelValues) //acquire value from audioprocessor.
    {
        jassert(m_levelValues.size() == levelValues.size());
        m_levelValues = levelValues;
    }
} // Gui