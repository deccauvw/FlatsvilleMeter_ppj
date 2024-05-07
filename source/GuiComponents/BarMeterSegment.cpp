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
            updateLevelBounds();l
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
        const auto segmentBounds = floatBounds.withY(floatBounds.getY() + floatBounds.proportionOfHeight(1.0f - m_segmentOptions.meterRange.getEnd()).with)
    }
    void BarMeterSegment::resetPeakHold() noexcept
    {
    }
    void BarMeterSegment::setSegmentOptions (SegmentOptions segmentOptions)
    {
    }
    void BarMeterSegment::setMeterOptions (const Options& options)
    {
    }
    void BarMeterSegment::updateLevelBounds()
    {
    }
    void BarMeterSegment::updatePeakHoldBounds()
    {
    }
    void BarMeterSegment::drawTickMarks (juce::Graphics& g, const MeterColours& meterColours)
    {
    }
    void BarMeterSegment::drawLabels (juce::Graphics& g, const MeterColours& meterColours) const
    {
    }
} // Gui