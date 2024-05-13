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
#include <vector>
#include <format>

namespace Gui
{

    class TinyStripComponent : public juce::Component, private juce::Timer
    {
    public:
        explicit TinyStripComponent(std::function<std::vector<float>()>&& valueStringSupplier);
        ~TinyStripComponent() override;

        void paint(juce::Graphics& g) override;

        //===================================================================

        void draw(juce::Graphics& g);
        juce::String getStringContent();
        //std::vector<float> setPeakHoldValuesForStringContent(std::vector<float>values);
        void setStringContent(std::vector<float> values);
        void timerCallback() override;
//        float computePeakValueWhileHoldInDb(int channel, float newValuePeak, float valuePeakBeingHeld);
    private:
        std::function<std::vector<float>()>valueStringSupplierFn;
        juce::String stringContent;
        juce::String stringContentPrevious;
        std::vector<int> m_previousRefreshTimes;
        std::vector<float> m_peakValues;
        //std::vector<float> m_inputPeakValues;

        float m_decayRate = 1.0f; //decay rate in dB/ms.

        bool m_isDirty;
        MeterColours m_meterColours;
        Options m_meterOptions;
        juce::Font m_fontDefault;

    };

} // gui

#endif //FLATSVILLEMETER_TINYSTRIPCOMPONENT_H
