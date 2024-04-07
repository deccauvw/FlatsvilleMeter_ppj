//
// Created by prmir on 2024-04-01.
//
#pragma once

#include "HorizontalBarMeterHeader.h"

namespace LevelMeter {
	HorizontalBarMeterHeader::~HorizontalBarMeterHeader() = default;
// ===================================================================================
	void HorizontalBarMeterHeader::setChannelType(const juce::AudioChannelSet::ChannelType &channelType)
	{
		meterHeader_ChannelType = channelType;
		meterHeader_DescriptionChannelType = juce::AudioChannelSet::getChannelTypeName(channelType);
	}

	void HorizontalBarMeterHeader::setMetricType(const juce::String &metricType)
	{
		if(metricType.isEmpty())
			return;
		meterHeader_DescriptionMetricType = metricType;
	}

	void HorizontalBarMeterHeader::setChannelName(const juce::String &name)
	{
		if(name.isEmpty())
			return;
		meterHeader_ChannelName = name;
		calculateInfoWidth();
	}

	void HorizontalBarMeterHeader::setFont(const juce::Font &font)
	{
		meterHeader_font = font;
		calculateInfoWidth();
	}

	void HorizontalBarMeterHeader::setBounds(const juce::Rectangle<int> &bounds) noexcept
	{
		meterHeader_Bounds = bounds;
	}

    void HorizontalBarMeterHeader::setReferredWidth (float referredWidth) noexcept
    {
        meterHeader_referredWidth = referredWidth;
    }
// ===================================================================================
	const juce::AudioChannelSet::ChannelType &HorizontalBarMeterHeader::getChannelType() const noexcept {
		return meterHeader_ChannelType;
	}

	const juce::String HorizontalBarMeterHeader::getMetricType() const noexcept {
		return meterHeader_DescriptionMetricType;
	}

	const juce::String HorizontalBarMeterHeader::getChannelName() const noexcept {
		return meterHeader_ChannelName;
	}

	float HorizontalBarMeterHeader::getChannelNameWidth() const noexcept {
		return meterHeader_ChannelNameWidth;
	}

	float HorizontalBarMeterHeader::getChannelTypeWidth() const noexcept {
		return meterHeader_ChannelTypeWidth;
	}

	float HorizontalBarMeterHeader::getMetricTypeWidth() const noexcept {
		return meterHeader_MetricTypeWidth;
	}

//	juce::String HorizontalBarMeterHeader::getChannelInfo() const noexcept {
//		//...
//	}

	bool HorizontalBarMeterHeader::textFits(const juce::String &text, int widthAvailable) const {
		return meterHeader_font.getStringWidthFloat(text) <= static_cast<float>(widthAvailable);
	}

	juce::Rectangle<int> HorizontalBarMeterHeader::getBounds() const noexcept {
		return meterHeader_Bounds;
	}
	// ===================================================================================
	//@brief this is to separate which channel is drawn upper and lower
	void HorizontalBarMeterHeader::drawHeaderWiseChannelType(juce::Graphics &g, const juce::String& channelType,
	                                                         const LevelMeter::MeterColours &meterColours){
		/*
		 * the bar meter position is already decided : They're either LR or MS
		 * L and M are positioned TOP, R and S are positioned BOTTOM
		 */
		std::vector<std::string> criteriaTop = {"left", "centre"};
		auto it = find(criteriaTop.begin(), criteriaTop.end(), channelType.toStdString());

		juce::Rectangle<int> headerCoordinates;
		headerCoordinates.setHeight(Constants::kHeaderChannelTypeHeight);
		headerCoordinates.setWidth(Constants::kHeaderChannelTypeWidth);
		headerCoordinates.setX(Constants::kHeaderChannelTypePositionX);
		(it != criteriaTop.end()) ? headerCoordinates.setY(Constants::kHeaderChannelTypePositionY_Top) : headerCoordinates.setY(Constants::kHeaderChannelTypePositionY_Btm);

		//draw
		g.setColour(meterColours.colourText);
		g.setFont(meterHeader_font.withHeight(Constants::kHeaderChannelTypeFontHeight));
		g.drawFittedText(channelType, headerCoordinates, juce::Justification::centred, 1);
	}

	void HorizontalBarMeterHeader::drawCenteredMetricType(juce::Graphics &g, const juce::String& metricType,
	                                                      const LevelMeter::MeterColours &meterColours) {
		juce::Rectangle<int> centeredHeaderCoordinates;
		centeredHeaderCoordinates.setX(Constants::kHeaderMetricTypePositionX);
		centeredHeaderCoordinates.setY(Constants::kHeaderMetricTypePositionY);
		centeredHeaderCoordinates.setWidth(Constants::kHeaderMetricTypeWidth);
		centeredHeaderCoordinates.setHeight(Constants::kHeaderMetricTypeHeight);
		g.setColour(meterColours.colourText);
		g.setFont(meterHeader_font.withHeight(Constants::kHeaderMetricTypeFontHeight));
		g.drawFittedText(metricType, centeredHeaderCoordinates, juce::Justification::centred, 1);
	}

	void HorizontalBarMeterHeader::drawChannelName(juce::Graphics &g, const juce::String& channelName,
	                                               const LevelMeter::MeterColours &meterColours) {
		juce::Rectangle<int> channelNameCoordinates;
		channelNameCoordinates.setX(Constants::kSmallDisplayPositionX);
		channelNameCoordinates.setY(Constants::kSmallDisplayPositionY);
		channelNameCoordinates.setHeight(Constants::kSmallDisplayHeight);
		channelNameCoordinates.setWidth(Constants::kSmallDisplayWidth);
		g.setColour(meterColours.colourText);
		g.setFont(meterHeader_font.withHeight(Constants::kSmallDisplayFontHeight));
		g.drawFittedText(channelName,channelNameCoordinates,juce::Justification::centred, 1);
	}

	// ===================================================================================
	void HorizontalBarMeterHeader::draw(juce::Graphics &g, bool meterActive, const MeterColours &meterColours) {
		if(meterHeader_Bounds.isEmpty())
			return;

		auto textChannelType = meterHeader_DescriptionChannelType;
		auto textMetricType =  meterHeader_DescriptionMetricType;
		//draw channel names
		auto textChannelName = getChannelName();

		//draw buttons if exists
		//...

		//if meter is active ~ virtually always active
		if(meterActive)
		{
			g.setColour(meterColours.colourText);
		}
		else
		{
			// ...
		}

		if(textChannelName.isNotEmpty())
			drawChannelName(g, textChannelName, meterColours);

		if(textChannelType.isNotEmpty())
			drawHeaderWiseChannelType(g, textChannelType, meterColours);

		if(textMetricType.isNotEmpty())
			drawCenteredMetricType(g, textMetricType, meterColours);

	}

	void HorizontalBarMeterHeader::calculateInfoWidth() {
		meterHeader_ChannelNameWidth = meterHeader_font.getStringWidthFloat(meterHeader_ChannelName);
		meterHeader_ChannelTypeWidth = meterHeader_font.getStringWidthFloat(meterHeader_DescriptionChannelType);
		meterHeader_MetricTypeWidth = meterHeader_font.getStringWidthFloat(meterHeader_DescriptionMetricType);

	}
} // LevelMeter