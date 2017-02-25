//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Maps string to style property type.

#ifndef STYLE_PROPERTY_NAME_MAPPER
#define STYLE_PROPERTY_NAME_MAPPER

#include "include/eyeGUI.h"

namespace eyegui
{
	// Maps from string to style property type
	namespace StylePropertyNameMapper
	{
		// TODO NOTE: ADD MAPPING OF PROPERTIES HERE! Also remember to add initial value and constraints in StyleClass.cpp

		const std::map<std::string, StylePropertyFloat> FLOAT_TYPE_MAP =
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
		};
		const std::map<std::string, StylePropertyVec4> VEC4_TYPE_MAP =
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
		};
	}
}

#endif // STYLE_PROPERTY_NAME_MAPPER