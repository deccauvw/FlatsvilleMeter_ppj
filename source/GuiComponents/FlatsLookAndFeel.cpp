//
// Created by deccauvw@github on 2024-05-12.
//

#include "FlatsLookAndFeel.h"

namespace Gui
{
    FlatsLookAndFeel::FlatsLookAndFeel() = default;
    FlatsLookAndFeel::~FlatsLookAndFeel() = default;


    void FlatsLookAndFeel::drawRotaryDial (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryAngleMin, float rotaryAngleMax, const juce::Slider::SliderStyle style, juce::Slider& slider)
    {
        g.fillAll(slider.findColour(juce::Slider::backgroundColourId));

        rotaryPath.startNewSubPath(static_cast<float>(x), static_cast<float>(y));
        if(sliderPosProportional < 0.5f)
        {
            rotaryPath.addArc(
                (float)x, (float)y, (float)width, (float)height,
                (1.0f - (2.0f * ((float)sliderPosProportional))) * -3.0f * Constants::M_PI_4, 0,
                true
                );
        }
        else
        {
            rotaryPath.addArc(
                (float)x, (float)y, (float)width, (float)height,
                0,
                (((float)sliderPosProportional - 0.5f) * 2.f) * 3.0f * Constants::M_PI_4,
                true
                );
        }

        // Turns single line path to an outline (path now has width)
        juce::PathStrokeType(16.0f).createStrokedPath(rotaryPath, rotaryPath);

        //Transforms to fit rotary inside width and height
        rotaryPath.applyTransform(juce::AffineTransform::scale(0.8f));
        rotaryPath.applyTransform(juce::AffineTransform::translation(
            (float)((float)width - (4.0f * (float)width / 5.0f)) / 2.0f,
            (float)(((float)height - (4.0f * (float)height / 5.0f)) / 2.0f))
            );

        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.fillPath(rotaryPath);

        g.drawLine(
            (float)x + ((float)width / 2.0f),
            (float)y,
            (float)x + ((float)width / 2.0f),
            (float)y + ((float)height / 2.0f)
            );
        rotaryPath.clear();
    }
} // Gui