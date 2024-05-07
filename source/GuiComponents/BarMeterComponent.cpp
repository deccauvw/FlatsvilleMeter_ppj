//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent () = default;
    BarMeterComponent::~BarMeterComponent() = default;


    void BarMeterComponent::paint(juce::Graphics &g)
    {
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

    }
    void BarMeterComponent::reset()
    {
    }
    void BarMeterComponent::resetMeters()
    {
    }
    void BarMeterComponent::clearMeters()
    {
    }
    void BarMeterComponent::resetPeakHold()
    {
    }
    void BarMeterComponent::setChannelFormat (const juce::AudioChannelSet& channelSet, const std::vector<juce::String>& channelNames)
    {
    }
    void BarMeterComponent::setChannelNames (const std::vector<juce::String>& channelNames)
    {
    }
    void BarMeterComponent::setRefreshRate (float refreshRate)
    {
    }
    void BarMeterComponent::useInternalTiming (bool useInternalTiming) noexcept
    {
    }
    void BarMeterComponent::setColours()
    {
    }

} // Gui