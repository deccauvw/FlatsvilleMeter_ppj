#pragma once
#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : m_audioProcessor(p), AudioProcessorEditor(&p)
      //initialize parameters := dials
{
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
    comboBoxMeterType.addItem("RMS", 1);
    comboBoxMeterType.addItem("VU", 2);
    comboBoxMeterType.addItem("PEAK", 3);
    comboBoxMeterType.setSelectedId(3); //default selection
    comboBoxMeterType.setBounds(Gui::Constants::kComboBoxPositionX, Gui::Constants::kComboBoxPositionY,Gui::Constants::kComboBoxWidth, Gui::Constants::kComboBoxHeight);
    comboBoxMeterType.addListener(this);
    comboBoxMeterType.onChange = [this]{ comboBoxChanged(&comboBoxMeterType);};
    addAndMakeVisible(comboBoxMeterType);


    //initialize parameters := label for metertype display (old tinystrip)
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
    startTimerHz((int)Gui::Constants::kInitialRefreshRateHz);
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
    if (comboBoxThatHasChanged == &comboBoxMeterType)
    {
        auto key = comboBoxMeterType.getSelectedId();
        jassert(key >= 0 || key <= 2);
        this->MeterBallisticsTypeKey = key;
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
    auto s = helpers.MeterBallisticsType[MeterBallisticsTypeKey];
    labelMeterType.setText(juce::String(s), juce::NotificationType::dontSendNotification);
    barMeterComponent.setMbtData(s);
    repaint();

}