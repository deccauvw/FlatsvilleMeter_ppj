//
// Created by Orstphone on 2024-03-31.
//presumes the plugin is STEREO only
//

#ifndef SOURCE_IML_HORIZONTALBARMETERHELPERS_H
#define SOURCE_IML_HORIZONTALBARMETERHELPERS_H

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <vector>

namespace LevelMeter {
	//@brief various horizontal bar meter helper constants
	namespace Constants{
		static constexpr auto kGuiSizeWidth = 1012;
		static constexpr auto kGuiSizeHeight = 230;
		static constexpr auto kLevelPeakHoldHeight = 2;//height of the peak hold strip (in pixels)

		//textpadding is "spacing" between numeric values
		static constexpr auto kLabelStripTextPadding = 2;

		//header is virtually the "LR MS" thing.
		static constexpr auto kDefaultHeaderHeight = 25;
		static constexpr auto kHeaderChannelTypePositionX = 85;
		static constexpr auto kHeaderChannelTypePositionY_Top = 594;
		static constexpr auto kHeaderChannelTypePositionY_Btm = 131;
		static constexpr auto kHeaderChannelTypeHeight = 12;
		static constexpr auto kHeaderChannelTypeWidth = 12;
		static constexpr auto kHeaderChannelTypeFontHeight = 12.0f;
		static constexpr auto kHeaderMetricTypePositionX = 590;
		static constexpr auto kHeaderMetricTypePositionY = 106;
		static constexpr auto kHeaderMetricTypeHeight = 19;
		static constexpr auto kHeaderMetricTypeWidth = 16;
		static constexpr auto kHeaderMetricTypeFontHeight = 19.0f;

		//label strip is the "20 10 5 3 2 1 0 3" thing showing where the meter is indicating
		static constexpr auto kLabelstripPositionX = 114;
		static constexpr auto kLabelstripPositionY = 105;
		static constexpr auto kLabelstripFontHeight = 21.0f;

		//text jargon position
			//horizontal small display
		static constexpr auto kSmallDisplayPositionX = 657;
		static constexpr auto kSmallDisplayPositionY = 174;
		static constexpr auto kSmallDisplayWidth = 167;
		static constexpr auto kSmallDisplayHeight = 23;
		static constexpr auto kSmallDisplayFontHeight = 19.0f;
			//everything else will be added later.


		//bouncy meters reacting to the input signal following the ballistics.
		static constexpr auto kMeterBarWidth = 455;  //horizontal bar meter width(long side)
		static constexpr auto kMeterBarHeight = 8;   //horizontal bar meter height(short side)
		static constexpr auto kMeterPositionAx = 124;
		static constexpr auto kMeterPositionAy = 87;
		static constexpr auto kMeterPositionBx = 124;
		static constexpr auto kMeterPositionBy = 135;

		//level meter level range
		static constexpr auto kLevelMaxInDecibels = +3.0f;
		static constexpr auto kLevelMinInDecibels = -20.0f;
		static constexpr auto kLevelMinDecayMs = 100.0f;
		static constexpr auto kLevelMaxDecayMs = 4000.0f;
		static constexpr auto kLevelDefaultDecayMs = 1000.0f;
		static constexpr auto kLevelPeakHoldTimeMs = 3000.0f;

		//aesthetics for easy level indications
		static constexpr auto kTickMarkWidth = 2;
		static constexpr auto kTickMarkAlphaMax = 0.3f;
		static constexpr auto kTickMarkAlphaMin = 0.1f;

		//aesthetics for "0VU" indicator (flashy orb)
		//will be added later if possible


		//ID(name) for all components
		static constexpr auto kMetersId = "meters_panel";
		static constexpr auto kLabelstripId = "label_strip";
	}//namespace Constants
//  ==============================================================================================
	//Refs' struct Padding part is removed
//  ==============================================================================================
	//@brief Options defining the meter segments.
	struct SegmentOptions{
		//the range of the segment in dB
		juce::Range<float> levelRange{
			Constants::kLevelMinInDecibels, Constants::kLevelMaxInDecibels
		};
		//the range of the segment in the meter (0.0f ~ 1.0f : with 0.0f being the bottom of the meter)
		juce::Range<float> meterRange{
			0.0f, 1.0f
		};

