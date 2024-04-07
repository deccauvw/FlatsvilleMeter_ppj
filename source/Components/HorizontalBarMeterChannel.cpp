#pragma once

#include "HorizontalBarMeterChannel.h"

namespace LevelMeter {
	HorizontalBarMeterChannel::HorizontalBarMeterChannel() noexcept =default;

	HorizontalBarMeterChannel::HorizontalBarMeterChannel(const Options &meterOptions, const juce::String &channelName,
	                                                     HorizontalBarMeterChannel::ChannelType channelType)
        :HorizontalBarMeterChannel()
    {
		setChannelName(channelName);
		setBufferedToImage(true);
		setChannelType(channelType);
		setOptions(meterOptions);
		//setIsLabelStrip(isLabelStrip);
		//setPadding(padding);
	}

	HorizontalBarMeterChannel::~HorizontalBarMeterChannel() =default;

//  ==========================================================================================================
	void HorizontalBarMeterChannel::reset() {
		meterChannel_Level.reset();
		setDirty();
	}

	void HorizontalBarMeterChannel::resetPeakHold() {
		meterChannel_Level.resetPeakHold();
	}

	void HorizontalBarMeterChannel::showPeakHold(bool showPeakHold) {
		meterChannel_options.showPeakHoldIndicator = showPeakHold;
		setOptions(meterChannel_options);
	}

	void HorizontalBarMeterChannel::showValue(bool showValue) {
		meterChannel_options.valueEnabled = showValue;
		setOptions(meterChannel_options);
	}

	void HorizontalBarMeterChannel::showHeader(bool headerVisible) {
		if(meterChannel_options.headerEnabled ==headerVisible)
			return;

		meterChannel_options.headerEnabled = headerVisible;

		resized();
		addDirty(meterChannel_header.getBounds());
	}

	void HorizontalBarMeterChannel::showTickMarks(bool showTickMarks) {
		meterChannel_options.tickMarksEnabled = showTickMarks;
		setOptions(meterChannel_options);
	}

	void HorizontalBarMeterChannel::showTickMarksOnTop(bool showTickMarksOnTop) {
		meterChannel_options.tickMarksOnTopOfTheBar = showTickMarksOnTop;
		setOptions(meterChannel_options);
	}

	void HorizontalBarMeterChannel::refresh(bool forceRefresh) {
		if(!isShowing())
			return;

		if(getBounds().isEmpty())
			return;

		if(meterChannel_active)
		{
			meterChannel_Level.refreshMeterLevel();
			const auto levelDirtyBounds = meterChannel_Level.getDirtyBounds();
			if(!levelDirtyBounds.isEmpty())
				addDirty(levelDirtyBounds);
		}

		if(isDirty()) {
			repaint(meterChannel_dirtyRect);
			setDirty(false);
		}
		else if (forceRefresh)
			repaint();

	}

	void HorizontalBarMeterChannel::useGradients(bool useGradients) {
		meterChannel_options.useGradient = useGradients;
		setOptions(meterChannel_options);
	}

    void HorizontalBarMeterChannel::setReferredTypeWidth (float referredTypeWidth) noexcept
    {
        meterChannel_header.setReferredWidth(referredTypeWidth);
    }
//  ==========================================================================================================
	void HorizontalBarMeterChannel::setRefreshRate(float refreshRateHz) {
		meterChannel_Level.setRefreshRate(refreshRateHz);
	}

	void HorizontalBarMeterChannel::setDecay(float decayMs) {
		meterChannel_Level.setDecay(decayMs);
	}

	void HorizontalBarMeterChannel::setTickMarks(const std::vector<float> &tickMarks) {
		meterChannel_options.tickMarksInDecibels = tickMarks;
		setOptions(meterChannel_options);
	}

	void HorizontalBarMeterChannel::setInputLevel(float inputLevel) {
		meterChannel_Level.setInputLevel(inputLevel);
	}

	void HorizontalBarMeterChannel::setOptions(const Options &meterOptions) {
		meterChannel_options = meterOptions;
		setVisible(meterOptions.enabled);
		setEnabled(meterOptions.enabled);
		meterChannel_Level.setMeterOptions(meterOptions);
		showHeader(meterOptions.headerEnabled);
		refresh(true);
	}

	void HorizontalBarMeterChannel::setMeterSegments(const std::vector<SegmentOptions> &segmentsOptions) {
		meterChannel_Level.setMeterSegments(segmentsOptions);
		setDirty();
	}

