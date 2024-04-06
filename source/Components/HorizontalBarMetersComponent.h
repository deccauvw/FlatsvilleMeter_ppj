//
// Created by decca on 2024-04-05.
//

#ifndef FLATSVILLEMETER_PPJ_HORIZONTALBARMETERCOMPONENT_H
#define FLATSVILLEMETER_PPJ_HORIZONTALBARMETERCOMPONENT_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "HorizontalBarMeterChannel.h"
#include "HorizontalBarMeterHelpers.h"

namespace LevelMeter
{
    class HorizontalBarMeterChannel;

    //@brief component containing one or more meters
    //after setting the channel format it
    //will auto. create the needed meters and give them proper names.
    class HorizontalBarMetersComponent final : public juce::Component, private juce::Timer
    {
    public:
        //@brief default constructor
        HorizontalBarMetersComponent();

        //@brief constr. with meter options
        //@param meteroptions the options to use with the meters and the label strip
        explicit HorizontalBarMetersComponent (const Options& meterOptions);

        //@brief constr. which accepts a channel format.
        //this constr. will auto set up the panel with the right amount of meters, named according to the channel format.
        //channelformat := the channel format to use to init. the panel
        explicit HorizontalBarMetersComponent (const juce::AudioChannelSet& channelFormat);

        HorizontalBarMetersComponent (const Options& meterOptions, const juce::AudioChannelSet& channelFormat);

        //destr
        ~HorizontalBarMetersComponent() override;

        // ==========================================================
        //redraw the meteres panel
        //can be manually or internally
        void refresh (bool forceRefresh = false);

        void reset();

        //reset all meters
        //reset all meterse to zero but not the peak hold
        void resetMeters();

        //clear the level of the meters
        void clearMeters();

        void createMeters(const juce::AudioChannelSet& channelFormat, const std::vector<juce::String>& channelNames);
        //reset all the peak hold indicators and peakValues
        void resetPeakHold();

        // ==========================================================
        void setInputLevel (int channel, float value);

        //number of channels
        //
        void setNumChannels (int numChannels, const std::vector<juce::String>& channelNames = {});

        void setChannelFormat (const juce::AudioChannelSet& channels, const std::vector<juce::String>& channelNames = {});

        void setChannelNames (const std::vector<juce::String>& channelNames);

        void setOptions (const Options& meterOptions);

        void setRefreshRate (float refreshRate);

        void setMeterSegments (const std::vector<SegmentOptions>& segmentsOptions);

        void setDecay (float decay_ms);

        void userGradients (bool useGradients);

        void setLabelStripPosition (LabelStripPosition labelStripPosition);

        void setFont (const juce::Font& font);
        // ==========================================================
        [[nodiscard]] int getNumChannels() const noexcept;

        [[nodiscard]] int getAutoSizedPanelWidth() const noexcept;

        // ==========================================================

        void useInternalTiming (bool useInternalTiming) noexcept;

        void showHeader (bool showHeader);

        void showValue (bool showValue);

        //enable or disable the panel
        void enable (bool enabled = true);

        void showTickMarks (bool showTIckMarks);

        void visibilityChanged() override;

        void lookAndFeelChanged() override;

        void paint (juce::Graphics& g) override;

        void resized() override;
        // ==========================================================
    private:

    Options meters_meterOptions {};
    std::vector<SegmentOptions> meters_segmentsOptions = MeterScales::getMeterScaleDefault();

    using MetersType = juce::OwnedArray<HorizontalBarMeterChannel>;
    MetersType meters_meterChannels {};
    HorizontalBarMeterChannel meters_labelStrip {};
    LabelStripPosition  meters_labelStripPosition = LabelStripPosition::upper;
    juce::AudioChannelSet meters_channelFormat = juce::AudioChannelSet::stereo();

    bool meters_useInternalTimer = true;
    juce::Font meters_font;
    int meters_autoSizedPanelWidth = 0;
    juce::Colour meters_backgroundColour = juce::Colours::black;

    //private methods
    void TimerCallback() override;
    void setColours();
    void createMeter (const juce::AudioChannelSet& channelFormat, const std::vector<juce::String>& channelNames);
    void deleteMeters();
    [[nodiscard]] HorizontalBarMeterChannel* getMeterChannel (int meterIndex) noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalBarMetersComponent)
};
} // LevelMeter

#endif //FLATSVILLEMETER_PPJ_HORIZONTALBARMETERCOMPONENT_H
