//
// Created by prmir on 2024-04-05.
//
#pragma once

#include "HorizontalBarMetersComponent.h"

namespace LevelMeter
{
    HorizontalBarMetersComponent::HorizontalBarMetersComponent() : HorizontalBarMetersComponent({}, {})
    {
    }

    HorizontalBarMetersComponent::HorizontalBarMetersComponent (const Options& meterOptions) : HorizontalBarMetersComponent(meterOptions, {})
    {
    }

    HorizontalBarMetersComponent::HorizontalBarMetersComponent (const juce::AudioChannelSet& channelFormat) : HorizontalBarMetersComponent({}, channelFormat)
    {
    }

    HorizontalBarMetersComponent::HorizontalBarMetersComponent
        (const Options& meterOptions, const juce::AudioChannelSet& channelFormat) :
                                                                               meters_meterOptions(meterOptions),
                                                                               meters_labelStrip(meterOptions, Constants::kLabelstripId, juce::AudioChannelSet::ChannelType::unknown),
                                                                               meters_channelFormat(channelFormat)
    {
        setName(Constants::kMetersId);
        addAndMakeVisible(meters_labelStrip);
        startTimer(static_cast<int>(std::round((1000.0f / meters_meterOptions.refreshRateHz))));
        createMeters(channelFormat, {});
    }

    HorizontalBarMetersComponent::~HorizontalBarMetersComponent()
    {
        deleteMeters();
    }
//  ====================================================================================================================================
    void HorizontalBarMetersComponent::refresh (bool forceRefresh)
    {
        if(!isShowing() || meters_meterChannels.isEmpty())
            return;

        meters_labelStrip.refresh(forceRefresh);
        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->refresh(forceRefresh);
    }

    void HorizontalBarMetersComponent::reset()
    {
        deleteMeters();
        meters_labelStrip.showTickMarks(false);
        meters_channelFormat = juce::AudioChannelSet::stereo();
        refresh(true);
    }

