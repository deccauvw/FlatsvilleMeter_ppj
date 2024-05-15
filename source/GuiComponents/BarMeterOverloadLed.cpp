//
// Created by deccauvw@github on 2024-05-09.
//

#include "BarMeterOverloadLed.h"

namespace Gui
{
    BarMeterOverloadLed::BarMeterOverloadLed (int channel, std::function<float()>&& valueSupplier)
        :valueSupplierFn(std::move(valueSupplier)),
          m_channelNumber(channel),
          m_previousRefreshTime(0.0f),
          m_isChannelOverloaded(ledState::optimal),
          m_refreshRateHz(Constants::kInitialRefreshRateHz),
          m_decayRate(Constants::kLevelDefaultDecayMs)

    {
        setLedBounds (channel);
        startTimerHz((int)m_meterOptions.refreshRateHz);
        m_meterBallisticsType = Gui::kMeterBallisticsTypeDefault;
    }
    // ============
    BarMeterOverloadLed::~BarMeterOverloadLed() = default;
    // ============
    void BarMeterOverloadLed::paint (juce::Graphics& g)
    {
        //printf("ChannelInfoTextBox::paint called\n");
        drawLed(g);
    }
    void BarMeterOverloadLed::setLedBounds(int channelNumber)
    {
        int xpos, ypos, width, height;

        switch(channelNumber)
        {
            case 0:
                ypos = Constants::kOverloadLedPositionYup;
                break;
            case 1:
                ypos = Constants::kOverloadLedPositionYdn;
                break;
            default:
                ypos = 0;
                printf("__invalid channel number at setLedBounds__\n");
                break;
        }

        xpos = Constants::kOverloadLedPositionX;
        width = Constants::kOverloadLedDiameterR;
        height = Constants::kOverloadLedDiameterR;
        m_LedBounds = juce::Rectangle<int>(xpos, ypos, width, height);
        m_LedPosition = juce::Rectangle<int>(m_LedBounds.getX(), m_LedBounds.getY());
    }
    //BarMeterOverloadLed::BarMeterOverloadLed() = default;

    void BarMeterOverloadLed::initialize()
    {
        m_isChannelOverloaded = ledState::optimal;
        m_inputLevelDb = -INFINITY;
        bundleOfValues = {0.0, 0.0, 0.0};
    }
    juce::Rectangle<int> BarMeterOverloadLed::getLedBounds()
    {
        return m_LedBounds;
    }
    //=======================================================================================================================
    //=======================================================================================================================

    void BarMeterOverloadLed::setIsChannelOverloaded()
    {
        float* desiredValue = nullptr;

        switch(m_meterBallisticsType)
        {
            case MeterBallisticsType::PEAK:
                desiredValue = &bundleOfValues.valuePEAK;
                break;
            case MeterBallisticsType::RMS:
                desiredValue = &bundleOfValues.valueRMS;
                break;
            case MeterBallisticsType::VU:
                desiredValue = &bundleOfValues.valueVU;
                break;
            default:
                jassert("invalid mbt");
                break;
        }

        auto level = juce::Decibels::gainToDecibels(*desiredValue);
        auto criteria = m_meterOptions.meterLedStageCriteria;
        jassert(criteria.size() == 2);

        auto timeCurrent = static_cast<int>(juce::Time::getMillisecondCounter());
        auto timeElapsed = timeCurrent - static_cast<int>(m_previousRefreshTime);

        //determine the rank
        if (level < criteria.at(0))
            m_ledStateRank = 2;
        else if(level >= criteria.at(0) && level < criteria.at(1))
            m_ledStateRank = 1;
        else
            m_ledStateRank = 0;


        //condition
        bool isInSuperiorStageToPrevious = (m_ledStateRank <= m_ledStateRankPrevious);
        bool isRequiringToBeUpdated = timeElapsed > static_cast<int>(Constants::kLevelDefaultDecayMs);

        if(isInSuperiorStageToPrevious || isRequiringToBeUpdated)
        {
            if (level < criteria.at(0))
                m_isChannelOverloaded = ledState::optimal, m_ledStateRank = 2;
            else if(level >= criteria.at(0) && level < criteria.at(1))
                m_isChannelOverloaded = ledState::overload, m_ledStateRank = 1;
            else
                m_isChannelOverloaded = ledState::tooHot, m_ledStateRank = 0;

            m_ledStateRankPrevious = m_ledStateRank;
            m_previousRefreshTime = static_cast<float>(timeCurrent);
        }

    }
    ledState BarMeterOverloadLed::getIsChannelOverloaded()
    {
        return m_isChannelOverloaded;
    }
    void BarMeterOverloadLed::setChannelInfo (int channel)
    {
        m_channelNumber = channel;
    }
    int BarMeterOverloadLed::getChannelInfo()
    {
        return m_channelNumber;
    }
    void BarMeterOverloadLed::setMeterOptions (Options meterOptions)
    {
        m_meterOptions = meterOptions;
    }
    Options BarMeterOverloadLed::getMeterOptions()
    {
        return m_meterOptions;
    }
    void BarMeterOverloadLed::drawLed(juce::Graphics& g)
    {
        //draw a circle in dark blue
        //change color if m_isMeterOverloaded into orange
        //change color if m_isMeterOverloaded into Red
        juce::Colour ledLightColour;
        switch(m_isChannelOverloaded)
        {
            case ledState::optimal:
                ledLightColour = m_meterColours.colourLedOptimal;
                break;
            case ledState::overload:
                ledLightColour = m_meterColours.colourLedOverload;
                break;
            case ledState::tooHot:
                ledLightColour = m_meterColours.colourLedTooHot;
                break;
            default:
                ledLightColour = juce::Colours::black;
        }






        g.setColour( ledLightColour);
        g.fillEllipse(m_LedBounds.toFloat());

    }

    void BarMeterOverloadLed::timerCallback()
    {
        setIsChannelOverloaded();
        repaint();
    }

    void BarMeterOverloadLed::setValueSupplier(std::function<float()>&& valueSupplier)
    {
        this->valueSupplierFn = valueSupplier;
    }


} // Gui