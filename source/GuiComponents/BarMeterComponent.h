//
// Created by Decca on 2024-04-19.
//

#ifndef FLATSVILLEMETER_BARMETERCOMPONENT_H
#define FLATSVILLEMETER_BARMETERCOMPONENT_H
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "C:\JetBrains\AudioDevelopment\_007_FlatsvilleMeter_ppj\FlatsvilleMeter_ppj\source\PluginProcessor.h"
#include <vector>

#include "BarMeterChannelInfoTextBox.h"
#include "BarMeterBar.h"
#include "BarMeterHelper.h"
#include "TinyStripComponent.h"
//#include "GainKnob.h"
#include "FlatsLookAndFeel.h"


//this "MeterComponent" will be staying @ plugin Editor.

namespace Gui
{

    class BarMeterComponent final:
        public juce::Component,
        private juce::Timer
    {
    public:
        explicit BarMeterComponent(PluginProcessor&);
        //BarMeterComponent(PluginProcessor& p, const juce::AudioChannelSet& channelFormat);
        ~BarMeterComponent() override;


        //reset peak hold, resetMeters
        void reset();
        void resetPeakHold();

        //set inputlevel
        void setInputMeterLevelValueDecibels (const std::vector<float>& values);
        void setChannelFormat(const juce::AudioChannelSet& channelSet, const std::vector<juce::String>& channelNames = {});
        void setChannelNames(const std::vector<juce::String>& channelNames);
        void setRefreshRate(float refreshRate);


        /**
     * @brief Set the timing option to use (internal/external).
     *
     * When using internal timing, the panel will redraw (updateBarFigure) the meters automatically
     * using the updateBarFigure rate specified in setPanelRefreshRate.
     * When using external timing, the user has to do this manually with the 'updateBarFigure' method.
     *
     * @param useInternalTiming When set to true, the meter panel will update itself.
     *
     * @see updateBarFigure, setPanelRefreshRate
    */
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
        void setLevelValues(std::vector<float>& levelValues);

        int M_RANDOMVALUEFORDEBUGGING = 0;
    // ================================================================================================================================
    private:
        PluginProcessor& audioProcessor;

        bool useInternalTimer = true;

        //std::vector<float> m_LevelValuesInDecibels {0.0f, 0.0f};

        Gui::BarMeterBar horizontalMeterBar0;
        Gui::BarMeterBar horizontalMeterBar1;
        Gui::BarMeterChannelInfoTextBox channelInfoTextBox0;
        Gui::BarMeterChannelInfoTextBox channelInfoTextBox1;
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
