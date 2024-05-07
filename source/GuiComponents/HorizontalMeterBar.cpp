//
// Created by deccauvw@github on 2024-05-07.
//

#include "HorizontalMeterBar.h"

namespace Gui
{
    HorizontalMeterBar::HorizontalMeterBar() = default;

    HorizontalMeterBar::~HorizontalMeterBar() = default;

    void HorizontalMeterBar::paint (juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour (juce::Colours::aliceblue);
        auto scaledWidth = juce::jmap (m_levelValue, Gui::Constants::kLevelMinInDecibels, Gui::Constants::kLevelMaxInDecibels, 0.0f, static_cast<float> (getWidth()));
        g.fillRect (bounds.removeFromLeft (scaledWidth));
    }
    void HorizontalMeterBar::setMeterLevelValueDecibels (const float value)
    {
        m_levelValue = value;
    }
    float HorizontalMeterBar::getMeterLevelValueDecibels()
    {
        return m_meterLevelDb;
    }
    void HorizontalMeterBar::refreshMeterLevel()
    {
        m_meterLevelDb = getDecayedLevel (getMeterLevelValueDecibels());
        if (m_meterLevelDb > getPeakHoldLevel())
            m_peakHoldDirty = true;
        for (auto& segment: segmen)
    }
    float HorizontalMeterBar::getMeterLevel() const noexcept
    {
        return 0;
    }
    void HorizontalMeterBar::setRefreshRateHz (float refreshRateHz)
    {
    }
    float HorizontalMeterBar::getRefreshRate()
    {
        return 0;
    }
    void HorizontalMeterBar::setDecay (float decayMs)
    {
    }
    float HorizontalMeterBar::getDecay() const noexcept
    {
        return 0;
    }
    void HorizontalMeterBar::setMeterSegments (const std::vector<SegmentOptions>& segmentOptions)
    {
    }
    float HorizontalMeterBar::getMeterBounds() const noexcept
    {
        return 0;
    }
    void HorizontalMeterBar::drawMeter (juce::Graphics& g, const MeterColours& meterColours)
    {
    }
    void HorizontalMeterBar::drawPeakValue (juce::Graphics& g, const MeterColours& meterColours) const
    {
    }
    float HorizontalMeterBar::getDecayedLevel (float newLeveldB)
    {
        return 0;
    }
    float HorizontalMeterBar::getLinearDecayedLevel (float newLeveldB)
    {
        return 0;
    }
    void HorizontalMeterBar::calculateDecayCoeff (const Options& meterOptions)
    {
    }
    void HorizontalMeterBar::syncMeterOptions()
    {
    }

} // Gui