//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"

namespace Gui
{
    BarMeterComponent::BarMeterComponent()
    {
        //add dummy vsfv here
        auto dummySupplierFn = []()->float {return 0.0f;};

        horizontalMeterBar0(0, dummySupplierFn);

    }

    BarMeterComponent::BarMeterComponent (const std::vector<std::function<float()>>& vsfv, juce::AudioProcessorValueTreeState& apvts):
                                            valueSupplierFnVector(vsfv),
                                             horizontalMeterBar0(0, packagedValueSuppliers(vsfv, kMeterBallisticsTypeDefault)),
                                             horizontalMeterBar1(1, packagedValueSuppliers(vsfv, kMeterBallisticsTypeDefault)),
                                            channelOverloadLed0(0, packagedValueSuppliers(vsfv, kMeterBallisticsTypeDefault)),
                                            channelOverloadLed1(1, packagedValueSuppliers(vsfv, kMeterBallisticsTypeDefault)),
                                            tinyStripComponent([&apvts]()->float{return apvts.getRawParameterValue("GAIN")->load();}),
                                            m_useInternalTimer(false)
    {
        useInternalTiming(m_useInternalTimer); //startTimerHz
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
            &channelOverloadLed0,
            &channelOverloadLed1,
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
        channelOverloadLed0.paint(g);
        channelOverloadLed1.paint(g);
        tinyStripComponent.paint(g);
    }
    void BarMeterComponent::updateEverything()
    {

        this->horizontalMeterBar0.refreshMeterLevel();
        this->horizontalMeterBar1.refreshMeterLevel();

        this->channelOverloadLed0.setChannelInfo(0);
        this->channelOverloadLed1.setChannelInfo(1);

        //this->tinyStripComponent.repaint();
    }
    void BarMeterComponent::repaintEverything() //flags every component as "dirty"
    {
        //printf("~~repaint Everything\n"); pass
        //flag everything with repaint
        this->horizontalMeterBar0.repaint();
        this->horizontalMeterBar1.repaint();
        this->channelOverloadLed0.repaint();
        this->channelOverloadLed1.repaint();
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
        m_useInternalTimer = isTimerInternal;
        if (isTimerInternal)
        {
            stopTimer();
            startTimerHz ((int)Constants::kInitialRefreshRateHz);
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
//    std::function<float()> BarMeterComponent::packagedValueSuppliers (std::vector<std::function<float()>>& funcVector, MeterBallisticsType mbt, int channel)
//    {
//        return std::function<float()>();
//    }

    std::function<float()> BarMeterComponent::packagedValueSuppliers(
        const std::vector<std::function<float()>>& funcVector,
        MeterBallisticsType mbt)
    {
        /*
         * funcVector will have N-types of value Suppliers as element.
         * .at(0) : peak
         * .at(1) : rms
         * .at(2) : vu
         */
        auto mbtKey = MeterBallisticsTypeKey.at(mbt);
        auto returningFunction =  [mbtKey, &funcVector]() -> float
        {
            return funcVector[mbtKey]();
        };

        return returningFunction;
    }
} // Gui