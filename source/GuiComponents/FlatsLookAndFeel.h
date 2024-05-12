//
// Created by deccauvw@github on 2024-05-12.
//

#ifndef FLATSVILLEMETER_FLATSLOOKANDFEEL_H
#define FLATSVILLEMETER_FLATSLOOKANDFEEL_H
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "BarMeterHelper.h"

namespace Gui
{

    class FlatsLookAndFeel: public juce::LookAndFeel_V4
    {
    public:
        FlatsLookAndFeel();
        virtual ~FlatsLookAndFeel() override;
        virtual void drawRotaryDial(
            juce::Graphics& g,
            int coordX, int coordY,
            int width, int height,
            float sliderPosProportional,
            float rotaryAngleMin,
            float rotaryAngleMax,
            juce::Slider::SliderStyle style,
            juce::Slider& slider);
    private:
        juce::Path rotaryPath;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlatsLookAndFeel)
    };

} // Gui

#endif //FLATSVILLEMETER_FLATSLOOKANDFEEL_H
