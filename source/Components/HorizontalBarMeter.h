/*
  ==============================================================================

    HorizontalBarMeter.h
    Created: 19 Mar 2024 12:21:19am
    Author:  Decca

  ==============================================================================
*/

#pragma once
#include "juce_graphics/juce_graphics.h"

namespace Gui
{
    //===============================================
    class HorizontalBarMeter : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override;

        void setLevel(float levelSetValue);

    //===============================================
    private:
        float level = -20.f;
    };
}