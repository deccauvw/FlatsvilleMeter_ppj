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
        auto bounds = getLocalBounds().toFloat();
//        g.setColour(juce::Colours::aquamarine.withBrightness(0.5f));
//        g.fillRect(bounds);
        g.setColour(juce::Colours::aliceblue);
        auto scaledWidth = juce::jmap(m_levelValue, Gui::Constants::kLevelMinInDecibels, Gui::Constants::kLevelMaxInDecibels, 0.0f, static_cast<float>(getWidth()));
        g.fillRect(bounds.removeFromLeft(scaledWidth));
    }

    void BarMeterComponent::setLevel(const float value)
    {
        m_levelValue = juce::Decibels::gainToDecibels(value);
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