//
// Created by deccauvw@github on 2024-05-07.
//

#include "TinyStripComponent.h"

namespace Gui
{

    TinyStripComponent::TinyStripComponent(std::function<std::vector<float>()>&& valueStringSupplier) :
                                                                                                 valueStringSupplierFn(std::move(valueStringSupplier)),
                                                                                                 m_isDirty(true)
    {
        m_fontDefault = juce::Font(Gui::Constants::kDefaultTypeFace, Gui::Constants::kTinyStripFontHeight, 0);
        startTimerHz(Gui::Constants::kInitialRefreshRateHz);
        m_peakValues = {0.0f, 0.0f};
        m_previousRefreshTimes = {0, 0};
    }
    TinyStripComponent::~TinyStripComponent()
    {
        stopTimer();
    }

    void TinyStripComponent::paint (juce::Graphics& g)
    {
        //printf("TinyStripComponent::paint called\n");
        //setStringContent(valueStringSupplierFn); //update string content
        draw(g);
    }
    //===================================================================
    //===================================================================
    //===================================================================
    //===================================================================

    void TinyStripComponent::draw (juce::Graphics& g)
    {
        //draw numeric value in the TinyStrip.
        //const auto tinyStripText = getNumericValue();
        g.setFont (m_fontDefault);
        g.setColour (m_meterColours.colourText);

//        if(!m_isDirty)
//            return;

        g.drawFittedText (stringContent,
            Gui::Constants::kTinyStripTextPositionX,
            Gui::Constants::kTinyStripTextPositionY,
            Gui::Constants::kTinyStripTextPositionWidth - Gui::Constants::kTinyStripTextMargin,
            Gui::Constants::kTinyStripTextPositionHeight,
            juce::Justification::centredRight,
            true);
    }

    juce::String TinyStripComponent::getStringContent()
    {
        return stringContent;
    }

//    std::vector<float> TinyStripComponent::setPeakHoldValuesForStringContent(const std::vector<float> values)
//    {
//        auto valueCh0 = juce::Decibels::gainToDecibels(values.at(0));
//        auto valueCh1 = juce::Decibels::gainToDecibels(values.at(1));
//
//        auto newValueToBeHeldCh0 = computePeakValueWhileHoldInDb(0,  valueCh0, m_peakValues.at(0));
//        auto newValueToBeHeldCh1 = computePeakValueWhileHoldInDb(1, valueCh1, m_peakValues.at(1));
//        auto processedPeakValues = {newValueToBeHeldCh0, newValueToBeHeldCh1};
//
//        return processedPeakValues;
//    }
//

    void TinyStripComponent::setStringContent(const std::vector<float> values)
    {
        std::string valueFormattedCh0 = std::format("{:.0f}", values.at(0));
        std::string valueUnit = " dB";
        auto s = juce::String(valueFormattedCh0) + juce::String("\t\t") + juce::String(valueUnit);
        stringContent = juce::String(s);
    }

//    float TinyStripComponent::computePeakValueWhileHoldInDb(int channel, float newValuePeakWithoutHoldDb, float valuePeakBeingHeldDb)
//    {
//
//        auto currentPeakValue = valuePeakBeingHeldDb; //helded or to be held dB
//        auto newPeakValue = newValuePeakWithoutHoldDb; //new data input dB
//
//        const auto currentTime = static_cast<int>(juce::Time::getMillisecondCounter());
//        const auto elapsedTime =currentTime - m_previousRefreshTimes.at(channel);
//        bool isTimedOut = elapsedTime > static_cast<int>(m_meterOptions.kTinyStripPeakValueHoldingTime);
//
//        //printf("elapsed time ::: %.0f\n", elapsedTime);
//
//
//        if(newPeakValue > currentPeakValue || isTimedOut) // elapsed time > 2s AND newPeak <~ currentPeak
//        {
//            currentPeakValue = newPeakValue;
//            m_previousRefreshTimes.at(channel) = currentTime; //reset Time
//            //m_isDirty = true;
//        }
//        return currentPeakValue;
//    }


    void TinyStripComponent::timerCallback()
    {
        auto inputValues = valueStringSupplierFn(); //vector
        for(int ch = 0;ch<inputValues.size(); ch++)
        {
            inputValues.at(ch) = inputValues.at(ch);
        }
        setStringContent(inputValues);
        repaint();
    }

} // gui