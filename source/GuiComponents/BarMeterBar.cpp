//
// Created by deccauvw@github on 2024-05-07.
//

#include "BarMeterBar.h"

namespace Gui
{
    BarMeterBar::BarMeterBar() = default;
    BarMeterBar::BarMeterBar (int channel)
    {
        jassert (channel == 0 || channel == 1);
        int yPos, yBarPos;
        if (channel == 0)
        {
            yPos = Gui::Constants::kPeakHoldLevelTextPositionYup;
            yBarPos = Gui::Constants::kMeterPositionAy;
        }
        else
        {
            yPos = Gui::Constants::kPeakHoldLevelTextPositionYdn;
            yBarPos = Gui::Constants::kMeterPositionBy;
        }

        m_meterBounds = juce::Rectangle<int> (
            Gui::Constants::kPeakHoldLevelTextPositionX,
            yPos,
            Gui::Constants::kPeakHoldLevelTextPositionWidth,
            Gui::Constants::kPeakHoldLevelTextPositionHeight);

        m_levelBounds = juce::Rectangle<int> (
            Gui::Constants::kMeterPositionAx,
            yBarPos,
            Gui::Constants::kMeterBarWidth,
            Gui::Constants::kMeterBarHeight);
    }

    BarMeterBar::~BarMeterBar() = default;

    void BarMeterBar::paint (juce::Graphics& g)
    {
        auto bounds = m_levelBounds.toFloat();
        g.setColour (juce::Colours::aliceblue);
        auto scaledWidth = juce::jmap (m_meterLevelDb, Gui::Constants::kLevelMinInDecibels, Gui::Constants::kLevelMaxInDecibels, m_meterRange.getStart(), m_meterRange.getEnd());
        g.fillRect (bounds.removeFromLeft (scaledWidth));
    }
    void BarMeterBar::setMeterLevelValueDecibels (const float value)
    {
        m_meterLevelDb = value;
    }
    float BarMeterBar::getMeterLevelValueDecibels()
    {
        return m_meterLevelDb;
    }
    void BarMeterBar::refreshMeterLevel()
    {
        m_meterLevelDb = getDecayedLevel (getMeterLevelValueDecibels()); //refershed
        if (m_meterLevelDb > getPeakHoldLevel())
            m_peakHoldDirty = true;
        // omitted segments
    }
    float BarMeterBar::getMeterLevel() const noexcept
    {
        return m_meterLevelDb;
    }
    void BarMeterBar::setRefreshRateHz (float refreshRateHz)
    {
        m_refreshRateHz = refreshRateHz;
    }
    float BarMeterBar::getRefreshRate()
    {
        return m_refreshRateHz;
    }
    void BarMeterBar::setDecay (float decayMs)
    {
        m_meterOptions.decayTimeMs = decayMs;
        calculateDecayCoeff (m_meterOptions);
        syncMeterOptions();
    }
    float BarMeterBar::getDecay() const noexcept
    {
        return m_meterOptions.decayTimeMs;
    }
    void BarMeterBar::setMeterSegments (const std::vector<SegmentOptions>& segmentsOptions)
    {
        m_segments.clear();
        for (const auto& segmentOptions : segmentsOptions)
        {
            m_segments.emplace_back (m_meterOptions, segmentOptions);
            m_meterRange.setStart (juce::jmin (m_meterRange.getStart(), segmentOptions.levelRange.getStart()));
            m_meterRange.setEnd (juce::jmax (m_meterRange.getEnd(), segmentOptions.levelRange.getEnd()));
        }
        syncMeterOptions();
        calculateDecayCoeff (m_meterOptions);
    }
    juce::Rectangle<int> BarMeterBar::getMeterBounds() const noexcept
    {
        return m_meterBounds;
    }
    juce::Rectangle<int> BarMeterBar::getLevelBounds() const noexcept
    {
        return m_levelBounds;
    }
    void BarMeterBar::drawMeter (juce::Graphics& g, const MeterColours& meterColours)
    {
        for (auto& segment : m_segments)
        {
            segment.draw (g, meterColours);
        }
        drawPeakValue (g, meterColours);
    }
    void BarMeterBar::drawPeakValue (juce::Graphics& g, const MeterColours& meterColours)
    {
        const auto peakDb = getPeakHoldLevel();
        if (peakDb > m_meterRange.getStart())
        {
            const int precision = peakDb <= -10.0f ? 1 : 2; //set precision depending on peak value.
            g.setColour (meterColours.colourText);
            g.drawFittedText (juce::String (peakDb, precision), m_meterBounds, juce::Justification::centred, 1);
        }
    }
    float BarMeterBar::getDecayedLevel (float newLevelDb)
    {
        const auto currentTime = static_cast<int> (juce::Time::getMillisecondCounter());
        const auto timePassed = static_cast<float> (currentTime - static_cast<int> (m_previousRefreshTime));

        if (timePassed < 1000.f / m_refreshRateHz)
            return m_meterLevelDb;
        m_previousRefreshTime = static_cast<float> (currentTime);
        if (newLevelDb >= m_meterLevelDb)
            return newLevelDb;

        if (timePassed > m_meterOptions.decayTimeMs)
            return newLevelDb;
        if (m_meterLevelDb == newLevelDb)
            return newLevelDb;

        auto numberOfFramePassed = static_cast<int> (std::round ((timePassed * m_meterOptions.refreshRateHz / 1000.f)));

        auto levelDb = m_meterLevelDb;
        for (int frame = 0; frame < numberOfFramePassed; ++frame)
            levelDb = newLevelDb + (m_decayCoeff * (levelDb - newLevelDb));
        if (std::abs (levelDb - newLevelDb) < Gui::Constants::kLevelMinInDecibels)
            levelDb = newLevelDb;
        return levelDb;
    }
    float BarMeterBar::getLinearDecayedLevel (float newLevelDb)
    {
        const auto currentTime = static_cast<int> (juce::Time::getMillisecondCounter());
        const auto timePassed = static_cast<float> (currentTime - static_cast<int> (m_previousRefreshTime));

        m_previousRefreshTime = static_cast<float> (currentTime);

        if (newLevelDb == m_meterLevelDb)
            return newLevelDb;
        return juce::jmax (newLevelDb, m_meterLevelDb - (timePassed * m_decayRate));
    }
    void BarMeterBar::calculateDecayCoeff (const Options& meterOptions)
    {
        m_meterOptions.decayTimeMs = juce::jlimit (Constants::kLevelMinDecayMs,
            Constants::kLevelMaxDecayMs,
            meterOptions.decayTimeMs);

        m_meterOptions.refreshRateHz = juce::jmax (1.0f, meterOptions.refreshRateHz);
        m_decayRate = m_meterRange.getLength() / m_meterOptions.decayTimeMs;

        m_decayCoeff = std::pow (0.01f, (1000.f / (m_meterOptions.decayTimeMs * m_meterOptions.refreshRateHz)));
    }
    void BarMeterBar::syncMeterOptions()
    {
        for (auto& segment : m_segments)
        {
            segment.setMeterOptions (m_meterOptions);
        }
        m_peakHoldDirty = true;
    }
    float BarMeterBar::getPeakHoldLevel()
    {
        if (m_segments.empty())
            return Constants::kLevelMinInDecibels;

        return m_segments[0].getPeakHold();
    }
    void BarMeterBar::resetPeakHold()
    {
        m_peakLevelDb = Constants::kLevelMinInDecibels;
        m_peakHoldDirty = true;
    }

} // Gui