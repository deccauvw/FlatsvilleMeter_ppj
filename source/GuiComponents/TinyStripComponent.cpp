//
// Created by deccauvw@github on 2024-05-07.
//

#include "TinyStripComponent.h"

namespace Gui
{

    TinyStripComponent::TinyStripComponent() : m_numericValue(0.0f), m_isDirty(true)
    {
        m_fontDefault = juce::Font(Gui::Constants::kDefaultTypeFace, Gui::Constants::kTinyStripFontHeight, 0);
    }
    TinyStripComponent::~TinyStripComponent() = default;

    void TinyStripComponent::paint (juce::Graphics& g)
    {
    }

    void TinyStripComponent::draw (juce::Graphics& g , MeterColours &meterColours)
    {
        //draw numeric value in the TinyStrip.
        const auto tinyStripText = getNumericValue();
        g.setFont (m_fontDefault);
        g.setColour (m_meterColours.colourText);

        juce::String s;

        if(m_isDirty)
        {
            s = juce::String (m_numericValue);
        }
        else
        {
            s = juce::String("initialize");
        }

        g.drawFittedText (s,
            Gui::Constants::kTinyStripTextPositionX,
            Gui::Constants::kTinyStripTextPositionY,
            Gui::Constants::kTinyStripTextPositionWidth - Gui::Constants::kTinyStripTextMargin,
            Gui::Constants::kTinyStripTextPositionHeight,
            juce::Justification::left,
            true);


    }

    float TinyStripComponent::getNumericValue()
    {
        return m_numericValue;
    }

    void TinyStripComponent::setNumericValue (const float value)
    {
        auto valueDb = juce::jmax(juce::Decibels::gainToDecibels(value), Gui::Constants::kLevelMinInDecibels);
        if(m_numericValue == valueDb)
        {
            m_isDirty = false;
        }
        else
        {
            m_isDirty = true;
            m_numericValue = truncateValue(valueDb, 4);

        }

    }


    float TinyStripComponent::truncateValue (float f, int upto)
    {
        auto p = static_cast<float>(std::pow(10, upto));
        f *= p;
        f = ceil(f);
        f /= p;
        return f;
    }
} // gui