//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent ():
                                             horizontalMeterBar0(0),
                                             horizontalMeterBar1(1),
                                             channelInfoTextBox0(),
                                             channelInfoTextBox1(),
                                             tinyStripComponent()
    {
        useInternalTiming(useInternalTimer);
    }

    BarMeterComponent::~BarMeterComponent() = default;

    //===================================================================

    void BarMeterComponent::paint(juce::Graphics &g)
    {
        tinyStripComponent.paint(g);
        horizontalMeterBar0.drawMeter(g, meterColours);
        horizontalMeterBar1.drawMeter(g, meterColours);
        channelInfoTextBox0.paint(g);
        channelInfoTextBox1.paint(g);
    }
    //===================================================================
    //===================================================================
    //===================================================================
    //===================================================================

    void BarMeterComponent::setInputMeterLevelValueDecibels (const std::vector<float>& values)
    {
        m_levelValues = values;
        updateMeters(); //update bars' length

        //repaint everything
        //repaint();

    }
    //===================================================================
    BarMeterComponent::BarMeterComponent (const juce::AudioChannelSet& channelFormat)
    {
        m_channelFormat = channelFormat;
    }
    //===================================================================
    void BarMeterComponent::reset()
    {
        m_channelFormat = juce::AudioChannelSet::stereo();
        refresh(true);
    }
    //===================================================================
    void BarMeterComponent::updateMeters()
    {
        //update meter value-tracking width
        horizontalMeterBar0.updateBarFigure (m_levelValues.at(0));
        horizontalMeterBar1.updateBarFigure (m_levelValues.at(1));
        //needs repaint globally
    }
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
        const auto numChannelNames = static_cast<int> (channelNames.size());
        const auto numMeters = static_cast<int> (Gui::Constants::kChannelSize);
        auto defaultMeterWidth = static_cast<float> (Constants::kMeterBarWidth);

        channelInfoTextBox0.setChannelName(channelNames[static_cast<size_t>(0)]);
        channelInfoTextBox1.setChannelName(channelNames[static_cast<size_t>(1)]);
    }
    //===================================================================
    void BarMeterComponent::setRefreshRate (float refreshRate)
    {
        meterOptions.refreshRateHz = refreshRate;
    }
    //===================================================================
    void BarMeterComponent::useInternalTiming (bool useInternalTiming) noexcept
    {
        useInternalTimer = useInternalTiming;
        stopTimer();
        if(useInternalTiming)
            startTimerHz(static_cast<int>(std::round(meterOptions.refreshRateHz)));
    }
    //===================================================================
    void BarMeterComponent::setColours()
    {
        m_backgroundColour = meterColours.colourBackground;
    }
    //===================================================================
    void BarMeterComponent::timerCallback()
    {
    //    updateBarFigure(useInternalTimer);
    /*
     * In the timerCallback function,
     * you're resetting the meters and then setting the level values again.
     * This approach is inefficient and can cause flickering on the UI.
     * Instead, you should directly update the meter values
     * based on the incoming audio levels in this callback.
     */
        //resetMeters();
        horizontalMeterBar0.updateBarFigure(10.f * M_RANDOMVALUEFORDEBUGGING);
        horizontalMeterBar1.updateBarFigure( 12.f *M_RANDOMVALUEFORDEBUGGING);

        tinyStripComponent.setNumericValue(M_RANDOMVALUEFORDEBUGGING );
        repaint();
    }
    //===================================================================
    void BarMeterComponent::refresh (bool forceRefresh) //global updating function
    {
        //meterColours.updateBarFigure();
//        if (!forceRefresh)
//            return;
        updateMeters();
        resetPeakHold();
    }
} // Gui