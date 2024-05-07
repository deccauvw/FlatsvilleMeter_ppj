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
        auto scaledWidth = juce::jmap(m_levelValue, BarMeter::Constants::kLevelMinInDecibels, BarMeter::Constants::kLevelMaxInDecibels, 0.0f, static_cast<float>(getWidth()));
        g.fillRect(bounds.removeFromLeft(scaledWidth));
    }

    void BarMeterComponent::setLevel(const float value)
    {
        m_levelValue = juce::Decibels::gainToDecibels(value);
    }



} // Gui