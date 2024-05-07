#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p), facePlateGui()
{
    juce::ignoreUnused (m_audioProcessor);

    //show ballistic bar meters
    addAndMakeVisible(barMeterComponent);
    //show faceplate
    addAndMakeVisible(facePlateGui);

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
    //barMeterComponent.setBounds();
}


void PluginEditor::timerCallback()
{
    //set value here
    auto levelValueRms0 = m_audioProcessor.getLevelValueRms(0);
    auto levelValueRms1 = m_audioProcessor.getLevelValueRms(1);
    auto levelValuePeak0 = m_audioProcessor.getLevelValuePeak(0);
    auto levelValuePeak1 = m_audioProcessor.getLevelValuePeak(1);

    barMeterComponent.setInputMeterLevelValueDecibels(0, levelValuePeak0);
    barMeterComponent.setInputMeterLevelValueDecibels(1, levelValuePeak1);
    barMeterComponent.repaint();
}