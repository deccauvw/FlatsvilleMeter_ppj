//
// Created by deccauvw@github on 2024-05-07.
//

#ifndef FLATSVILLEMETER_BARMETERBAR_H
#define FLATSVILLEMETER_BARMETERBAR_H
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "BarMeterHelper.h"
#include "BarMeterSegment.h"
namespace Gui
{

    class BarMeterBar : public juce::Component, private juce::Timer
    {
    public:
        explicit BarMeterBar (int channel, std::function<float()>&& valueSupplierFn);
        ~BarMeterBar() override;
        void initialize();
        void drawBar (juce::Graphics& g);
        void paint (juce::Graphics& g) override;
        void drawMeter (juce::Graphics& g);
        void drawPeakValue (juce::Graphics& g);

        void setMeterLevelValueDecibels (float value);
        void setValueSupplier (std::function<float()>&& valueSupplierFn);
        [[nodiscard]] float getMeterLevelValueDecibels();
        void refreshMeterLevel();
        [[nodiscard]] float getMeterLevel() const noexcept;
        void setRefreshRateHz (float refreshRateHz);
        [[nodiscard]] float getRefreshRate();
        void setDecay (float decayMs);
        [[nodiscard]] float getDecay() const noexcept;
        //void setMeterSegments (const std::vector<SegmentOptions>& segmentOptions);
        [[nodiscard]] juce::Rectangle<int> getMeterBounds() const noexcept;
        [[nodiscard]] juce::Rectangle<int> getLevelBounds() const noexcept;
        //float getPeakHoldLevel();
        void resetPeakHold();
        void updateBarFigure (float meterLevelValueDb);
        void timerCallback() override;

    private:
        int channelNumber;
        MeterBallisticsType m_meterBallisticsType;
        std::function<float()> valueSupplier;
        //std::vector<SegmentOptions> m_segmentOptions = Gui::MeterScales::getMeterScaleDefault();
        //std::vector<BarMeterSegment> m_segments {};
        juce::Rectangle<int> m_meterBounds {}; //numeric peak meter
        juce::Rectangle<int> m_BarBounds {}; //graphic level meter
        juce::Range<float> m_meterRange { Constants::kLevelMaxInDecibels, Constants::kLevelMinInDecibels };
        //meterLevels
        std::atomic<float> m_inputLevelDb { 0.0f };
        float m_meterLevelDb = Constants::kLevelMinInDecibels;
        float m_peakLevelDb = Constants::kLevelMinInDecibels;
        Options m_meterOptions;
        [[maybe_unused]] MeterColours meterColours;
        float m_peakHoldDirty = false;
        float m_decayCoeff = 0.0f;
        float m_refreshRateHz = 0.0f;
        float m_previousRefreshTime = 0.0f;
        float m_decayRate = 0.0f; //decay rate in dB/ms.

        [[nodiscard]] float getDecayedLevel (float newLevelDb);
        [[nodiscard]] float getLinearDecayedLevel (float newLevelDb);
        void calculateDecayCoeff (const Options& meterOptions);
        //void syncMeterOptions();

        JUCE_LEAK_DETECTOR (BarMeterBar)
    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERBAR_H
