//
// Created by deccauvw@github on 2024-05-09.
//

#include "BarMeterChannelInfoTextBox.h"

namespace Gui
{
    BarMeterChannelInfoTextBox::BarMeterChannelInfoTextBox() = default;
    BarMeterChannelInfoTextBox::~BarMeterChannelInfoTextBox() = default;

    void BarMeterChannelInfoTextBox::setChannelName (juce::String channelName)
    {
        m_channelName = channelName;
    }
    juce::String BarMeterChannelInfoTextBox::getChannelName()
    {
        return m_channelName;
    }
    void BarMeterChannelInfoTextBox::paint (juce::Graphics& g)
    {
        juce::ignoreUnused(g);
    }
    void BarMeterChannelInfoTextBox::setBounds(juce::String &channelName)
    {
        int xpos, ypos, width, height;

        if(channelName == juce::String("left"))
        {
            ypos = Constants::kChannelInfoTextBoxChannelTypePositionYup;
        }
        else
            ypos = Constants::kChannelInfoTextBoxChannelTypePositionYdn;

        xpos = Constants::kChannelInfoTextBoxChannelTypePositionX;
        width = Constants::kChannelInfoTextBoxChannelTypePositionWidth;
        height = Constants::kChannelInfoTextBoxChannelTypePositionHeight;
        m_SquareBounds = juce::Rectangle<int>(xpos, ypos, width, height);
    }
    void BarMeterChannelInfoTextBox::draw(juce::Graphics& g, MeterColours& meterColours)
    {
        g.setColour( meterColours.colourText);
        auto font = juce::Font(Constants::kDefaultTypeFace, Constants::kTinyStripFontHeight, 0);
        g.setFont(font);
        g.drawFittedText(m_channelName,m_SquareBounds,juce::Justification::centred, 0);
    }
} // Gui