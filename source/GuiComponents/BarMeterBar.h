//
// Created by deccauvw@github on 2024-05-07.
//

#ifndef FLATSVILLEMETER_BARMETERBAR_H
#define FLATSVILLEMETER_BARMETERBAR_H
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "BarMeterSegment.h"
#include "BarMeterHelper.h"
namespace Gui
{

    class BarMeterBar : public juce::Component, private juce::Timer
    {
    public:
        BarMeterBar();
        explicit BarMeterBar (int channel);
        ~BarMeterBar() override;

        void paint (juce::Graphics& g) override;
        void setMeterLevelValueDecibels (const float value);
        [[nodiscard]] float getMeterLevelValueDecibels();
        void refreshMeterLevel();
        [[nodiscard]] float getMeterLevel() const noexcept;
        void setRefreshRateHz (float refreshRateHz);
        [[nodiscard]] float getRefreshRate();
        void setDecay (float decayMs);
        [[nodiscard]] float getDecay() const noexcept;
        void setMeterSegments (const std::vector<SegmentOptions>& segmentOptions);
        [[nodiscard]] juce::Rectangle<int> getMeterBounds() const noexcept;
        [[nodiscard]] juce::Rectangle<int> getLevelBounds() const noexcept;
        void drawMeter (juce::Graphics& g, const MeterColours& meterColours);
        void drawPeakValue (juce::Graphics& g, const MeterColours& meterColours);
        float getPeakHoldLevel();

    private:
        std::vector<SegmentOptions> m_segmentOptions = Gui::MeterScales::getMeterScaleDefault();
        std::vector<BarMeterSegment> m_segments {};
        juce::Rectangle<int> m_meterBounds {}; //numeric peak meter
        juce::Rectangle<int> m_levelBounds {}; //graphic level meter
        juce::Range<float> m_meterRange { Constants::kLevelMaxInDecibels, Constants::kLevelMinInDecibels };
        //meterLevels
        std::atomic<float> m_inputLevel { 0.0f };
        float m_meterLevelDb = Constants::kLevelMinInDecibels;
        Options m_meterOptions;
        float m_peakHoldDirty = false;
        float m_decayCoeff = 0.0f;
        float m_refreshRateHz = Constants::kInitialRefreshRateHz;
        float m_previousRefreshTime = 0.0f;
        float m_decayRate = 0.0f; //decay rate in dB/ms.

        [[nodiscard]] float getDecayedLevel (float newLevelDb);
        [[nodiscard]] float getLinearDecayedLevel (float newLevelDb);
        void calculateDecayCoeff (const Options& meterOptions);
        void syncMeterOptions();

        JUCE_LEAK_DETECTOR (BarMeterBar)
    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERBAR_H
