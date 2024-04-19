//
// Created by orstphone@github on 2024-04-01.
//
#pragma once
#include "HorizontalBarMeterSegment.h"

namespace LevelMeter {
	HorizontalBarMeterSegment::HorizontalBarMeterSegment(const Options &meterOptions,
	                                                     const SegmentOptions &segmentOptions) {
		setSegmentOptions(segmentOptions);
		setMeterOptions(meterOptions);
	}

	HorizontalBarMeterSegment::~HorizontalBarMeterSegment() =default;

	void HorizontalBarMeterSegment::setLevel(float levelDecibels) {
		if(juce::approximatelyEqual(levelDecibels, meterSegment_currentLevelDecibels)){
			meterSegment_currentLevelDecibels = levelDecibels;
		}

		if(levelDecibels > meterSegment_peakHoldLevelDecibels){
			meterSegment_peakHoldLevelDecibels = levelDecibels;
			updatePeakHoldBounds();
		}
	}
	// ==============================================================================================

	void HorizontalBarMeterSegment::setMeterBounds(juce::Rectangle<int> meterBounds) {
		meterSegment_meterBounds = meterBounds;
		auto floatBounds = meterBounds.toFloat();
		const auto segmentBounds = floatBounds.withY(floatBounds.getY() + floatBounds.proportionOfHeight(1.0f - meterSegment_segmentOptions.meterRange.getEnd()))
				.withHeight(floatBounds.proportionOfHeight(meterSegment_segmentOptions.meterRange.getLength()));
		meterSegment_segmentBounds = segmentBounds;
		updateLevelBounds();
		updatePeakHoldBounds();

		if(meterSegment_meterOptions.useGradient){
			meterSegment_gradientFill =
					juce::ColourGradient(meterSegment_segmentOptions.colourSegment, segmentBounds.getBottomLeft(),
										 meterSegment_segmentOptions.colourSegmentNext,
										 segmentBounds.getTopLeft(),
										 false);
		}
		meterSegment_isDirty = true;
	}

	void HorizontalBarMeterSegment::setSegmentOptions(LevelMeter::SegmentOptions segmentOptions) {
		//check level range validity
		jassert(segmentOptions.levelRange.getLength() > 0.0f); //NOLINT
		//check meter range validity [0, 1]
		jassert(segmentOptions.meterRange.getStart() >= 0.0f &&
		        segmentOptions.meterRange.getEnd() <= 1.0f &&
		        segmentOptions.meterRange.getLength() > 0.0f);

	}

	void HorizontalBarMeterSegment::setMeterOptions(const LevelMeter::Options &meterOptions) {
		meterSegment_meterOptions = meterOptions;

		//find all tick marks in this segment range
		meterSegment_tickMarks.clear();
		for(const auto& tick: meterOptions.tickMarksInDecibels){
			if(Helpers::containsUpTo(meterSegment_segmentOptions.levelRange, tick))
				meterSegment_tickMarks.emplace_back(tick);
		}
		meterSegment_isDirty = true;
	}

