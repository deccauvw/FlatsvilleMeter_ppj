//
// Created by deccauvw@github on 2024-05-07.
//

#include "BarMeterSegment.h"

namespace Gui
{
    BarMeterSegment::BarMeterSegment (const Options& meterOptions, const SegmentOptions& segmentOptions)
    {
        setSegmentOptions(segmentOptions);
        setMeterOptions(meterOptions);
    }
    void BarMeterSegment::setLevel (float levelDb)
    {
        if(levelDb != m_currentLevelDb)
        {
            m_currentLevelDb = levelDb;
            updateLevelBounds();
        }

        if(levelDb > m_peakHoldLevelDb)
        {
            m_peakHoldLevelDb = levelDb;
            updatePeakHoldBounds();
        }
    }
    void BarMeterSegment::draw (juce::Graphics& g, const MeterColours& meterColours)
    {
        m_isDirty = false;
        drawTickMarks(g, meterColours);

        if(!m_drawnBounds.isEmpty())
        {

            g.setColour(m_segmentOptions.colourSegment);
            g.fillRect(m_drawnBounds);
        }

        drawTickMarks(g,meterColours);

        g.setColour(meterColours.colourPeakHold);
        g.fillRect(m_peakHoldBounds);
        m_drawnPeakHoldBounds = m_peakHoldBounds;
    }
    void BarMeterSegment::setMeterBounds (juce::Rectangle<int> meterBounds)
    {
        m_meterBounds = meterBounds;
        auto floatBounds = meterBounds.toFloat();
        const auto segmentBounds =
            floatBounds.withY (floatBounds.getY() + floatBounds.proportionOfHeight (1.0f - m_segmentOptions.meterRange.getEnd()))
                .withHeight (floatBounds.proportionOfHeight (m_segmentOptions.meterRange.getLength()));
        m_segmentBounds = segmentBounds;
        updateLevelBounds();
        updatePeakHoldBounds();
        m_isDirty = true;

    }
    void BarMeterSegment::segmentLevel(float levelDb)
    {
        if(levelDb != m_currentLevelDb)
        {
            m_currentLevelDb = levelDb;
            updateLevelBounds();
        }
        if(levelDb > m_peakHoldLevelDb)
        {
            m_peakHoldLevelDb = levelDb;
            updateLevelBounds();
        }
    }
    void BarMeterSegment::resetPeakHold() noexcept
    {
        m_peakHoldBounds.setHeight(0);
        m_peakHoldLevelDb = Constants::kLevelMinInDecibels;
        m_drawnPeakHoldBounds = m_peakHoldBounds;
        m_isDirty = true;
    }
    void BarMeterSegment::setSegmentOptions (SegmentOptions segmentOptions)
    {
        jassert(segmentOptions.levelRange.getLength() > 0.0f);
        jassert(
            segmentOptions.meterRange.getStart() >= 0.0f &&
            segmentOptions.meterRange.getEnd() <= 1.0f&&
            segmentOptions.meterRange.getLength() > 0.0f);
        m_segmentOptions = segmentOptions;
        if(!m_meterBounds.isEmpty())
            setMeterBounds(m_meterBounds);
        m_isDirty = true;
    }
    void BarMeterSegment::setMeterOptions (const Options& meterOptions)
    {
        m_meterOptions = meterOptions;
        m_tickMarks.clear();
        m_tickMarks.clear();
        for(const auto& tickMark: meterOptions.tickMarksInDecibels)
        {
            if(Helpers::containsUpTo(m_segmentOptions.levelRange, tickMark))
                m_tickMarks.emplace_back(tickMark);
        }
        m_isDirty = true;
    }
    void BarMeterSegment::updateLevelBounds()
    {
        const auto levelRatio = std::clamp((m_currentLevelDb - m_segmentOptions.levelRange.getStart())/
            m_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
        const auto levelBounds = m_segmentBounds.withTop(m_segmentBounds.getY() + m_segmentBounds.proportionOfHeight(1.0f - levelRatio));
        if(m_drawnBounds == levelBounds)
            return;
        m_drawnBounds = levelBounds;
        m_isDirty = true;
    }
    void BarMeterSegment::updatePeakHoldBounds()
    {
        auto peakHoldBounds = juce::Rectangle<float>();
        if(Helpers::containsUpTo(m_segmentOptions.levelRange, m_peakHoldLevelDb))
        {
            const auto peakHoldRatio = std::clamp((m_peakHoldLevelDb - m_segmentOptions.levelRange.getStart()) /
                m_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
            if(peakHoldRatio == 0.0f)
                return;

            const auto peakHoldY = m_segmentBounds.getY() + m_segmentBounds.proportionOfHeight(1.0f - peakHoldRatio);
            peakHoldBounds = m_segmentBounds.withTop(peakHoldY).withHeight(Constants::kPeakHoldLevelTextPositionHeight);
        }
        if(peakHoldBounds == m_drawnPeakHoldBounds)
            return;
        m_peakHoldBounds = peakHoldBounds;
        m_isDirty = true;
    }
    void BarMeterSegment::drawTickMarks (juce::Graphics& g, const MeterColours& meterColours)
    {
        g.setColour(meterColours.colourTickMark);
        for(const auto& tickMark: m_tickMarks)
        {
            if((tickMark <= m_currentLevelDb))
                continue;
            const auto tickMarkLevelRatio = std::clamp(
                (tickMark - m_segmentOptions.levelRange.getStart()) / m_segmentOptions.levelRange.getLength() , 0.0f, 1.0f);
            const auto tickMarkY = m_segmentBounds.getY() + m_segmentBounds.proportionOfHeight(1.0f - tickMarkLevelRatio);
            const auto tickMarkBounds =
            juce::Rectangle<float>(m_segmentBounds.getX(), tickMarkY, m_segmentBounds.getWidth(), static_cast<float>(Constants::kTickMarkWidth));
            g.fillRect(tickMarkBounds);
        }
    }
    void BarMeterSegment::drawLabels (juce::Graphics& g, const MeterColours& meterColours) const
    {
        g.setColour(meterColours.colourText);
        const float fontsize = juce::jlimit(1.0f, 15.0f, static_cast<float>(m_meterBounds.getHeight()) / 4.0f);
        g.setFont(fontsize);
        for(const auto& tickMark:m_tickMarks)
        {
            const auto tickMarkLevelRatio = std::clamp(
                (tickMark - m_segmentOptions.levelRange.getStart()) / m_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
            const auto tickMarkY = m_segmentBounds.getY() + m_segmentBounds.proportionOfHeight(1.0f - tickMarkLevelRatio);
            const auto labelBounds = juce::Rectangle<float>(m_segmentBounds.getX(), tickMarkY - (fontsize / 2.0f), m_segmentBounds.getWidth(), fontsize);
            g.drawFittedText(juce::String(std::abs(tickMark)), labelBounds.reduced(Constants::kLabelStripTextPadding, 0).toNearestInt(), juce::Justification::topLeft, 1);
        }
    }
} // Gui