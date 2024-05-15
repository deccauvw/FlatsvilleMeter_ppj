//
// Created by Decca on 2024-04-19.
//

#ifndef FLATSVILLEMETER_BARMETERCOMPONENT_H
#define FLATSVILLEMETER_BARMETERCOMPONENT_H
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"
//#include "C:\JetBrains\AudioDevelopment\_007_FlatsvilleMeter_ppj\FlatsvilleMeter_ppj\source\PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"

#include <vector>

//#include "../PluginProcessor.h"
#include "BarMeterBar.h"
#include "BarMeterHelper.h"
#include "BarMeterOverloadLed.h"
#include "TinyStripComponent.h"
//#include "FlatsLookAndFeel.h"
//#include "GainKnob.h"

//this "MeterComponent" will be staying @ plugin Editor.

namespace Gui
{
    //3 pairs of value Suppliers functor


    class BarMeterComponent final:
        public juce::Component,
        private juce::Timer
    {
    public:
        BarMeterComponent();
        explicit BarMeterComponent(const std::vector<std::function<float()>>& vsfv, juce::AudioProcessorValueTreeState& apvts);
        ~BarMeterComponent() override;

        //reset peak hold, resetMeters
        void reset();

        void resetPeakHold();

        void setInputMeterLevelValueDecibels (const std::vector<float>& values);

        void setChannelFormat(const juce::AudioChannelSet& channelSet, const std::vector<juce::String>& channelNames = {});

        void setChannelNames(const std::vector<juce::String>& channelNames);

        void setRefreshRate(float refreshRate);

        void useInternalTiming(bool useInternalTiming) noexcept;
        //void showTickMarks(bool showTickMarks);
        void paint (juce::Graphics& g)override;
        //void resized();
        void updateEverything();
        //void addAndMakeVisibleEverything();
        void repaintEverything();
        std::vector<juce::Component*> addAndMakeVisibleEverythingThrower();

//        template<typename T>
//        std::vector<T*> addAndMakeVisibleEveryUniquePtrThrower();
        void drawEverything(juce::Graphics& g);
        //void setLevelValues(std::vector<float>& levelValues);

        void setMbtData(Gui::MeterBallisticsType mbtInput);
    // ================================================================================================================================

        static std::function<float()> packagedValueSuppliers(const std::vector<std::function<float()>>& funcVector, MeterBallisticsType mbt);
        std::vector<std::function<float()>> valueSupplierFnVector; //VSFV
        // ================================================================================================================================

    private:

        MeterBallisticsType m_ballistics = MeterBallisticsType::VU;
        bool m_useInternalTimer;

        //std::vector<float> m_LevelValuesInDecibels {0.0f, 0.0f};

        Gui::BarMeterBar horizontalMeterBar0;
        Gui::BarMeterBar horizontalMeterBar1;
        Gui::BarMeterOverloadLed channelOverloadLed0;
        Gui::BarMeterOverloadLed channelOverloadLed1;
        Gui::TinyStripComponent tinyStripComponent;
//        std::unique_ptr<juce::Slider> gainDbSlider;
//        std::unique_ptr<juce::Slider> gainDbLabel;
        //  =============================================
        Gui::MeterColours meterColours;
        Options meterOptions;
        //  =============================================
        std::vector<float> m_levelValues = {Gui::Constants::kLevelMinInDecibels, Gui::Constants::kLevelMinInDecibels};
        juce::AudioChannelSet m_channelFormat = juce::AudioChannelSet::stereo();

        juce::Font m_font;
        juce::Colour m_backgroundColour = juce::Colours::black;
        void timerCallback()override;
        void setColours ();
        //[[nodiscard]] MeterChannel getMeterChannel (int meterIndex) noexcept;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BarMeterComponent)
    };

} // Gui

#endif //FLATSVILLEMETER_BARMETERCOMPONENT_H
