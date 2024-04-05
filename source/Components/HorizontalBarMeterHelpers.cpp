//
// Created by prmir on 2024-03-31.
//

#include "HorizontalBarMeterHelpers.h"

namespace LevelMeter {
	namespace Helpers {
		[[nodiscard]] static constexpr bool containsUpTo(juce::Range<float> levelRange, float levelDecibels) noexcept
		{
			return levelDecibels > levelRange.getStart() && levelDecibels <= levelRange.getEnd();
		}

		//juce::Rectangle<int> result {rectToPad};
	}
} // LevelMeter