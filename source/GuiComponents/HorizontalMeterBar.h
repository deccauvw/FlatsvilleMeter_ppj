//
// Created by deccauvw@github on 2024-05-07.
//

#ifndef FLATSVILLEMETER_HORIZONTALMETERBAR_H
#define FLATSVILLEMETER_HORIZONTALMETERBAR_H
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"

#include "BarMeterHelper.h"
namespace Gui
{

    class HorizontalMeterBar: public juce::Component, private juce::Timer
    {
    public:
        HorizontalMeterBar();
        ~HorizontalMeterBar() override;

        void paint(juce::Graphics &g) override;
        void setMeterLevelValueDecibels(const float value);
        [[nodiscard]] float getMeterLevelValueDecibels();
        void refreshMeterLevel();
        [[nodiscard]] float getMeterLevel() const noexcept;
        void setRefreshRateHz(float refreshRateHz);
        [[nodiscard]] float getRefreshRate();
        void setDecay(float decayMs);
        [[nodiscard]] float getDecay() const noexcept;
        void setMeterSegments(const std::vector<SegmentOptions>& segmentOptions);
        [[nodiscard]] float getMeterBounds() const noexcept;

        void drawMeter(juce::Graphics &g, const MeterColours& meterColours);
        void drawPeakValue(juce::Graphics &g, const MeterColours& meterColours) const;
        float getPeakHoldLevel();


    private:
        float m_levelValue = Gui::Constants::kLevelMinInDecibels;
        std::vector<SegmentOptions> m_segmentOptions = Gui::MeterScales::getMeterScaleDefault();
        //std::vector<Segment> m_segments {};
        juce::Rectangle<int> m_meterBounds {};
        juce::Rectangle<int> m_levelBounds {};

        //meterLevels
        std::atomic<float> m_inputLevel {0.0f};
        float m_meterLevelDb = Constants::kLevelMinInDecibels;
        float m_peakHoldDirty = false;
        float m_decayCoeff = 0.0f;
        float m_refreshRateHz = Constants::kInitialRefreshRateHz;
        float m_decayRate = 0.0f; //decay rate in dB/ms.

        [[nodiscard]] float getDecayedLevel(float newLeveldB);
        [[nodiscard]] float getLinearDecayedLevel(float newLeveldB);
        void calculateDecayCoeff(const Options& meterOptions);
        void syncMeterOptions();

        JUCE_LEAK_DETECTOR(HorizontalMeterBar)

    };

} // Gui

#endif //FLATSVILLEMETER_HORIZONTALMETERBAR_H
