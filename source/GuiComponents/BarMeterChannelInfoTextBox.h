//
// Created by deccauvw@github on 2024-05-09.
//

#ifndef FLATSVILLEMETER_BARMETERCHANNELINFOTEXTBOX_H
#define FLATSVILLEMETER_BARMETERCHANNELINFOTEXTBOX_H

#include "BarMeterHelper.h"

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"

namespace Gui
{

    class BarMeterChannelInfoTextBox final : private juce::Component
    {
    public:
        BarMeterChannelInfoTextBox();
        ~BarMeterChannelInfoTextBox() override;

        void setChannelName(juce::String channelName);
        juce::String getChannelName();
        void paint(juce::Graphics &g) override;
        void draw(juce::Graphics &g,  MeterColours &meterColours);
        void setBounds(juce::String &channelName);
    private:
        //MeterColours meterColours;
        juce::String m_channelName;
        juce::Rectangle<int> m_SquareBounds;
        juce::Rectangle<int> m_SquarePosition;

    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERCHANNELINFOTEXTBOX_H
