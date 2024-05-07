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
        g.setFont (m_fontDefault);
        g.setColour (m_meterColours.colourText);

        juce::String s;

        if(m_isDirty)
        {
            s = juce::String (m_numericValue);
        }
        else
        {
            s = juce::String("Dirty");
        }

        g.drawText (s,
            Gui::Constants::kTinyStripTextPositionX,
            Gui::Constants::kTinyStripTextPositionY,
            Gui::Constants::kTinyStripTextPositionWidth,
            Gui::Constants::kTinyStripTextPositionHeight,
            juce::Justification::centred,
            true);


    }


    void TinyStripComponent::setNumericValue (const float value)
    {
        if(juce::approximatelyEqual(m_numericValue, value))
        {
            m_isDirty = false;
        }
        else
        {
            m_isDirty = true;
            m_numericValue = value;
        }
    }

    void TinyStripComponent::timerCallback()
    {
        if(m_isDirty)
        {
            m_isDirty = false;
            repaint();
        }
    }

} // gui