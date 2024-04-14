//
// Created by prmir on 2024-04-01.
//

//#ifndef SOURCE_IML_HORIZONTALBARMETERHEADER_H
//#define SOURCE_IML_HORIZONTALBARMETERHEADER_H
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "HorizontalBarMeterHelpers.h"
// ===================================================================================
/*
 *@brief Class responsible for the meters' HEADER part
 * HEADER is the part above the meter displaying following arguments: name, channelType(LR MS), MetricType(VU, RMS, LU etc)
 * the HEADER has 2 types of information to display, the channelType is slightly smaller than the Metrictype
 */
// ===================================================================================
namespace LevelMeter {

	class HorizontalBarMeterHeader final{
	public:
		//@brief constructor
		//param font : font to use in the header
		explicit HorizontalBarMeterHeader(juce::Font &font) noexcept: HorizontalBarMeterHeader({},
		                                                                                       juce::AudioChannelSet::unknown, "NA",
		                                                                                       font) {}
		//@brief constructor
		//param font : font to use in the header
		//param name : channel name to display in the header : channel name belongs to the track feeding the meter
		//param channelType : LR MS channelType to display in the header
		//param metricType : RMS VU LU etc. to display in the header.
		HorizontalBarMeterHeader(juce::String name, const juce::AudioChannelSet::ChannelType& channelType, juce::String metricType, juce::Font& font) noexcept :meterHeader_font(font), meterHeader_DescriptionChannelType(channelType), meterHeader_DescriptionMetricType(std::move(metricType)), meterHeader_ChannelName(std::move(name)){}

		~HorizontalBarMeterHeader();
// ===================================================================================
//setters
		void setChannelType(const juce::AudioChannelSet::ChannelType& channelType);

		void setMetricType(const juce::String& metricType);

		void setChannelName(const juce::String& name);

		//@brief Font to be used for the header, value, label strip etc.
			//param font := the font to use
		void setFont(const juce::Font& font);

        void setReferredWidth(float referredWidth) noexcept;
//getters

		//@brief Set the bounds of the "Header" part of the meter (essentially a virtual marquee)
		//@param bounds := bounds used for the header bounds
		//see getBounds
		void setBounds(const juce::Rectangle<int>& bounds) noexcept;

		[[nodiscard]] const juce::AudioChannelSet::ChannelType& getChannelType() const noexcept;

		[[nodiscard]] const juce::String getMetricType() const noexcept;

		[[nodiscard]] const juce::String getChannelName() const noexcept;

		[[nodiscard]] float getChannelNameWidth() const noexcept;

		[[nodiscard]] float getChannelTypeWidth() const noexcept;

		[[nodiscard]] float getMetricTypeWidth() const noexcept;

		//@brief get the information txt. displayed in the "header"
			//this can be either the channel name (when set  by the user)
			//or the channel type description(LRMS), or the metric type (LU, VU etc.)
			//returns the info text displayed in the header
			//see getChannelName, getChannelType, getMetricType
	//	[[nodiscard]] juce::String getChannelInfo() const noexcept;

		//@brief check weather a certain text will fit the width available using the meters' specified font.
		//param text := the info text to check the width of
		//param widthAvailable := the width available in the header part
		//@see get info, getChannelTypeWidth, getChannelNameWidth
		[[nodiscard]] bool textFits(const juce::String& text, int widthAvailable) const;

		[[nodiscard]] juce::Rectangle<int> getBounds() const noexcept;
// ===================================================================================
		//text drawing functions
		void drawHeaderWiseChannelType(juce::Graphics& g,const juce::String& channelType, const MeterColours& meterColours);

		void drawCenteredMetricType(juce::Graphics& g, const juce::String& metricType, const MeterColours& meterColours);

		void drawChannelName(juce::Graphics& g, const juce::String& channelName, const MeterColours& meterColours);
		//Everything JUCE
		void draw(juce::Graphics& g, bool meterActive, const MeterColours& meterColours);
// ===================================================================================
	private:
		juce::Font& meterHeader_font;

		//information
		juce::AudioChannelSet::ChannelType meterHeader_ChannelType = juce::AudioChannelSet::ChannelType::unknown;
		juce::String meterHeader_DescriptionChannelType = "";
		juce::String meterHeader_DescriptionMetricType = "";
		juce::String meterHeader_ChannelName = "";

		//Bounds
		juce::Rectangle<int> meterHeader_Bounds = {};
		float meterHeader_ChannelNameWidth = 0.0f;
		float meterHeader_ChannelTypeWidth = 0.0f;
		float meterHeader_MetricTypeWidth = 0.0f;
        float meterHeader_referredWidth = 0.0f;

		void calculateInfoWidth();

		JUCE_LEAK_DETECTOR(HorizontalBarMeterHeader)
	};

} // LevelMeter

//#endif //SOURCE_IML_HORIZONTALBARMETERHEADER_H
