#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p), facePlateGui()
{
    juce::ignoreUnused (m_audioProcessor);

    //show ballistic bar meters
    addAndMakeVisible(barMeterComponentChannel0);
    addAndMakeVisible(barMeterComponentChannel1);
    //show faceplate
    addAndMakeVisible(facePlateGui);
    addAndMakeVisible(tinyStripComponent);

    startTimerHz(Gui::Constants::kInitialRefreshRateHz);
    tinyStripComponent.setNumericValue(0);
    setSize (Gui::Constants::kGuiSizeWidth, Gui::Constants::kGuiSizeHeight);

}//constructor

PluginEditor::~PluginEditor() =default;

//=============================================================
void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); //opaque filling
    // render everything here
    facePlateGui.paint(g); //faceplate render
    tinyStripComponent.paint(g); //tinyStrip render

}

void PluginEditor::resized()
{
    auto boundsChannelL = juce::Rectangle<int>(Gui::Constants::kMeterPositionAx, Gui::Constants::kMeterPositionAy, Gui::Constants::kMeterBarWidth, Gui::Constants::kMeterBarHeight);
    auto boundsChannelR = juce::Rectangle<int>(Gui::Constants::kMeterPositionBx, Gui::Constants::kMeterPositionBy, Gui::Constants::kMeterBarWidth, Gui::Constants::kMeterBarHeight);
    barMeterComponentChannel0.setBounds(boundsChannelL);
    barMeterComponentChannel1.setBounds(boundsChannelR);
}


void PluginEditor::timerCallback()
{
    //set value here
    auto levelValueRms0 = m_audioProcessor.getLevelValueRms(0);
    auto levelValueRms1 = m_audioProcessor.getLevelValueRms(1);
    auto levelValuePeak0 = m_audioProcessor.getLevelValuePeak(0);
    auto levelValuePeak1 = m_audioProcessor.getLevelValuePeak(1);

    barMeterComponentChannel0.setLevel(levelValuePeak0);
    barMeterComponentChannel1.setLevel(levelValuePeak1);
    barMeterComponentChannel0.repaint();
    barMeterComponentChannel1.repaint();
}