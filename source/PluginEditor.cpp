#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p), barMeterComponent(p)
{
    //juce::ignoreUnused (m_audioProcessor);

    //show displays from barMeterComponent
    for(auto* c : barMeterComponent.addAndMakeVisibleEverythingThrower())
    {
        addAndMakeVisible(c);
    }

    //show controllable elements from barMeterComponent unique_ptr<juce::Sliders>
    for(auto* c : barMeterComponent.addAndMakeVisibleEveryUniquePtrThrower<juce::Slider>())
    {
        addAndMakeVisible(c);
        c->setRange(-96.0, 10.0, 0.01);
    }

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
void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); //opaque filling

    // render everything here
    facePlateGui.paint(g); //faceplate render
    barMeterComponent.paint(g); //everything else render
}

void PluginEditor::resized()
{}

void PluginEditor::timerCallback()
{
    repaint();
    //printf("mainEditor callback called....................\n");
    //barMeterComponent.setLevelValues(m_audioProcessor.m_nChannelPeakLevels);
//    barMeterComponent.updateEverything();
//    barMeterComponent.repaintEverything();
}