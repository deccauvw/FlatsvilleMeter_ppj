//
// Created by orstphone@github on 2024-04-01.
//
#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include "HorizontalBarMeterHelpers.h"



namespace LevelMeter {

	class HorizontalBarMeterSegment final{
	public:
		HorizontalBarMeterSegment(const Options& meterOptions, const SegmentOptions& segmentOptions);
		~HorizontalBarMeterSegment();
		// ==============================================================================================
		//@brief set the level in dB
		void setLevel(float levelDecibels);

		//@brief set the bounds of the total meter (all segments)
		void setMeterBounds(juce::Rectangle<int> meterBounds);
		void setSegmentOptions(SegmentOptions segmentOptions);
		void setMeterOptions(const Options& meterOptions);
		// ==============================================================================================
		//@brief get the bounding box of this segment
		[[nodiscard]] juce::Rectangle<float> getSegmentBounds() const noexcept;

		//reset the peak hold
		void resetPeakHold() noexcept;

		//get the peak hold lv.
		[[nodiscard]] float getPeakHold() const noexcept;

		//check if the segment needs to be redrawn (dirty)
		[[nodiscard]] bool isDirty() const noexcept;

		[[nodiscard]] Options getMeterOptions() const;
		// ==============================================================================================
		//@brief draw the segment
		void draw(juce::Graphics& g, const MeterColours& meterColours);


		// ==============================================================================================
	private:
		SegmentOptions meterSegment_segmentOptions {};
		Options meterSegment_meterOptions {};
		std::vector<float> meterSegment_tickMarks{};
		juce::Rectangle<int> meterSegment_meterBounds{};
		juce::Rectangle<float> meterSegment_segmentBounds{};
		juce::Rectangle<float> meterSegment_drawnBounds{};
		juce::Rectangle<float> meterSegment_peakHoldBounds{};
		juce::Rectangle<float> meterSegment_drawnPeakHoldBounds{};
		juce::ColourGradient meterSegment_gradientFill{};

		float meterSegment_currentLevelDecibels = Constants::kLevelMinInDecibels;
		float meterSegment_peakHoldLevelDecibels = Constants::kLevelMinInDecibels;
		bool meterSegment_isDirty = false;
			// ==============================

		void updateLevelBounds();
		void updatePeakHoldBounds();
		void drawTickMarks(juce::Graphics& g, const MeterColours& meterColours);
		void drawLabels(juce::Graphics& g, const MeterColours& setColours) const;

	};
} // LevelMeter

