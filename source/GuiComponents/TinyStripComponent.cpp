//
// Created by deccauvw@github on 2024-05-07.
//

#include "TinyStripComponent.h"

namespace Gui
{

    TinyStripComponent::TinyStripComponent(std::function<float()>valueStringSupplier) :
                                                                                                 valueStringSupplierFn(valueStringSupplier),
                                                                                                 m_isDirty(true)
    {
        m_fontDefault = juce::Font(Gui::Constants::kDefaultTypeFace, Gui::Constants::kTinyStripFontHeight, 0);
        startTimerHz(Gui::Constants::kInitialRefreshRateHz);
    }
    TinyStripComponent::~TinyStripComponent()
    {
        stopTimer();
    }

    void TinyStripComponent::paint (juce::Graphics& g)
    {
        printf("TinyStripComponent::paint called\n");
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
            juce::Justification::centred,
            true);
    }

    juce::String TinyStripComponent::getStringContent()
    {
        return stringContent;
    }

    void TinyStripComponent::setStringContent (const float s)
    {
        stringContent = juce::String(s);
    }

    void TinyStripComponent::timerCallback()
    {
        setStringContent(valueStringSupplierFn());
        repaint();
    }

} // gui