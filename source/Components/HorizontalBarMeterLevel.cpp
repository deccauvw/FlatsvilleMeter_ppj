//
// Created by prmir on 2024-04-01.
//
#pragma once

#include "HorizontalBarMeterLevel.h"

namespace LevelMeter {
	HorizontalBarMeterLevel::HorizontalBarMeterLevel() {
		setMeterSegments(meterLevel_segmentOptions);
	}

	HorizontalBarMeterLevel::~HorizontalBarMeterLevel() =default;
//  ==============================================================================================



	void HorizontalBarMeterLevel::deriveDecayCoefficient(const Options &meterOptions) {
		meterLevel_Options.decayTimeMs = juce::jlimit(Constants::kLevelMinDecayMs, Constants::kLevelMaxDecayMs, meterOptions.decayTimeMs);
		meterLevel_Options.refreshRateHz = std::max(1.0f, meterOptions.refreshRateHz);
		meterLevel_refreshPeriod_ms = (1.0f / meterLevel_Options.refreshRateHz) * 1000.0f;

		meterLevel_decayRate = meterLevel_meterRange.getLength() / meterLevel_Options.decayTimeMs;
		meterLevel_decayCoefficient = std::pow(0.01f, (1000.0f / (meterLevel_Options.decayTimeMs * meterLevel_Options.refreshRateHz)));
	}

	void HorizontalBarMeterLevel::syncMeterOptions() {
		for (auto& segment : meterLevel_segments){
			segment.setMeterOptions(meterLevel_Options);
			//...
		}
		meterLevel_peakHoldDirty = true;
	}

	void HorizontalBarMeterLevel::reset() {
		meterLevel_inputLevel.store(0.0f);
		meterLevel_meterLevelDecibels = Constants::kLevelMinInDecibels;
		meterLevel_previousRefreshTime = 0;
	}

	void HorizontalBarMeterLevel::resetPeakHold() {
		for (auto& segment: meterLevel_segments)
			segment.resetPeakHold();
		meterLevel_peakHoldDirty = true;
	}

	void HorizontalBarMeterLevel::refreshMeterLevel() {
		meterLevel_meterLevelDecibels = getDecayedLevelLinear(getInputLevel());
		if(meterLevel_meterLevelDecibels > getPeakHoldLevel())
			meterLevel_peakHoldDirty = true;

		for(auto& segment : meterLevel_segments)
			segment.setLevel(meterLevel_meterLevelDecibels);
	}