    void HorizontalBarMetersComponent::resetMeters()
    {
        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->reset();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::clearMeters()
    {
        for(auto* meter : meters_meterChannels)
            if (meter)
                meter->setInputLevel(0.0f);
        refresh(true);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::createMeters (const juce::AudioChannelSet& channelFormat, const std::vector<juce::String>& channelNames)
    {
        //create enough meters to match the channel format
        for(int channelIdx = 0; channelIdx < channelFormat.size(); ++channelIdx)
        {
            auto meterChannel = std::make_unique<HorizontalBarMeterChannel> (meters_meterOptions, Constants::kMetersId, channelFormat.getTypeOfChannel (channelIdx));
            meterChannel->setFont (meters_font);
            //meterChannel->addMouseListener(this, true);
            addChildComponent (meterChannel.get());
            meters_meterChannels.add (meterChannel.release());
            //meters_labelStrip.setactive (true);
        }

        setChannelNames(channelNames);
        setMeterSegments(meters_segmentsOptions);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::resetPeakHold()
    {
        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->resetPeakHold();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setInputLevel (int channel, float value)
    {
        if (auto* meterChannel = getMeterChannel((channel)))
            if (meterChannel)
                meterChannel->setInputLevel(value);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setNumChannels (int numChannels, const std::vector<juce::String>& channelNames /* = {};*/)
    {
        if(numChannels <= 0)
            return;
        setChannelFormat(juce::AudioChannelSet::canonicalChannelSet(numChannels), channelNames);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setChannelFormat (const juce::AudioChannelSet& channels, const std::vector<juce::String>& channelNames)
    {
        if(channels.size() <= 0)
            return;

        meters_channelFormat = channels;
        //make sure the number of meters matches the num. of channels
        if(channels.size() != meters_meterChannels.size())
        {
            deleteMeters(); //ifnot then del. all prev. meters
            createMeters(channels, channelNames); // and create new ones. matching the req. channel format
        }
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setChannelNames (const std::vector<juce::String>& channelNames)
    {
        if(meters_meterChannels.isEmpty())
            return;

        const auto numChannelNames = static_cast<int> (channelNames.size());
        const auto numMeters = static_cast<int> (meters_meterChannels.size());
        auto defaultMeterWidth = static_cast<float> (Constants::kMeterBarWidth);

        //loop through all meters
        for(int meterIdx = 0; meterIdx < numMeters; ++meterIdx)
        {
            if(meterIdx < numChannelNames)
            {
                if(channelNames[static_cast<size_t>(meterIdx)].isNotEmpty())
                {
                    meters_meterChannels[meterIdx]->setChannelName(channelNames[static_cast<size_t>(meterIdx)]);
                    //calc. the meter width so it fits the largest of channelNames
                    defaultMeterWidth = std::max(defaultMeterWidth, meters_meterChannels[meterIdx]->getChannelNameWidth());
                }
            }
            else
            {
                defaultMeterWidth = std::max(defaultMeterWidth, meters_meterChannels[meterIdx]->getChannelTypeWidth());
            }
        }

        if(channelNames.empty())
        {
            for(auto* meter:meters_meterChannels)
                if(meter)
                    meter->setReferredTypeWidth(defaultMeterWidth);
        }
        //calc. default mixer width
        //this is the width @ wich all channel names can be displayed
        meters_autoSizedPanelWidth = static_cast<int> (defaultMeterWidth * static_cast<float>(numMeters));//min. width need for ch. names
//        m_autoSizedPanelWidth += numMeters * (2 * Constants::kFaderRightPadding);  // Add the padding that is on the right side of the channels.
//        m_autoSizedPanelWidth += Constants::kDefaultHeaderLabelWidth + Constants::kLabelStripLeftPadding;  // Add master fader width (incl. padding).

    }
    // ==========================================================
    void HorizontalBarMetersComponent::setOptions (const Options& meterOptions)
    {
        meters_meterOptions = meterOptions;
        for(auto* meter : meters_meterChannels)
        {
            if (meter)
                meter->setOptions (meterOptions);
        }
        meters_labelStrip.setOptions(meterOptions);
        setRefreshRate(meterOptions.refreshRateHz);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setRefreshRate (float refreshRate)
    {
        meters_meterOptions.refreshRateHz = refreshRate;
        meters_labelStrip.setRefreshRate(static_cast<float> (refreshRate));
        for(auto* meter:meters_meterChannels)
            if (meter)
                meter->setRefreshRate(static_cast<float>(refreshRate));
        if(meters_useInternalTimer)
        {
            stopTimer();
            startTimer(juce::roundToInt(1000.f / refreshRate));
        }
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setMeterSegments (const std::vector<SegmentOptions>& segmentsOptions)
    {
        meters_segmentsOptions = segmentsOptions;
        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->setMeterSegments(meters_segmentsOptions);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setDecay (float decay_ms)
    {
        meters_meterOptions.decayTimeMs = decay_ms;
        for(auto* meter: meters_meterChannels)
            if(meter)
                meter->setDecay(decay_ms);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::userGradients (bool useGradients)
    {
        for (auto* meter: meters_meterChannels)
            if(meter)
                meter->useGradients(useGradients);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setLabelStripPosition (LabelStripPosition labelStripPosition)
    {
        meters_labelStripPosition = labelStripPosition;
        resized();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setFont (const juce::Font& font)
    {
        meters_font = font;
        for(auto* meter: meters_meterChannels)
            if(meter)
                meter->setFont(meters_font);
        meters_labelStrip.setFont(meters_font);
    }
    // ==========================================================
    int HorizontalBarMetersComponent::getNumChannels() const noexcept
    {
        return meters_meterChannels.size();
    }
    // ==========================================================
    int HorizontalBarMetersComponent::getAutoSizedPanelWidth() const noexcept
    {
        return meters_autoSizedPanelWidth;
    }
    // ==========================================================
    void HorizontalBarMetersComponent::useInternalTiming (bool useInternalTiming) noexcept
    {
        meters_useInternalTimer = useInternalTiming;
        stopTimer();
        if(useInternalTiming)
            startTimer(static_cast<int>(std::round((1000.0f / meters_meterOptions.refreshRateHz))));
    }
    // ==========================================================
    void HorizontalBarMetersComponent::showHeader (bool showHeader)
    {
        if(meters_meterOptions.headerEnabled == showHeader)
            return;

        meters_meterOptions.headerEnabled = showHeader;
        meters_labelStrip.showHeader(showHeader);
        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->showHeader (showHeader);

        resized();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::showValue (bool showValue)
    {
        if (meters_meterOptions.valueEnabled == showValue)
            return;

        meters_labelStrip.showValue(showValue);

        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->showValue(showValue);

        resized();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::enable (bool enabled)
    {
        meters_meterOptions.enabled = enabled;

        for(auto* meter: meters_meterChannels)
        {
            if(meter)
            {
                meter->setEnabled(enabled);
                meter->setVisible(enabled);
            }
        }
        meters_labelStrip.setEnabled(enabled);
        meters_labelStrip.setVisible(enabled);
        refresh(true);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::showTickMarks (bool showTickMarks)
    {
        meters_meterOptions.tickMarksEnabled = showTickMarks;
        for(auto* meter : meters_meterChannels)
            if(meter)
                meter->showTickMarks(showTickMarks);
        meters_labelStrip.showTickMarks(showTickMarks);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::visibilityChanged()
    {
        setColours();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::lookAndFeelChanged()
    {
        setColours();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::paint (juce::Graphics& g)
    {
        Component::paint (g);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::resized()
    {
        Component::resized();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::hiResTimerCallback()
    {
        refresh();
    }
    // ==========================================================
    void HorizontalBarMetersComponent::setColours()
    {
        if(isColourSpecified (HorizontalBarMeterChannel::backgroundColourId))
            meters_backgroundColour = findColour(HorizontalBarMeterChannel::backgroundColourId);
        else if(getLookAndFeel().isColourSpecified( HorizontalBarMeterChannel::backgroundColourId))
            meters_backgroundColour = getLookAndFeel().findColour(HorizontalBarMeterChannel::backgroundColourId);
    }
    // ==========================================================
    void HorizontalBarMetersComponent::createMeter (const juce::AudioChannelSet& channelFormat, const std::vector<juce::String>& channelNames)
    {
        //create enough meters to match the channel format.
        for (int channelIdx = 0; channelIdx < channelFormat.size(); ++channelIdx)
            auto meterChannel = std::make_unique<HorizontalBarMeterChannel> (meters_meterOptions,
                Constants::kMetersId,
                channelFormat.getTypeOfChannel (channelIdx));
        setChannelNames(channelNames);
        setMeterSegments(meters_segmentsOptions);
    }

    void HorizontalBarMetersComponent::deleteMeters()
    {
        meters_meterChannels.clear();
    }

    HorizontalBarMeterChannel* HorizontalBarMetersComponent::getMeterChannel (int meterIndex) noexcept
    {
        return(juce::isPositiveAndBelow(meterIndex, meters_meterChannels.size()) ? meters_meterChannels[meterIndex] : nullptr);
    }

} // LevelMeter