	void HorizontalBarMeterSegment::updateLevelBounds() {
		if(meterSegment_segmentBounds.isEmpty()) {
			return;
		}
			const auto levelRatio = std::clamp((meterSegment_currentLevelDecibels - meterSegment_segmentOptions.levelRange.getStart()) / meterSegment_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
			const auto levelBounds = meterSegment_segmentBounds.withTop(meterSegment_segmentBounds.getY() + static_cast<float>(meterSegment_meterBounds.proportionOfHeight(1.0f - levelRatio)));

			if(meterSegment_drawnBounds == levelBounds)
				return;

			meterSegment_drawnBounds = levelBounds;
			meterSegment_isDirty = true;

	}

	void HorizontalBarMeterSegment::updatePeakHoldBounds() {
		auto peakHoldBounds = juce::Rectangle<float>();
		if(Helpers::containsUpTo(meterSegment_segmentOptions.levelRange, meterSegment_peakHoldLevelDecibels)) {
			const auto peakHoldRatio = std::clamp(
					(meterSegment_peakHoldLevelDecibels - meterSegment_segmentOptions.levelRange.getStart()) /
					meterSegment_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
			if (peakHoldRatio == 0.0f)
				return;

			const auto peakHoldY = meterSegment_segmentBounds.getY() +
			                       meterSegment_segmentBounds.proportionOfHeight(1.0f - peakHoldRatio);
			peakHoldBounds = meterSegment_segmentBounds.withTop(peakHoldY).withHeight(Constants::kLevelPeakHoldHeight);
		}
		if(peakHoldBounds ==meterSegment_drawnPeakHoldBounds)
			return;

		meterSegment_peakHoldBounds = peakHoldBounds;
		meterSegment_isDirty = true;
	}
	// ==============================================================================================

	void HorizontalBarMeterSegment::draw(juce::Graphics &g, const MeterColours &meterColours) {
		meterSegment_isDirty = false;

		if(!meterSegment_meterOptions.tickMarksOnTopOfTheBar)
			drawTickMarks(g, meterColours);

		if(!meterSegment_drawnBounds.isEmpty()) {
			if (meterSegment_meterOptions.useGradient)
				g.setGradientFill(meterSegment_gradientFill);
			else
				g.setColour(meterSegment_segmentOptions.colourSegment);

			g.fillRect(meterSegment_drawnBounds);
		}

		if(meterSegment_meterOptions.tickMarksOnTopOfTheBar)
			drawTickMarks(g, meterColours);

		if(meterSegment_meterOptions.showPeakHoldIndicator && !meterSegment_peakHoldBounds.isEmpty()){
			g.setColour(meterColours.colourPeakHold);
			g.fillRect(meterSegment_peakHoldBounds);
			meterSegment_drawnPeakHoldBounds = meterSegment_peakHoldBounds;
		}

	}

	void HorizontalBarMeterSegment::drawTickMarks(juce::Graphics &g, const MeterColours &meterColours) {
		g.setColour(meterColours.colourTickMark);

		for(const auto& tickMark: meterSegment_tickMarks){
			if((tickMark <= meterSegment_currentLevelDecibels)&& !meterSegment_meterOptions.tickMarksOnTopOfTheBar)
				continue;

			const auto tickMarkLevelRatio = std::clamp((tickMark - meterSegment_segmentOptions.levelRange.getStart()) / meterSegment_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
			const auto tickMarkY = meterSegment_segmentBounds.getY() + meterSegment_segmentBounds.proportionOfHeight(1.0f - tickMarkLevelRatio);
			const auto tickMarkBounds =
					juce::Rectangle<float>(meterSegment_segmentBounds.getX(), tickMarkY, meterSegment_segmentBounds.getWidth(), static_cast<float>(Constants::kTickMarkWidth));
			g.fillRect(tickMarkBounds);
		}
	}
	// ==============================================================================================

	void HorizontalBarMeterSegment::drawLabels(juce::Graphics &g, const MeterColours &meterColours) const {
		g.setColour(meterColours.colourText);
		const float fontsize = juce::jlimit(1.0f, 15.0f, meterSegment_meterBounds.getHeight() / 4.0f); //set font size proportionally NOLINT
		g.setFont(fontsize);

		for(const auto& tickMark : meterSegment_tickMarks){
			const auto tickMarkLevelRatio = std::clamp((tickMark - meterSegment_segmentOptions.levelRange.getStart()) / meterSegment_segmentOptions.levelRange.getLength(), 0.0f, 1.0f);
			const auto tickMarkY = meterSegment_segmentBounds.getY() + meterSegment_segmentBounds.proportionOfHeight(1.0f - tickMarkLevelRatio);
			const auto labelBounds = juce::Rectangle<float> (meterSegment_segmentBounds.getX(), tickMarkY - (fontsize / 2.0f), meterSegment_segmentBounds.getWidth(), fontsize);
			g.drawFittedText(juce::String(std::abs(tickMark)), labelBounds.reduced(Constants::kLabelStripTextPadding, 0).toNearestInt(), juce::Justification::topLeft, 1);
		}
	}

	juce::Rectangle<float> HorizontalBarMeterSegment::getSegmentBounds() const noexcept {
		return meterSegment_segmentBounds;
	}

	void HorizontalBarMeterSegment::resetPeakHold() noexcept {
		meterSegment_peakHoldBounds.setHeight(0);
		meterSegment_peakHoldLevelDecibels = Constants::kLevelMinInDecibels;
		meterSegment_drawnPeakHoldBounds = meterSegment_peakHoldBounds;
		meterSegment_isDirty = true;
	}

	float HorizontalBarMeterSegment::getPeakHold() const noexcept {
		return meterSegment_peakHoldLevelDecibels;
	}

	bool HorizontalBarMeterSegment::isDirty() const noexcept {
		return meterSegment_isDirty;
	}

	Options HorizontalBarMeterSegment::getMeterOptions() const {
		return meterSegment_meterOptions;
	}
} // LevelMeter