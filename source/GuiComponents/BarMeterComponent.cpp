//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent (PluginProcessor& p): audioProcessor(p),
                                             horizontalMeterBar0(0, [&](){return audioProcessor.getLevelValuePeak(0);}),
                                             horizontalMeterBar1(1, [&](){return audioProcessor.getLevelValuePeak(1);}),
                                             channelInfoTextBox0(0, [&](){return audioProcessor.getLevelValuePeak(0);}),
                                             channelInfoTextBox1(1, [&](){return audioProcessor.getLevelValuePeak(1);}),
                                             tinyStripComponent([&](){
                                                                    auto value = audioProcessor.parameters.param_gain;
                                                                    std::vector<float> values = {value};
                                                                    return values;
                                                                }),
                                            useInternalTimer(false)
    {
        useInternalTiming(useInternalTimer); //startTimerHz
        //gainDbSlider = new juce::Slider ("SliderGain");

        //setChannelFormat()
//        addAndMakeVisible(tinyStripComponent);
//        addAndMakeVisible(horizontalMeterBar0);
//        addAndMakeVisible(horizontalMeterBar1);
//        addAndMakeVisible(channelInfoTextBox0);
//        addAndMakeVisible(channelInfoTextBox1);
    }

    BarMeterComponent::~BarMeterComponent()
    {
        stopTimer();
    }

    //===================================================================

    void BarMeterComponent::paint(juce::Graphics &g)
    {
        drawEverything(g);
    }
    //===================================================================

    std::vector<juce::Component*> BarMeterComponent::addAndMakeVisibleEverythingThrower()
    {
        std::vector<juce::Component*> listOfComponent = {
            &horizontalMeterBar0,
            &horizontalMeterBar1,
            &channelInfoTextBox0,
            &channelInfoTextBox1,
            &tinyStripComponent
        };
        return listOfComponent;
    }

//    template<typename T>
//    std::vector<T*> BarMeterComponent::addAndMakeVisibleEveryUniquePtrThrower()
//    {
//        std::vector<T*> listOfSliderControllers =
//        {
//            &gainDbSlider,
//            &gainDbLabel
//        };
//
//        return listOfSliderControllers;
//    }

    void BarMeterComponent::drawEverything(juce::Graphics &g)
    {
        horizontalMeterBar0.paint(g);
        horizontalMeterBar1.paint(g);
        channelInfoTextBox0.paint(g);
        channelInfoTextBox1.paint(g);
        tinyStripComponent.paint(g);
    }
    void BarMeterComponent::updateEverything()
    {
        float randomValue =audioProcessor.m_nChannelRmsLevels[0]; //for textbox stuff
        //printf("fed randomValue :: %3f...\n", randomValue );
        //printf("~~update Everything\n"); pass
        this->horizontalMeterBar0.refreshMeterLevel();
        this->horizontalMeterBar1.refreshMeterLevel();

        auto isOverloaded = [&](int ch){
            if(audioProcessor.m_nChannelPeakLevels.at(ch) > -0.0f)
                return 1;
            return 0;
        };

        this->channelInfoTextBox0.setChannelName(juce::String(isOverloaded(0)));
        this->channelInfoTextBox1.setChannelName(juce::String(isOverloaded(1)));
        //this->tinyStripComponent.repaint();
    }
    void BarMeterComponent::repaintEverything() //flags every component as "dirty"
    {
        //printf("~~repaint Everything\n"); pass
        //flag everything with repaint
        this->horizontalMeterBar0.repaint();
        this->horizontalMeterBar1.repaint();
        this->channelInfoTextBox0.repaint();
        this->channelInfoTextBox1.repaint();
        this->tinyStripComponent.repaint();
        //repaint();
    }
    //===================================================================
    //===================================================================
    //===================================================================
    //===================================================================

    void BarMeterComponent::setInputMeterLevelValueDecibels (const std::vector<float>& values)
    {
        m_levelValues = values;

        //repaint everything
        //repaint();

    }
    //===================================================================
    void BarMeterComponent::reset()
    {
        m_channelFormat = juce::AudioChannelSet::stereo();
    }
    //===================================================================
    //===================================================================
    void BarMeterComponent::resetPeakHold()
    {
        horizontalMeterBar0.resetPeakHold();
        horizontalMeterBar1.resetPeakHold();
    }
    //===================================================================
    void BarMeterComponent::setChannelFormat (const juce::AudioChannelSet& channelSet, const std::vector<juce::String>& channelNames)
    {
        if(channelSet.size() <= 0)
            return;
        m_channelFormat = channelSet;

        if(channelSet.size() != Gui::Constants::kChannelSize)
        {
            jassert("channelSize is not eq. to Stereo In Stereo Out");
        }
        setChannelNames(channelNames);
    }
    //===================================================================
    void BarMeterComponent::setChannelNames (const std::vector<juce::String>& channelNames)
    {
        juce::ignoreUnused(channelNames);
    }
    //===================================================================
    void BarMeterComponent::setRefreshRate (float refreshRate)
    {
        meterOptions.refreshRateHz = refreshRate;
    }
    //===================================================================
    void BarMeterComponent::useInternalTiming (bool isTimerInternal) noexcept
    {
        useInternalTimer = isTimerInternal;
        if (isTimerInternal)
        {
            stopTimer();
            startTimerHz (Constants::kInitialRefreshRateHz);
            //printf("using internal timing for BMC...\n");
        }
        else
        {
            stopTimer();
            //printf("using global timing for BMC...\n");
            return;
        }
    }
    //===================================================================
    void BarMeterComponent::setColours()
    {
        m_backgroundColour = meterColours.colourBackground;
    }
    //===================================================================
    void BarMeterComponent::timerCallback()
    {
        //printf("BarMeterComponent timerCallback called\n");
        //updateEverything();
        //repaintEverything();
        //printf("Global timerCallback ==================================\n");
    }
    //===================================================================
    void BarMeterComponent::setLevelValues(std::vector<float>& levelValues) //acquire value from audioprocessor.
    {
        jassert(m_levelValues.size() == levelValues.size());
        m_levelValues = levelValues;
    }
} // Gui