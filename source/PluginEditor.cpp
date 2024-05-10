#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p)
{
    juce::ignoreUnused (m_audioProcessor);

    //show ballistic bar meters
    addAndMakeVisible(barMeterComponent);
    //show faceplate
    addAndMakeVisible(facePlateGui);

    startTimerHz((int)Gui::Constants::kInitialRefreshRateHz);
    //barMeterComponent.reset();
    setSize (Gui::Constants::kGuiSizeWidth, Gui::Constants::kGuiSizeHeight);
}//constructor

PluginEditor::~PluginEditor() =default;

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
    //set value here
//    auto levelValueRms0 = m_audioProcessor.getLevelValueRms(0);
//    auto levelValueRms1 = m_audioProcessor.getLevelValueRms(1);
    auto RNDVALUEFORDEBUG = m_audioProcessor.M_RANDOMVALUEFORDEBUGGING;
    auto levelValuePeak0 = m_audioProcessor.getLevelValuePeak(0);
    auto levelValuePeak1 = m_audioProcessor.getLevelValuePeak(1);
    std::vector<float> levelValuesPeak = {levelValuePeak0, levelValuePeak1};
    barMeterComponent.setInputMeterLevelValueDecibels(levelValuesPeak);
    barMeterComponent.M_RANDOMVALUEFORDEBUGGING = RNDVALUEFORDEBUG;
    //repaint();
}