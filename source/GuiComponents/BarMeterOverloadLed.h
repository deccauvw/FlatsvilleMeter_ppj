//
// Created by deccauvw@github on 2024-05-09.
//

#ifndef FLATSVILLEMETER_BARMETEROVERLOADLED_H
#define FLATSVILLEMETER_BARMETEROVERLOADLED_H

#include "BarMeterHelper.h"

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"

namespace Gui
{
    enum ledState{
        optimal, overload, tooHot
    };


    class BarMeterOverloadLed final : public juce::Component, private juce::Timer
    {
    public:
        //BarMeterOverloadLed();
        explicit BarMeterOverloadLed (int channel, std::function<float()>&& valueSupplierFn);
        ~BarMeterOverloadLed() override;
        void initialize();
        void paint(juce::Graphics &g) override;
        void drawLed(juce::Graphics &g);
        // ==================================
        // ============
        void setLedBounds(int channelNumber);
        juce::Rectangle<int> getLedBounds();
        // ============
        void setIsChannelOverloaded();
        ledState getIsChannelOverloaded();
        // ============
        void setChannelInfo(int channel);
        int getChannelInfo();
        // ============
        void setMeterOptions(Options meterOptions);
        Options getMeterOptions();

        void setValueSupplier(std::function<float()>&&);
        // ============
        void timerCallback() override;
        // ==================================
    private:
        MeterBallisticsType m_meterBallisticsType;
        BundleOfLevelValues bundleOfValues;
        std::function<float()> valueSupplierFn; //value input gateway
        std::atomic<float> m_inputLevelDb{0.0f};
        int m_channelNumber;
        MeterColours m_meterColours;
        Options m_meterOptions;
        ledState m_isChannelOverloaded;
        int m_ledStateRank = -1;
        int m_ledStateRankPrevious = 0;
        juce::Rectangle<int> m_LedBounds;
        juce::Rectangle<int> m_LedPosition;

        float m_refreshRateHz;
        float m_previousRefreshTime;
        float m_decayRate;
    };

} // Gui

#endif //FLATSVILLEMETER_BARMETEROVERLOADLED_H