	void HorizontalBarMeterChannel::setFont(const juce::Font &font) {
		meterChannel_font = font;
		meterChannel_header.setFont(meterChannel_font);
		setDirty();
	}

	void HorizontalBarMeterChannel::setChannelName(const juce::String &channelName) {
		meterChannel_header.setChannelName(channelName);
		addDirty(meterChannel_header.getBounds());
	}

	void HorizontalBarMeterChannel::setChannelType(HorizontalBarMeterChannel::ChannelType channelType) {
		meterChannel_header.setChannelType(channelType);
	}

	float HorizontalBarMeterChannel::getDecay() const noexcept {
		return meterChannel_Level.getDecay();
	}

	float HorizontalBarMeterChannel::getChannelNameWidth() const noexcept {
		return meterChannel_header.getChannelNameWidth();
	}

	float HorizontalBarMeterChannel::getChannelTypeWidth() const noexcept {
		return meterChannel_header.getChannelTypeWidth();
	}

	juce::Rectangle<int> HorizontalBarMeterChannel::getLabelStripBounds() const noexcept {
		return meterChannel_Level.getMeterBounds().getUnion(meterChannel_header.getBounds());
	}

	bool HorizontalBarMeterChannel::nameFits(const juce::String &name, int widthAvailable) const {
		return meterChannel_header.textFits(name, widthAvailable);
	}

	void HorizontalBarMeterChannel::paint(juce::Graphics &g) {
		if(getLocalBounds().isEmpty())
			return;
		g.setFont(meterChannel_font);

		//draw the header part of the meter
		if(!meterChannel_header.getBounds().isEmpty() && meterChannel_options.headerEnabled){
			meterChannel_header.draw(g, true, meterChannel_meterColours);
		}
		drawMeter(g);

	}

	void HorizontalBarMeterChannel::resized() {
		auto meterBounds = getLocalBounds();

		//meter header
		meterChannel_header.setBounds(meterBounds.withHeight(0));
		if(meterChannel_options.headerEnabled)
		{
			meterChannel_header.setBounds(meterBounds.removeFromTop(Constants::kDefaultHeaderHeight));
		}
		meterChannel_Level.setMeterBounds(meterBounds);
	}

	void HorizontalBarMeterChannel::lookAndFeelChanged() {
		visibilityChanged();
	}

	void HorizontalBarMeterChannel::visibilityChanged() {
        setColours();
	}

	void HorizontalBarMeterChannel::setDirty(bool isDirty /*=true*/) noexcept {
        if(!isShowing())
            return;
        meterChannel_dirtyRect = {0, 0, 0, 0};
        if(isDirty)
            meterChannel_dirtyRect = getLocalBounds();
	}

	bool HorizontalBarMeterChannel::isDirty(const juce::Rectangle<int> &rectToCheck) const noexcept {
		if(rectToCheck.isEmpty())
            return !meterChannel_dirtyRect.isEmpty();
        return meterChannel_dirtyRect.intersects(rectToCheck);
	}

	void HorizontalBarMeterChannel::addDirty(const juce::Rectangle<int> &dirtyRect) noexcept {
        if(!isShowing())
            return;
        meterChannel_dirtyRect.getUnion(dirtyRect);
	}

	void HorizontalBarMeterChannel::drawMeter(juce::Graphics &g) {
        g.setColour(meterChannel_meterColours.colourBackground);
        g.fillRect(meterChannel_Level.getMeterBounds());
        meterChannel_Level.drawMeter(g, meterChannel_meterColours);
	}

	juce::Colour HorizontalBarMeterChannel::getColourFromLnf(int colourId, const juce::Colour &fallbackColour) const {
		if(isColourSpecified((colourId)))
            return findColour(colourId);
        if(getLookAndFeel().isColourSpecified((colourId)))
            return getLookAndFeel().findColour(colourId);
        return fallbackColour;
	}

	void HorizontalBarMeterChannel::setColours() {
        meterChannel_meterColours.colourBackground    = getColourFromLnf (backgroundColourId, juce::Colours::black);
        meterChannel_meterColours.colourText          = getColourFromLnf (textValueColourId, juce::Colours::white.darker (0.6f));  // NOLINT
        meterChannel_meterColours.colourTickMark      = getColourFromLnf (tickMarkColourId, juce::Colours::white.darker (0.3f).withAlpha (0.5f));  // NOLINT
        meterChannel_meterColours.colourPeakHold      = getColourFromLnf (peakHoldColourId, juce::Colours::red);
	}
} // LevelMeter