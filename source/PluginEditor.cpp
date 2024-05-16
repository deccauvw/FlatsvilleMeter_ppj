#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"
//using APVTS = juce::AudioProcessorValueTreeState;

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p), barMeterComponent(p)
{
    //initialize parameters := dials
    sliderGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sliderGain.setTextBoxStyle(
        juce::Slider::NoTextBox,
        true,
        40,20);
    sliderGain.setRange(-20.f, 20.f, 0.1f);
    sliderGain.setBounds(669, 28, 145, 145); //referring tinyStrip
    sliderGain.addListener(this);
    addAndMakeVisible(sliderGain);
    sliderGainAttachment = std::make_unique<APVTS::SliderAttachment>(m_audioProcessor.apvts, "GAIN", sliderGain);


    //show displays from barMeterComponent
    for(auto* c : barMeterComponent.addAndMakeVisibleEverythingThrower())
    {
        addAndMakeVisible(c);
    }

    //show controllable elements from barMeterComponent unique_ptr<juce::Sliders>
//    for(auto* c : barMeterComponent.addAndMakeVisibleEveryUniquePtrThrower<juce::Slider>())
//    {
//        addAndMakeVisible(c);
//        c->setRange(-96.0, 10.0, 0.01);
//    }

    //show faceplate
    addAndMakeVisible(facePlateGui, 1);

    setSize (Gui::Constants::kGuiSizeWidth, Gui::Constants::kGuiSizeHeight);
    setResizable(false, false);
    startTimerHz(24);
    //barMeterComponent.reset();

}//constructor

PluginEditor::~PluginEditor()
{
    stopTimer();
}
//=============================================================
void PluginEditor::sliderValueChanged (juce::Slider* sliderLastChanged)
{
    if(sliderLastChanged == &sliderGain)
    {
        m_audioProcessor.parameters.param_gain = static_cast<float>(sliderGain.getValue());
    }
}
//=============================================================
void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); //opaque filling

    // render everything here
    facePlateGui.paint(g); //faceplate render
    barMeterComponent.paint(g); //everything else render
}

void PluginEditor::resized()
{
    //sliderGain.setBounds(700, 54, 83, 83);
}

void PluginEditor::timerCallback()
{
    repaint();
    //printf("mainEditor callback called....................\n");
    //barMeterComponent.setLevelValues(m_audioProcessor.m_nChannelPeakLevels);
//    barMeterComponent.updateEverything();
//    barMeterComponent.repaintEverything();
}