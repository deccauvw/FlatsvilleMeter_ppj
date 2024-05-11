//
// Created by deccauvw@github on 2024-05-09.
//

#include "BarMeterChannelInfoTextBox.h"

namespace Gui
{
    BarMeterChannelInfoTextBox::BarMeterChannelInfoTextBox(int channel, std::function<float()>valueSupplier)
        :valueSupplierFn(valueSupplier),
          channelNumber(channel)
    {
        int ypos;
        switch(channelNumber)
        {
            case 0:
                ypos = Constants::kChannelInfoTextBoxChannelTypePositionYup;
                break;
            case 1:
                ypos = Constants::kChannelInfoTextBoxChannelTypePositionYdn;
                break;
            default:
                jassert("invalid channel number");
                ypos = 0;
                break;
        }

        m_SquareBounds = juce::Rectangle<int>(Constants::kChannelInfoTextBoxChannelTypePositionX, ypos,
            Constants::kChannelInfoTextBoxChannelTypePositionWidth, Constants::kChannelInfoTextBoxChannelTypePositionHeight);
        m_SquarePosition = m_SquareBounds;

//        setChannelName();
    }
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
        printf("ChannelInfoTextBox::paint called\n");
        draw(g);
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
    void BarMeterChannelInfoTextBox::draw(juce::Graphics& g)
    {
        g.setColour( meterColours.colourText);
        auto font = juce::Font(Constants::kDefaultTypeFace, Constants::kTinyStripFontHeight, 0);
        g.setFont(font);
        g.drawFittedText(m_channelName,m_SquareBounds,juce::Justification::centred, 0);
    }
} // Gui