	bool HorizontalBarMeterLevel::isPeakValueVisible() const noexcept {
		return !meterLevel_valuesBounds.isEmpty();
	}
//  ==============================================================================================
	void HorizontalBarMeterLevel::setInputLevel(float newLevel) {
		meterLevel_inputLevel.store(meterLevel_LevelRead.load() ? newLevel : std::max(meterLevel_inputLevel.load(), newLevel));
		meterLevel_LevelRead.store(false);
	}

//  =============================
	void HorizontalBarMeterLevel::setMeterOptions(const Options &meterOptions) {
		meterLevel_Options = meterOptions;
	}
//  =============================
	void HorizontalBarMeterLevel::setRefreshRate(float refreshRateHz) noexcept {
		meterLevel_Options.refreshRateHz = refreshRateHz;
		deriveDecayCoefficient(meterLevel_Options);
		syncMeterOptions();
	}
//  =============================
	void HorizontalBarMeterLevel::setDecay(float decay_ms) {
		meterLevel_Options.decayTimeMs = decay_ms;
		deriveDecayCoefficient(meterLevel_Options);
		syncMeterOptions();
	}
//  =============================
	void HorizontalBarMeterLevel::setMeterSegments(const std::vector<SegmentOptions> &segmentsOptions) {
		meterLevel_segments.clear();
		for(const auto& segmentOptions: segmentsOptions){
			meterLevel_segments.emplace_back(meterLevel_Options, segmentOptions);
			meterLevel_meterRange.setStart(std::min(meterLevel_meterRange.getStart(), segmentOptions.levelRange.getStart()));
			meterLevel_meterRange.setEnd(std::max(meterLevel_meterRange.getEnd(), segmentOptions.levelRange.getEnd()));
		}
		syncMeterOptions();
		deriveDecayCoefficient(meterLevel_Options);
	}
//  =============================
	void HorizontalBarMeterLevel::setMeterBounds(const juce::Rectangle<int> &bounds) {
		if(bounds == meterLevel_meterBounds)
			return;

		meterLevel_meterBounds = bounds;
		meterLevel_levelBounds = meterLevel_meterBounds;

		for(auto& segment : meterLevel_segments)
			segment.setMeterBounds(meterLevel_levelBounds);

		meterLevel_peakHoldDirty = true;
	}
//  ==============================================================================================
	float HorizontalBarMeterLevel::getInputLevel() {
		meterLevel_LevelRead.store(true);
		return meterLevel_meterRange.clipValue(juce::Decibels::gainToDecibels(meterLevel_inputLevel.load()));
	}
//  =============================
	float HorizontalBarMeterLevel::getMeterLevel() const noexcept {
		return meterLevel_meterLevelDecibels;
	}
//  =============================
	float HorizontalBarMeterLevel::getPeakHoldLevel() const noexcept {
		if(meterLevel_segments.empty())
			return Constants::kLevelMinInDecibels;

		return meterLevel_segments.at(0).getPeakHold();
	}
//  =============================
	float HorizontalBarMeterLevel::getDecay() const noexcept {
		return meterLevel_Options.decayTimeMs;
	}
//  =============================
	float HorizontalBarMeterLevel::getRefreshRate() const noexcept {
		return meterLevel_Options.refreshRateHz;
	}
//  =============================
	float HorizontalBarMeterLevel::getDecayedLevel(float newLevelDecibels) {
		const auto currentTime = static_cast<int>(juce::Time::getMillisecondCounter());
		const auto timePassed  = static_cast<float>(currentTime - static_cast<int>(meterLevel_previousRefreshTime));
		meterLevel_previousRefreshTime = currentTime;

		if (newLevelDecibels >= meterLevel_meterLevelDecibels)
			return newLevelDecibels;

		//more time has passed than the meter decay. ~= fully decayed
		if (timePassed > meterLevel_Options.decayTimeMs)
			return newLevelDecibels;

		if (meterLevel_meterLevelDecibels == newLevelDecibels)
			return newLevelDecibels;

		//convert that to refreshed frames
		auto numberOfFramePassed = static_cast<int> (std::round(
				(timePassed * meterLevel_Options.refreshRateHz) / 1000.0f)); //NOLINT

		auto     levelDecibels = meterLevel_meterLevelDecibels;
		for (int f             = 0; f < numberOfFramePassed; ++f)
			levelDecibels = newLevelDecibels + (meterLevel_decayCoefficient * (levelDecibels - newLevelDecibels));

		if (std::abs(levelDecibels - newLevelDecibels) < Constants::kLevelMinInDecibels)
			levelDecibels = newLevelDecibels;

		return levelDecibels;
	}
//  =============================
	float HorizontalBarMeterLevel::getDecayedLevelLinear(float newLevelDecibels) {
		const auto currentTime = static_cast<int>(juce::Time::getMillisecondCounter());
		const auto timePassed = static_cast<float>(currentTime - static_cast<int>(meterLevel_previousRefreshTime));

		meterLevel_previousRefreshTime = currentTime;
		if(newLevelDecibels >= meterLevel_meterLevelDecibels)
			return newLevelDecibels;

		return std::max(newLevelDecibels, meterLevel_meterLevelDecibels - (timePassed * meterLevel_decayRate));
	}
//  =============================
	juce::Rectangle<int> HorizontalBarMeterLevel::getValueBounds() const noexcept {
		return meterLevel_valuesBounds;
	}
//  =============================
	juce::Rectangle<int> HorizontalBarMeterLevel::getMeterBounds() const noexcept {
		return meterLevel_meterBounds;
	}
//  =============================
	juce::Rectangle<int> HorizontalBarMeterLevel::getLevelBounds() const noexcept {
		return meterLevel_levelBounds;
	}
//  ==============================================================================================
	void HorizontalBarMeterLevel::drawMeter(juce::Graphics &g, const MeterColours &meterColours) {
		for(auto& segment : meterLevel_segments)
			segment.draw(g, meterColours);

		if(!meterLevel_valuesBounds.isEmpty())
			drawPeakValue(g, meterColours);
	}
//  =============================
	void HorizontalBarMeterLevel::drawPeakValue(juce::Graphics &g, const MeterColours &meterColours) const {
		if(meterLevel_valuesBounds.isEmpty())
			return;

		//get bounds for peakValue
		auto bound = meterLevel_PeakLvBoundsTop;

		//figure the channelInfo

		auto boundY = bound.getY();
		bound.setY(boundY + offsetForSecondChannelPeakValueDrawing);

		//acquire Peak value
		const auto peakDecibels = getPeakHoldLevel();

		//draw Peak Value
		if(peakDecibels > meterLevel_meterRange.getStart()) //if active & present & enough space is availble.
		{
			g.setColour(meterColours.colourText);
			g.drawFittedText(juce::String(peakDecibels), bound, juce::Justification::centredLeft, 1);
		}
	}

	juce::Rectangle<int> HorizontalBarMeterLevel::getDirtyBounds() noexcept {
		juce::Rectangle<int> dirtyBounds {};
		for(const auto& segment: meterLevel_segments)
		{
			if(segment.isDirty())
			{
				dirtyBounds = dirtyBounds.getUnion(segment.getSegmentBounds().toNearestIntEdges());
			}

			if(meterLevel_peakHoldDirty)
			{
				dirtyBounds = dirtyBounds.getUnion(meterLevel_valuesBounds);
				meterLevel_peakHoldDirty = false;
			}
			return dirtyBounds;
		}
	}
} // LevelMeter