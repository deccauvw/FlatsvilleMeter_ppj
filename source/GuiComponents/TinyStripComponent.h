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

namespace Gui
{

    class TinyStripComponent : public juce::Component, public juce::Timer
    {
    public:
        TinyStripComponent();
        ~TinyStripComponent() override;

        void paint(juce::Graphics& g) override;
        void setNumericValue(const float value);
        void timerCallback() override;
    private:
        float m_numericValue;
        bool m_isDirty;
        MeterColours m_meterColours;
        juce::Font m_fontDefault;
    };

} // gui

#endif //FLATSVILLEMETER_TINYSTRIPCOMPONENT_H
