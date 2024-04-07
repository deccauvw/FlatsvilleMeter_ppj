
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "HorizontalBarMeterHeader.h"
#include "HorizontalBarMeterHelpers.h"
#include "HorizontalBarMeterLevel.h"


namespace LevelMeter {
	/*
	 * @brief an individual meter channel.
	 *
	 * this represents a single meter.
	 * use the metersComponent to create multiple meters matching the specified channel format
	 */
	class HorizontalBarMeterChannel:
		public juce::Component,
		private juce::SettableTooltipClient
	{
	public:
		using ChannelType   = juce::AudioChannelSet::ChannelType;
		using Ptr           = juce::Component::SafePointer<HorizontalBarMeterChannel>;
//skeleton
		//@brief Default constructor
		HorizontalBarMeterChannel() noexcept;

		//@brief parameterized constr.
		/*
		 * MeterOptions := meter options to use, defining aesthetics and functionality.
		 * padding := N/A
		 * channel name := channel name set by the user
		 * isLabelStrip := N/A
		 * channelType := the channelType(L R M S etc)
		 */
		HorizontalBarMeterChannel(const Options& meterOptions, const juce::String& channelName, ChannelType channelType = ChannelType::unknown);

		//@brief default destructor.
		~HorizontalBarMeterChannel() override;
		//==========================================
//utilities
		//@brief reset the meter (but not the peak hold)
		//see@ResetPeakHold
		void reset();

		//@brief reset the peak hold
		//resets the peak hold indicator and value
		void resetPeakHold();

		//@brief show(and hide) hte peak hold indicator
		void showPeakHold(bool showPeakHold = true);

		//@brief show the peak 'value' part of the meter.
		//the peak value part will be shown above and below each meter (in dB)
		//same level as the peak hold bar
		//showValue := if true : 'value' will be shown
		void showValue(bool showValue = true);

		//@brief show the header part of the meter
		//headerVisible := when true, header will be visible
		void showHeader(bool headerVisible);

		//@brief enable tick marks on the meter (divider lines)
		void showTickMarks(bool showTickMarks);

		//@brief show the tick marks on top of the level or below it.
		//when below, meter will "cover" the tickMarks.
		void showTickMarksOnTop(bool showTickMarksOnTop);

		//@brief Refresh the meter with the current input level
		//@forceRefresh := when set to true will be forced to repaint (even if not Dirty)
		void refresh(bool forceRefresh);

		//@brief use gradients instead of hard segment boundaries.
		//if true : smooth gradients. false gives hard seg. boundaries.
		void useGradients(bool useGradients);

		//==========================================
//setter
		//@brief Sets the meter refresh rate
		//set this to optimize the meter's decay rate
		//@refreshRateHz:= refresh rate in Hz
		void setRefreshRate(float refreshRateHz);

		//@brief set meter decay
		//decayMs := meterDecay in ms
		void  setDecay(float decayMs);

		//@brief set the level of the tick marks.
		//tickMarks := list of the tick mark values in dB
		void setTickMarks(const std::vector<float>& tickMarks);

		//@brief set the input level from the audio engine
		//called from the audio thread
		//inputLevel := new input level in amp
		inline void setInputLevel(float inputLevel);

		//@brief set the meter's options.
		//options determine the appearance and func. of the meter
		//meterOptions := meter options to use
		void setOptions(const Options& meterOptions);

		//@brief set the segments the meter is made out of.
		//a range within the meter and a colour or gradient
		//@param segmentsOptions := the segments options to create the segments with.
		void setMeterSegments(const std::vector<SegmentOptions>& segmentsOptions);

		//@brief set the meter font.
		//font used in everything to draw something : header, value, label strip etc.
		//font := font to use
		void setFont(const juce::Font& font);

		//@brief Set the channel name
		//sets the channel name belonging to the track feeding the meter.
		void setChannelName(const juce::String& channelName);

		//@brief set the channelType
		//sets the channelType(L R M S)
		void setChannelType(ChannelType channelType);

		//@brief set the padding of the meter
		//void setPadding not used

		//@brief set whether this meter is a label strip
		//void setLabelStrip(bool isLabelStrip = false); N/A

		//@brief set meter active
		//void setActive(bool isActive) N/A

        /**
     * @brief Set the referred width (from other meters) used to decide what info to display.
     *
     * When this is set to zero, each meter uses his own bounds to decide what to display.
     * When set to a non zero value (for instance from another meter) this meter will use that
     * value to decide what to display.
     * When there is not enough room (width) to display the full description or name, display
     * the abbreviated type description.
     *
     * @param referredTypeWidth The width (in pixels) to use when deciding what to display in the header.
    */
		void setReferredTypeWidth(float referredTypeWidth) noexcept;

		//void setMinimalMode()

		//bool isMinimalModeActive()
		//==========================================
//getter
		[[nodiscard]] float getDecay() const noexcept;

		//get the width(pixels) of the full "channel name" of the header part.
		//meter will have small horizontal display on the right : checks if the text bound exceeds there.
		[[nodiscard]] float getChannelNameWidth() const noexcept;

		//get the width(pixels) of the channel type(L R M S) of the header part.
		[[nodiscard]] float getChannelTypeWidth() const noexcept;

		//get the bounds of the meter and header parts combined.
		//return the bounds of the respective parts
		[[nodiscard]] juce::Rectangle<int> getLabelStripBounds() const noexcept;

		//==========================================
//Anything that prevents meter aesthetics going "Dirty"
		//used with "getChannel ~ width fn.
		[[nodiscard]] bool nameFits(const juce::String& name, int widthAvailable) const;

		//==========================================
//internal graphics stuff
		void paint(juce::Graphics& g) override;
		void resized() override;
		void lookAndFeelChanged() override;
		void visibilityChanged() override;

		//==========================================
//ColourIDs
		//@brief ColourIDs that can be used to custom the colours.
		//this can be done by overriding the look and feel class
		enum ColourIds
		{
			backgroundColourId     = 0x1a03201,  ///< Background colour.
			tickMarkColourId       = 0x1a03202,  ///< Tick-mark colour.
			textColourId           = 0x1a03203,  ///< Text colour.
		//	faderColourId          = 0x1a03204,  ///< Fader colour.
			textValueColourId      = 0x1a03205,  ///< Value text colour.
		//	mutedColourId          = 0x1a03206,  ///< Muted button colour.
		//	soloedColourId          = 0x1a03207,  ///< Soloed button colour.
		//	mutedMouseOverColourId = 0x1a03208,  ///< Muted mouse over button colour.
		//	inactiveColourId       = 0x1a03209,  ///< Inactive (muted) colour.
			peakHoldColourId       = 0x1a03210   ///< Peak hold colour.
		};
	private:
		HorizontalBarMeterHeader meterChannel_header{meterChannel_font};
		HorizontalBarMeterLevel  meterChannel_Level{};
		Options                  meterChannel_options{};
//isActive
		bool meterChannel_active = true;
//aesthetics
		juce::Font           meterChannel_font;
		MeterColours         meterChannel_meterColours{};
//dirty stuff
		juce::Rectangle<int> meterChannel_dirtyRect{};

		void setDirty(bool isDirty = true) noexcept;

		[[nodiscard]]bool isDirty(const juce::Rectangle<int> &rectToCheck = {}) const noexcept;

		void addDirty(const juce::Rectangle<int> &dirtyRect) noexcept;
//graphics
		void drawMeter(juce::Graphics &g);

		[[nodiscard]] juce::Colour getColourFromLnf(int colourId, const juce::Colour &fallbackColour) const;

		void setColours();
	};
} // LevelMeter
