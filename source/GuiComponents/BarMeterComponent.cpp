//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent ()
    {
        startTimerHz(24);
        //grill = ...
    }
    BarMeterComponent::~BarMeterComponent() = default;


    void BarMeterComponent::paint(juce::Graphics &g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colours::white); //blank screen

        g.setColour(juce::Colours::white);
        float scaledLevelToWidth = juce::jmap(m_levelValue, BarMeter::Constants::kLevelMinInDecibels, BarMeter::Constants::kLevelMaxInDecibels, 0.0f, static_cast<float>(getWidth()));
        g.fillRect(bounds.removeFromLeft(scaledLevelToWidth));

    }

    void BarMeterComponent::setLevel(float value)
    {
        m_levelValue = value;
    }
    void BarMeterComponent::timerCallback()
    {
    }

} // Gui