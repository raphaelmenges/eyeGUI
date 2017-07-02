//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Maps string to style property type.

#ifndef STYLE_PROPERTY_NAME_MAPPER
#define STYLE_PROPERTY_NAME_MAPPER

#include "include/eyeGUI.h"

#include <map>
#include <tuple>

namespace eyegui
{
	// Compile time mapping between style property and it value type
	template<typename Type> struct StylePropertyValue;
	template<> struct StylePropertyValue<StylePropertyFloat> { typedef float type; };
	template<> struct StylePropertyValue<StylePropertyVec4> { typedef glm::vec4 type; };
	template<> struct StylePropertyValue<StylePropertyString> { typedef std::string type; };

	// Typedef of tuple with maps for the StyleClass
	template<typename Type> using StyleMap = std::map<Type, std::shared_ptr<StyleProperty<typename StylePropertyValue<Type>::type> > >;
	typedef
		std::tuple<
			StyleMap<StylePropertyFloat>,
			StyleMap<StylePropertyVec4>,
			StyleMap<StylePropertyString>
		> StyleMaps;

	// Indices of property types in tuple above
	template<typename Type> struct StylePropertyTupleIndex { static const int index = -1; }; // fallback, should produce error
	template<> struct StylePropertyTupleIndex<StylePropertyFloat> { static const int index = 0; };
	template<> struct StylePropertyTupleIndex<StylePropertyVec4> { static const int index = 1; };
	template<> struct StylePropertyTupleIndex<StylePropertyString> { static const int index = 2; };

	// TODO
	// - have uniform map of strings to property type (or at least uniform way to access)
	// - have some map providing default values of properties
	// - template specialization providing constraint per property type

	// Maps from string to style property type
	namespace StylePropertyNameMapper
	{
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
		const std::map<std::string, StylePropertyString> STRING_TYPE_MAP =
		{
			{ "SoundButtonDown",									StylePropertyString::SoundButtonDown },
		};
	}
}

#endif // STYLE_PROPERTY_NAME_MAPPER