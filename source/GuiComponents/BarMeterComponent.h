//
// Created by Decca on 2024-04-19.
//

#ifndef FLATSVILLEMETER_BARMETERCOMPONENT_H
#define FLATSVILLEMETER_BARMETERCOMPONENT_H
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include <vector>

#include "BarMeterChannelInfoTextBox.h"
#include "BarMeterBar.h"
#include "BarMeterHelper.h"
#include "TinyStripComponent.h"
//this "MeterComponent" will be staying @ plugin Editor.

namespace Gui
{

    class BarMeterComponent final:
        public juce::Component,
        private juce::Timer
    {
    public:
        BarMeterComponent();
        explicit BarMeterComponent(const juce::AudioChannelSet& channelFormat);
        ~BarMeterComponent() override;

        //can be called manually or internally.
        //see setRefreshRate, useInternalTiming
        void refresh(bool forceRefresh = false);

        //reset peak hold, resetMeters
        void reset();
        void updateMeters();
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
        //void drawEverything(juce::Graphics& g);


        float M_RANDOMVALUEFORDEBUGGING = 0.0f;
    private:

        bool useInternalTimer = true;

        std::vector<float> inputLevelsInDecibels {0.0f, 0.0f};

        Gui::BarMeterBar horizontalMeterBar0;
        Gui::BarMeterBar horizontalMeterBar1;
        Gui::BarMeterChannelInfoTextBox channelInfoTextBox0;
        Gui::BarMeterChannelInfoTextBox channelInfoTextBox1;
        Gui::TinyStripComponent tinyStripComponent;
        Gui::MeterColours meterColours;
        Options meterOptions;
        //  =====
        std::vector<float> m_levelValues = {Gui::Constants::kLevelMinInDecibels, Gui::Constants::kLevelMinInDecibels};
        float m_levelValueChannel0 = Gui::Constants::kLevelMinInDecibels;
        float m_levelValueChannel1 = Gui::Constants::kLevelMinInDecibels;
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
