//
// Created by prmir on 2024-04-19.
//

#include "BarMeterComponent.h"
namespace Gui
{
    BarMeterComponent::BarMeterComponent ():
                                            valueSupplierFnVector(std::vector<std::function<float()>>{[]()->float{return 0.0f;}}),
                                            horizontalMeterBar0(0, []()->float{return 0.0f;}),
                                            horizontalMeterBar1(1, []()->float{return 0.0f;}),
                                            channelOverloadLed0(0, []()->float{return 0.0f;}),
                                            channelOverloadLed1(1, []()->float{return 0.0f;}),
                                            tinyStripComponent([]()->float{return 0.0f;}),
                                             //tinyStripLevelOptionDisplay([]()->float{return 0.0f;}),
                                            m_useInternalTimer(false)
    {
        useInternalTiming(m_useInternalTimer); //startTimerHz
    }

    BarMeterComponent::BarMeterComponent (const std::vector<std::function<float()>>& vsfv, juce::AudioProcessorValueTreeState& apvts):
                                            valueSupplierFnVector(vsfv),
                                             horizontalMeterBar0(0, packagedValueSuppliers(vsfv, Constants::kMeterBallisticsTypeDefault)),
                                             horizontalMeterBar1(1, packagedValueSuppliers(vsfv, Constants::kMeterBallisticsTypeDefault)),
                                            channelOverloadLed0(0, packagedValueSuppliers(vsfv, Constants::kMeterBallisticsTypeDefault)),
                                            channelOverloadLed1(1, packagedValueSuppliers(vsfv, Constants::kMeterBallisticsTypeDefault)),
                                            tinyStripComponent([&apvts]()->float{
                                                                                                auto v = apvts.getRawParameterValue("GAIN")->load();
                                                                                                //printf("%f\n",v);
                                                                                                return v * 100.f;
                                                                                            }),
//                                            tinyStripLevelOptionDisplay([&apvts]()->float{
//                                                                                                        auto v = apvts.getRawParameterValue("METEROPTIONS")->load();
//                                                                                                        return v;
//                                                                                                        }),
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
        //tinyStripLevelOptionDisplay.paint(g);
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
        //this->tinyStripLevelOptionDisplay.repaint();
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
        auto valueFn = supplySupplierFnFromMbtKeys(m_mbtValue);
        horizontalMeterBar0.setValueSupplier ([&valueFn](){return valueFn();});
        horizontalMeterBar1.setValueSupplier ([&valueFn](){return valueFn();});
        channelOverloadLed0.setValueSupplier([&valueFn](){return valueFn();});
        channelOverloadLed1.setValueSupplier([&valueFn](){return valueFn();});


    }
//    std::function<float()> BarMeterComponent::packagedValueSuppliers (std::vector<std::function<float()>>& funcVector, MeterBallisticsType mbt, int channel)
//    {
//        return std::function<float()>();
//    }

    void BarMeterComponent::setSupplierFnVector(std::function<float()>&& fn, std::string& keyName, int channel)
    {
        //how vectors are mapped
        //v[val][ch] == keyname (~val) 's ch-channel functor
        auto f = std::move(fn);
        int val = Gui::Helpers().MeterBallisticsType[keyName];
        std::tuple<int, int, std::function<float()>> t {val, channel, f};
        valueSupplierFnVector.push_back(t);
    }

    std::function<float()> BarMeterComponent::supplySupplierFnFromMbtKeys(std::string& mbtKey, int channel)
    {
//        MeterBallisticsType = std::unordered_map<std::string, int>
//            {
//                {"PEAK", 0},
//                {"RMS", 1},
//                {"VU", 2}
//            };
//      vector v <- keyVal (from MBT tag)  channel  functor
        auto &vect = valueSupplierFnVector;
        auto val = Gui::Helpers().MeterBallisticsType[mbtKey];

        auto it = std::find(
            vect.begin(),
            vect.end(),
            [&val, &channel](std::tuple<int, int, std::function<float()>>& e){v});


        auto returningFunction =  [mbtValue, &v]() -> float
        {
            return v.at(mbtValue)();
        };

        return returningFunction;
    }

    void BarMeterComponent::setMbtKey (int mbtKeyInput)
    {
        this->m_mbtValue = mbtKeyInput;
    }
} // Gui