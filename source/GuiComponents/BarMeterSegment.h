//
// Created by deccauvw@github on 2024-05-07.
//

#ifndef FLATSVILLEMETER_BARMETERSEGMENT_H
#define FLATSVILLEMETER_BARMETERSEGMENT_H
#pragma once
#include "BarMeterHelper.h"
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

namespace Gui
{

    class BarMeterSegment final
    {
    public:
        BarMeterSegment (const Options& meterOptions, const SegmentOptions& segmentOptions);

        void setLevel (float levelDb);

        void draw (juce::Graphics& g, const MeterColours& meterColours);

        void setMeterBounds (juce::Rectangle<int> meterBounds);

        [[nodiscard]] juce::Rectangle<float> getSegmentBounds() const noexcept { return m_segmentBounds; }

        void resetPeakHold() noexcept;

        [[nodiscard]] float getPeakHold() const noexcept { return m_peakHoldLevelDb; }

        [[nodiscard]] bool isDirty() const noexcept { return m_isDirty; }

        void setSegmentOptions (SegmentOptions segmentOptions);

        [[nodiscard]] SegmentOptions getSegmentOptions() const noexcept { return m_segmentOptions; }

        void setMeterOptions (const Options& options);

        [[nodiscard]] Options getMeterOptions() const { return m_meterOptions; }

    private:
        SegmentOptions m_segmentOptions {};
        Options m_meterOptions {};
        std::vector<float> m_tickMarks {};
        juce::Rectangle<int> m_meterBounds {};
        juce::Rectangle<float> m_segmentBounds {};
        juce::Rectangle<float> m_drawnBounds {};
        juce::Rectangle<float> m_peakHoldBounds {};
        juce::Rectangle<float> m_drawnPeakHoldBounds {};
        juce::ColourGradient m_gradientFill {};

        float m_currentLevelDb = Constants::kLevelMinInDecibels;
        float m_peakHoldLevelDb = Constants::kLevelMinInDecibels;
        bool m_isDirty = false;

        void updateLevelBounds();
        void updatePeakHoldBounds();
        void drawTickMarks (juce::Graphics& g, const MeterColours& meterColours);
        void drawLabels (juce::Graphics& g, const MeterColours& meterColours) const;

        JUCE_LEAK_DETECTOR (BarMeterSegment)
    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERSEGMENT_H