		//colour of the segment
		juce::Colour colourSegment {
			juce::Colours::aliceblue
		};

		//second colour of the segment for gradient.
		juce::Colour colourSegmentNext{
			colourSegment.brighter()
		};
	};
//  ==============================================================================================
//@brief EVERY available meter options for aesthetics and functionality.
	struct Options{
		bool enabled = true;  //enable the meber
		bool headerEnabled = true;  //enable the header
		bool valueEnabled = true;  //enable the value
		float decayTimeMs = Constants::kLevelDefaultDecayMs;
		float refreshRateHz = 24.0f; //meter refresh rate when using internal timing;
		bool tickMarksEnabled = true; //show tickmarks := divider lines on meter at certain dB level point.
		bool tickMarksOnTopOfTheBar = true; //show the tickmarks above the level meter.
		bool useGradient = true; //use gradients for meter segments. if False use Solid Colours.
		bool showPeakHoldIndicator = true; //enable peakhold indicator.
		std::vector<float> tickMarksInDecibels = {3.0f, 2.0f, 1.0f, 0.0f, -1.0f, -2.0f, -3.0f, -5.0f, -7.0f, -10.0f, -20.0f}; //VU LU RMS Scale all in dB
	};
//  ==============================================================================================
//@brief ALL meter colours for the aesthetics
	struct MeterColours{
		juce::Colour colourBackground = juce::Colours::black; //default solid black
		juce::Colour colourLabelStrip = juce::Colours::orange; //-20 ~ 0dB
		juce::Colour colourLabelStripOverZero = juce::Colours::orangered; //+1 2 3dB
		juce::Colour colourText = juce::Colours::aliceblue; //indigo screen with aqua color
		juce::Colour colourTickMark = juce::Colours::white; //colour of tickMarks
		juce::Colour colourPeakHold = juce::Colours::orange; //colour of the peakhold indicator.
	};
//  ==============================================================================================
//@brief A class with static functions to create different types of meter scales.
	 class MeterScales {
	 private:
		MeterScales() = default;
	 public:
		 //default meter scale : 3 Segments. from -20 to 0 to 3dB
		 [[nodiscard]] static std::vector<SegmentOptions> getMeterScaleDefault() {
		    juce::Colour coloursForMeterRange_at20 = juce::Colours::steelblue; //colour of the bar under zero dB cross line
			juce::Colour coloursForMeterRange_at0 = juce::Colours::aliceblue; //colour of the bar under zero dB cross line
			juce::Colour coloursForMeterRange_at3 = juce::Colours::orange; //colour of the bar over zero dB cross line
			return {{{-20.0f, 0.0f}, {0.0f, 0.8696f}, coloursForMeterRange_at20, coloursForMeterRange_at0},
			        {{0.0f, 3.0f}, {0.8696f, 1.0f}, coloursForMeterRange_at0, coloursForMeterRange_at3}};
		 };
	 };
//  ==============================================================================================
//@brief Type indicating whether to notify the listeners or not
//enum class NotificationOptions not implemented
//  ==============================================================================================
//@brief Position of the label strip
	enum class LabelStripPosition{
		upper, lower, none
	};
//  ==============================================================================================
//@brief apply padding
	namespace Helpers
	{
		//[[nodiscard]] juce::Rectangle<int> applyPadding (const juce::Rectangle<int>& rectToPad, Padding paddingToApply) noexcept;
		[[nodiscard]] static constexpr bool containsUpTo(juce::Range<float> levelRange, float levelDecibels) noexcept;
	}

} // LevelMeter

#endif //SOURCE_IML_HORIZONTALBARMETERHELPERS_H
