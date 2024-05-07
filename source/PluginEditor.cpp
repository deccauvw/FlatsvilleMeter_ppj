#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p), facePlateGui()
{
    //juce::ignoreUnused (m_audioProcessor);

    //show ballistic bar meters
    addAndMakeVisible(barMeterComponentChannel0);
    addAndMakeVisible(barMeterComponentChannel1);

    //show faceplate
    addAndMakeVisible(facePlateGui);

    startTimer(static_cast<int>(std::round((1000.0f / BarMeter::Constants::kInitialRefreshRateHz))));
    setSize (BarMeter::Constants::kGuiSizeWidth, BarMeter::Constants::kGuiSizeHeight);

}//constructor

PluginEditor::~PluginEditor() =default;

//=============================================================
void PluginEditor::paint (juce::Graphics& g)
{
    //g.fillAll(juce::Colours::darkgrey); //opaque filling
    //render everything here
    facePlateGui.paint(g); //faceplate render

}

void PluginEditor::resized()
{
    auto boundsChannelL = juce::Rectangle<int>(BarMeter::Constants::kMeterPositionAx,BarMeter::Constants::kMeterPositionAy, BarMeter::Constants::kMeterBarWidth, BarMeter::Constants::kMeterBarHeight);
    auto boundsChannelR = juce::Rectangle<int>(BarMeter::Constants::kMeterPositionBx,BarMeter::Constants::kMeterPositionBy, BarMeter::Constants::kMeterBarWidth, BarMeter::Constants::kMeterBarHeight);
    barMeterComponentChannel0.setBounds(boundsChannelL);
    barMeterComponentChannel1.setBounds(boundsChannelR);
}


void PluginEditor::timerCallback()
{
    //set value here
    auto levelValueRmsDb0 = juce::Decibels::gainToDecibels(m_audioProcessor.getLevelValueRms(0));
    auto levelValueRmsDb1 = juce::Decibels::gainToDecibels(m_audioProcessor.getLevelValueRms(1));
    barMeterComponentChannel0.setLevel(levelValueRmsDb0);
    barMeterComponentChannel1.setLevel(levelValueRmsDb1);
    barMeterComponentChannel0.repaint();
    barMeterComponentChannel1.repaint();
}