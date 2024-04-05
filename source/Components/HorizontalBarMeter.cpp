/*
  ==============================================================================

    HorizontalBarMeter.cpp
    Created: 19 Mar 2024 12:21:19am
    Author:  Decca

  ==============================================================================
*/

#include "HorizontalBarMeter.h"
#include "juce_gui_basics/components/juce_Component.h"
#include "juce_graphics/juce_graphics.h"

void Gui::HorizontalBarMeter::paint(juce::Graphics& g)
{
    auto bounds = juce::Component::getLocalBounds().toFloat();

    g.setColour(juce::Colours::black);
    g.fillRect(bounds);
    g.setColour(juce::Colours::aliceblue);
    //map level from -20.f ~ +3.f to 0 ~ width

    const auto scaledWidthOfBar = juce::jmap(
            level,
            -20.f, +3.f,
            0.f, static_cast<float>(juce::Component::getWidth())
            );

    g.fillRoundedRectangle(bounds.removeFromLeft(scaledWidthOfBar), 1.f);
}

void Gui::HorizontalBarMeter::setLevel(float levelSetValue)
{
    level = levelSetValue;
}
