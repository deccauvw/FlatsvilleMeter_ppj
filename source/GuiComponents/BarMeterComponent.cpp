//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent ():
                                             horizontalMeterBar0(0)
                                             ,horizontalMeterBar1(1)
    {
    }
    BarMeterComponent::~BarMeterComponent() = default;

    void BarMeterComponent::paint(juce::Graphics &g)
    {
        horizontalMeterBar0.drawMeter(g, meterColours);
        horizontalMeterBar1.drawMeter(g, meterColours);
    }

    void BarMeterComponent::setInputMeterLevelValueDecibels (const int channel, const float value)
    {
        jassert(channel == 0 || channel == 1);
        switch(channel){
            case 0:
                m_levelValueChannel0 = juce::Decibels::gainToDecibels(value);
                break;
            case 1:
                m_levelValueChannel1 = juce::Decibels::gainToDecibels(value);
                break;
            default:
                break;
        }
    }
    BarMeterComponent::BarMeterComponent (const juce::AudioChannelSet& channelFormat)
    {
        m_channelFormat = channelFormat;
    }
    void BarMeterComponent::refresh (bool forceRefresh)
    {
//        meterColours.refresh();
//        horizontalMeterBar0.refresh();
//        horizontalMeterBar1.refresh();
    }
    void BarMeterComponent::reset()
    {
        m_channelFormat = juce::AudioChannelSet::stereo();
        refresh(true);
    }
    void BarMeterComponent::resetMeters()
    {
//        horizontalMeterBar0.refresh();
//        horizontalMeterBar1.refresh();
    }
    void BarMeterComponent::clearMeters()
    {
        horizontalMeterBar0.setMeterLevelValueDecibels(0.0f);
        horizontalMeterBar1.setMeterLevelValueDecibels(0.0f);
        refresh(true);
    }
    void BarMeterComponent::resetPeakHold()
    {
        horizontalMeterBar0.resetPeakHold();
        horizontalMeterBar1.resetPeakHold();
    }
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
    void BarMeterComponent::setChannelNames (const std::vector<juce::String>& channelNames)
    {
        const auto numChannelNames = static_cast<int> (channelNames.size());
        const auto numMeters = static_cast<int> (Gui::Constants::kChannelSize);
        auto defaultMeterWidth = static_cast<float> (Constants::kMeterBarWidth);

        channelInfoTextBox0.setChannelName(channelNames[static_cast<size_t>(0)]);
        channelInfoTextBox1.setChannelName(channelNames[static_cast<size_t>(1)]);
    }
    void BarMeterComponent::setRefreshRate (float refreshRate)
    {
        meterOptions.refreshRateHz = refreshRate;

    }
    void BarMeterComponent::useInternalTiming (bool useInternalTiming) noexcept
    {
    }
    void BarMeterComponent::setColours()
    {
    }

} // Gui