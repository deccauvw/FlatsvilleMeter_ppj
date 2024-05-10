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

    class BarMeterChannelInfoTextBox final : public juce::Component
    {
    public:
        BarMeterChannelInfoTextBox(int channel, std::function<float()> valueSupplierFn);
        ~BarMeterChannelInfoTextBox() override;

        void setChannelName(juce::String channelName);
        juce::String getChannelName();
        void paint(juce::Graphics &g) override;
        void draw(juce::Graphics &g);
        void setBounds(juce::String &channelName);
    private:
        std::function<float()> valueSupplierFn;
        int channelNumber;
        MeterColours meterColours;
        juce::String m_channelName;
        juce::Rectangle<int> m_SquareBounds;
        juce::Rectangle<int> m_SquarePosition;

    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERCHANNELINFOTEXTBOX_H
