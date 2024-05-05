#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p), facePlateGui()
{
    //juce::ignoreUnused (m_audioProcessor);

    //show ballistic bar meters
    addAndMakeVisible(barMeterComponentChannelL);
    addAndMakeVisible(barMeterComponentChannelR);

    //show faceplate
    addAndMakeVisible(facePlateGui);

    startTimer(static_cast<int>(std::round((1000.0f / BarMeter::Constants::kInitialRefreshRateHz))));
    setSize (BarMeter::Constants::kGuiSizeWidth, BarMeter::Constants::kGuiSizeHeight);

}//constructor

PluginEditor::~PluginEditor() =default;

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); //opaque filling
}

void PluginEditor::resized()
{
    auto boundsChannelL = juce::Rectangle<int>(BarMeter::Constants::kMeterPositionAx,BarMeter::Constants::kMeterPositionAy, BarMeter::Constants::kMeterBarWidth, BarMeter::Constants::kMeterBarHeight);
    auto boundsChannelR = juce::Rectangle<int>(BarMeter::Constants::kMeterPositionBx,BarMeter::Constants::kMeterPositionBy, BarMeter::Constants::kMeterBarWidth, BarMeter::Constants::kMeterBarHeight);
    barMeterComponentChannelL.setBounds(boundsChannelL);
    barMeterComponentChannelR.setBounds(boundsChannelR);
}


void PluginEditor::timerCallback()
{
    //set value here
    barMeterComponentChannelL.setLevel(m_audioProcessor.getRmsValue(0));
    barMeterComponentChannelR.setLevel(m_audioProcessor.getRmsValue(1));

    barMeterComponentChannelR.repaint();
    barMeterComponentChannelL.repaint();
}