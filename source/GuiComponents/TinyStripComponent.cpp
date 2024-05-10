//
// Created by deccauvw@github on 2024-05-07.
//

#include "TinyStripComponent.h"

namespace Gui
{

    TinyStripComponent::TinyStripComponent(std::function<juce::String()>valueStringSupplier) :
                                                                                                 valueStringSupplierFn(valueStringSupplier),
                                                                                                 m_isDirty(true)
    {
        m_fontDefault = juce::Font(Gui::Constants::kDefaultTypeFace, Gui::Constants::kTinyStripFontHeight, 0);
        //startTimerHz(Gui::Constants::kInitialRefreshRateHz);
    }
    TinyStripComponent::~TinyStripComponent() = default;

    void TinyStripComponent::paint (juce::Graphics& g)
    {
        setStringContent(valueStringSupplierFn());
        draw(g);
        DBG("TinyStripComponent::paint called");
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

        if(!m_isDirty)
            return;

        g.drawFittedText (stringContent,
            Gui::Constants::kTinyStripTextPositionX,
            Gui::Constants::kTinyStripTextPositionY,
            Gui::Constants::kTinyStripTextPositionWidth - Gui::Constants::kTinyStripTextMargin,
            Gui::Constants::kTinyStripTextPositionHeight,
            juce::Justification::left,
            true);
    }

    juce::String TinyStripComponent::getStringContent()
    {
        return stringContent;
    }

    void TinyStripComponent::setStringContent (const juce::String& newStringContent)
    {
        if(stringContent == newStringContent)
        {
            m_isDirty = false;
            return;
        }
        else
        {
            m_isDirty = true;
            stringContent = newStringContent;

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
    void TinyStripComponent::timerCallback()
    {
        repaint();
    }
} // gui