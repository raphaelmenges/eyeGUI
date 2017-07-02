//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleDefinitions.h"

namespace eyegui
{
	StylePropertyStringTuple StylePropertyStringMaps::value =
	{
		{
			{ "AnimationDuration",									StylePropertyFloat::AnimationDuration },
			{ "SensorPenetrationIncreaseDuration",					StylePropertyFloat::SensorPenetrationIncreaseDuration },
			{ "SensorPenetrationDecreaseDuration",					StylePropertyFloat::SensorPenetrationDecreaseDuration },
			{ "ButtonThresholdIncreaseDuration",					StylePropertyFloat::ButtonThresholdIncreaseDuration },
			{ "ButtonThresholdDecreaseDuration",					StylePropertyFloat::ButtonThresholdDecreaseDuration },
			{ "ButtonPressingDuration",								StylePropertyFloat::ButtonPressingDuration },
			{ "SensorInteractionPenetrationAmount",					StylePropertyFloat::SensorInteractionPenetrationAmount },
			{ "DimIncreaseDuration",								StylePropertyFloat::DimIncreaseDuration },
			{ "DimDecreaseDuration",								StylePropertyFloat::DimDecreaseDuration },
			{ "DimAlpha",											StylePropertyFloat::DimAlpha },
			{ "FlashDuration",										StylePropertyFloat::FlashDuration },
			{ "MaximalAdaptiveScaleIncrease",						StylePropertyFloat::MaximalAdaptiveScaleIncrease },
			{ "AdaptiveScaleIncreaseDuration",						StylePropertyFloat::AdaptiveScaleIncreaseDuration },
			{ "AdaptiveScaleDecreaseDuration",						StylePropertyFloat::AdaptiveScaleDecreaseDuration },
			{ "KeyboardZoomSpeedMultiplier",						StylePropertyFloat::KeyboardZoomSpeedMultiplier },
			{ "KeyboardKeySelectionDuration",						StylePropertyFloat::KeyboardKeySelectionDuration },
			{ "FlowSpeedMultiplier",								StylePropertyFloat::FlowSpeedMultiplier },
			{ "TextEditScrollSpeedMultiplier",						StylePropertyFloat::TextEditScrollSpeedMultiplier },

			// Experimental
			{ "FutureKeyboardPressDuration",						StylePropertyFloat::FutureKeyboardPressDuration },
			{ "FutureKeyboardRetriggerDelay",						StylePropertyFloat::FutureKeyboardRetriggerDelay },
			{ "FutureKeyboardThresholdDuration",					StylePropertyFloat::FutureKeyboardThresholdDuration },
			{ "FutureKeyboardRepeatKeyThresholdMultiplier",			StylePropertyFloat::FutureKeyboardRepeatKeyThresholdMultiplier },
			{ "FutureKeyboardSpaceKeyThresholdMultiplier",			StylePropertyFloat::FutureKeyboardSpaceKeyThresholdMultiplier },
			{ "FutureKeyboardBackspaceKeyThresholdMultiplier",		StylePropertyFloat::FutureKeyboardBackspaceKeyThresholdMultiplier },
			{ "FutureKeyboardSuggestionLineThresholdMultiplier",	StylePropertyFloat::FutureKeyboardSuggestionLineThresholdMultiplier }
		},
		{
			{ "Color",												StylePropertyVec4::Color },
			{ "BackgroundColor",									StylePropertyVec4::BackgroundColor },
			{ "HighlightColor",										StylePropertyVec4::HighlightColor },
			{ "SeparatorColor",										StylePropertyVec4::SeparatorColor },
			{ "SelectionColor",										StylePropertyVec4::SelectionColor },
			{ "IconColor",											StylePropertyVec4::IconColor },
			{ "FontColor",											StylePropertyVec4::FontColor },
			{ "DimColor",											StylePropertyVec4::DimColor },
			{ "FlashColor",											StylePropertyVec4::FlashColor },
			{ "MarkColor",											StylePropertyVec4::MarkColor },
			{ "PickColor",											StylePropertyVec4::PickColor },
			{ "ThresholdColor",										StylePropertyVec4::ThresholdColor }
		},
		{
			{ "SoundButtonDown",									StylePropertyString::SoundButtonDown },
		}
	};
}