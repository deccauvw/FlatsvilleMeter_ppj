//
// Created by prmir on 2024-04-01.
//

//#ifndef SOURCE_IML_HORIZONTALBARMETERLEVEL_H
//#define SOURCE_IML_HORIZONTALBARMETERLEVEL_H
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "HorizontalBarMeterHelpers.h"
#include "HorizontalBarMeterSegment.h"


namespace LevelMeter {

	class HorizontalBarMeterLevel final {
	public:
		//@brief constructor
		HorizontalBarMeterLevel();

		//@brief destructor
		~HorizontalBarMeterLevel();
		// ==============================================================================================
		//@brief reset the meter (not the peak hold)
		//@see resetPeakHold
		void reset();

		//@brief Reset the peak hold level
		//@see getPeakHoldLevel, isPeakValueVisible, setPeakValueVisible, showPeakHold, showValue, isPeakHoldEnabled
		void resetPeakHold();

		//@brief calc. the actual meter level (ballistics included)
		/*
		 * calculate the meter's level incl.  ballistics.
		 * instant attack, decayed release.
		 * @see getMeterLevel, setDecay
		 */
		void refreshMeterLevel();

		//@brief check if the peak Value part is visible
		/*
		 * the peak value will be shown below the meter (in dB);
		 * it's the same level as the peak hold bar.
		 * return True, if the peak hold value part is visible
		 * @see showValue, resetPeakHold
		 */
		[[nodiscard]] bool isPeakValueVisible() const noexcept;
		//  ==============================================================================================
//setters
		//brief Set the level of the meter
		/*
		 * level is set from the audio engine.
		 * very likely called from teh audiothread(buffer)
		 *
		 * @param newLevel := the level from the audio engine.
		 * @see getInputLevel
		 */
		void setInputLevel(float newLevel);

		//@brief set the meters' options
		//the option determines the appearance and func. of the meter
		//@param meterOptions := Meter options to use
		void setMeterOptions(const Options& meterOptions);

		//@brief sets the meters' refrehs rate
		//set this to optim. the meter's decay rate
		void setRefreshRate(float refreshRateHz) noexcept;

		//@brief Set meter decay
		//param decay_ms := meter decay in ms
		void setDecay(float decay_ms);

		//@brief set the segments the meter is made out of
		//all segments have a level range, a range within the meter and a colour( or gradient)
		//segmentsOptions := the segments options to create the segments width
		void setMeterSegments(const std::vector<SegmentOptions>& segmentsOptions);

		//@brief set the bounds of the 'meter' part of the meter
		//@bounds := the bounds to use for the meter part of the meter
		void setMeterBounds(const juce::Rectangle<int>& bounds);

		//  ==============================================================================================
//getters
		[[nodiscard]] float getInputLevel();

		[[nodiscard]] float getMeterLevel() const noexcept;

		[[nodiscard]] float getPeakHoldLevel() const noexcept;

		[[nodiscard]] float getDecay() const noexcept;

		[[nodiscard]] float getRefreshRate() const noexcept;

		[[nodiscard]] juce::Rectangle<int> getValueBounds() const noexcept;

		[[nodiscard]] juce::Rectangle<int> getMeterBounds() const noexcept;

		[[nodiscard]] juce::Rectangle<int> getLevelBounds() const noexcept;

		[[nodiscard]] juce::Rectangle<int> getDirtyBounds() noexcept;
		//  ==============================================================================================
//JUCE graphics context district
		void drawMeter(juce::Graphics &g, const MeterColours& meterColours);

		void drawPeakValue(juce::Graphics& g, const MeterColours& meterColours) const;


	private:
		Options meterLevel_Options;
		std::vector<SegmentOptions>                 meterLevel_segmentOptions = MeterScales::getMeterScaleDefault();
		std::vector<HorizontalBarMeterSegment>      meterLevel_segments{}; //list of meter segments
		juce::Range<float>          meterLevel_meterRange{Constants::kLevelMaxInDecibels,
		                                                  Constants::kLevelMinInDecibels};
		juce::Rectangle<int>        meterLevel_valuesBounds    = {}; // Bounds of the value areas (plural)
		juce::Rectangle<int>        meterLevel_meterBounds    = {}; // Bounds of the meter area
		juce::Rectangle<int>        meterLevel_levelBounds    = {}; //Bounds of the level area
		juce::Rectangle<int>        meterLevel_PeakLvBoundsTop  = {}; //Bounds of the upmost PeakValueDisplaying Pos
		const int offsetForSecondChannelPeakValueDrawing = 156 - 53;
														//5356 52
		//meter Levels
		std::atomic<float> meterLevel_inputLevel{0.0f}; // audio level
		std::atomic<bool>  meterLevel_LevelRead{false};
		float              meterLevel_meterLevelDecibels  = Constants::kLevelMinInDecibels; // init. meter level
		bool               meterLevel_peakHoldDirty       = false;
		float              meterLevel_decayCoefficient    = 0.0f;
		float              meterLevel_refreshPeriod_ms    = (1.0f / meterLevel_Options.refreshRateHz) * 1000.0f;
		int                meterLevel_previousRefreshTime = 0;
		float              meterLevel_decayRate           = 0.0f; //Decay Rate in dB/ms

		[[nodiscard]] float getDecayedLevel (float newLevelDecibels);
		[[nodiscard]] float getDecayedLevelLinear (float newLevelDecibels);
		void deriveDecayCoefficient(const Options& meterOptions);
		void syncMeterOptions();

		JUCE_LEAK_DETECTOR(HorizontalBarMeterLevel)
	};

} // LevelMeter

//#endif //SOURCE_IML_HORIZONTALBARMETERLEVEL_H
