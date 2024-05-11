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
        explicit TinyStripComponent(std::function<float()>valueStringSupplier);
        ~TinyStripComponent() override;

        void paint(juce::Graphics& g) override;

        //===================================================================

        void draw(juce::Graphics& g);
        juce::String getStringContent();
        void setStringContent(float s);
        void timerCallback() override;
    private:
        std::function<float()>valueStringSupplierFn;
        juce::String stringContent;
        juce::String stringContentPrevious;

        bool m_isDirty;
        MeterColours m_meterColours;
        juce::Font m_fontDefault;

    };

} // gui

#endif //FLATSVILLEMETER_TINYSTRIPCOMPONENT_H
