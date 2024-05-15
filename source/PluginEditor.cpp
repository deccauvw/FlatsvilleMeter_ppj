#pragma once
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "LevelMeter.h"
//using APVTS = juce::AudioProcessorValueTreeState;

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), m_audioProcessor(p)
{
    //initialize parameters := dials
    sliderGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sliderGain.setTextBoxStyle(
        juce::Slider::NoTextBox,
        true,
        40,20);
    sliderGain.setRange(-20.f, 20.f, 0.1f);
    sliderGain.setBounds(Gui::Constants::kGainKnobPositionX, Gui::Constants::kGainKnobPositionY, Gui::Constants::kGainKnobWidth, Gui::Constants::kGainKnobHeight); //referring tinyStrip
    sliderGain.addListener(this);
    addAndMakeVisible(sliderGain);
    sliderGainAttachment = std::make_unique<APVTS::SliderAttachment>(m_audioProcessor.apvts, "GAIN", sliderGain);
    //initialize parameters := combobox
    comboBoxMeterType.addItem("OptionRMS", 1);
    comboBoxMeterType.addItem("OptionVU", 2);
    comboBoxMeterType.addItem("OptionPEAK", 3);
    comboBoxMeterType.setSelectedId(3); //default selection
    comboBoxMeterType.setBounds(Gui::Constants::kComboBoxPositionX, Gui::Constants::kComboBoxPositionY,Gui::Constants::kComboBoxWidth, Gui::Constants::kComboBoxHeight);
    comboBoxMeterType.addListener(this);
    addAndMakeVisible(comboBoxMeterType);

    //initialize parameters := label for metertype display
    labelMeterType.setFont(juce::Font(Gui::Constants::kDefaultTypeFace,  13, 1));
    auto getStringForLabel = m_audioProcessor.apvts.getRawParameterValue("METEROPTIONS")->load();
    labelMeterType.setText(juce::String(getStringForLabel), juce::NotificationType::dontSendNotification);
    labelMeterType.setJustificationType(juce::Justification::centred);
    labelMeterType.setBounds(Gui::Constants::kTinyStripTextPositionX, Gui::Constants::kTinyStripTextPositionY, Gui::Constants::kTinyStripTextPositionWidth, Gui::Constants::kTinyStripTextPositionHeight);
    addAndMakeVisible(labelMeterType);


    //show displays from barMeterComponent
    for(auto* c : barMeterComponent.addAndMakeVisibleEverythingThrower())
    {
        addAndMakeVisible(c);
    }

    //show faceplate
    addAndMakeVisible(facePlateGui, 1);

    setSize (Gui::Constants::kGuiSizeWidth, Gui::Constants::kGuiSizeHeight);
    setResizable(false, false);
    startTimerHz(Gui::Constants::kInitialRefreshRateHz);
}//constructor

PluginEditor::~PluginEditor()
{
    sliderGain.removeListener(this);
    comboBoxMeterType.removeListener(this);
    stopTimer();
}
//=============================================================
void PluginEditor::sliderValueChanged (juce::Slider* sliderLastChanged)
{
    if(sliderLastChanged == &sliderGain)
    {
        m_audioProcessor.parameters.param_gain = (float)sliderGain.getValue();
    }
}

void PluginEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if(comboBoxThatHasChanged == &comboBoxMeterType)
    {
        int selectedId = comboBoxThatHasChanged->getSelectedId();
        m_audioProcessor.parameters.param_meterBallisticsType = selectedId;
    }
}
//=============================================================
void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); //opaque filling

    // render everything here
    facePlateGui.paint(g); //faceplate render
    barMeterComponent.paint(g); //everything else render
    comboBoxMeterType.paint(g);
}

void PluginEditor::resized()
{
    //sliderGain.setBounds(700, 54, 83, 83);
}

void PluginEditor::timerCallback()
{
    auto getStringForLabel = m_audioProcessor.apvts.getRawParameterValue("METEROPTIONS")->load();
    labelMeterType.setText(juce::String(getStringForLabel), juce::NotificationType::dontSendNotification);
    repaint();

    //barMeterComponent.setLevelValues(m_audioProcessor.m_nChannelPeakLevels);
//    barMeterComponent.updateEverything();
//    barMeterComponent.repaintEverything();
}