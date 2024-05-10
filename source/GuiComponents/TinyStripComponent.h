//
// Created by deccauvw@github on 2024-05-07.
//

#ifndef FLATSVILLEMETER_TINYSTRIPCOMPONENT_H
#define FLATSVILLEMETER_TINYSTRIPCOMPONENT_H

#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "BarMeterHelper.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <cmath>

namespace Gui
{

    class TinyStripComponent : public juce::Component, private juce::Timer
    {
    public:
        TinyStripComponent();
        ~TinyStripComponent() override;

        void paint(juce::Graphics& g) override;

        //===================================================================

        void draw(juce::Graphics& g, MeterColours &meterColours);
        float getNumericValue();
        void setNumericValue(const float value);
        void timerCallback();
    private:
        float m_numericValue;
        bool m_isDirty;
        MeterColours m_meterColours;
        juce::Font m_fontDefault;

        static float truncateValue(float f, int upto);
    };

} // gui

#endif //FLATSVILLEMETER_TINYSTRIPCOMPONENT_H
