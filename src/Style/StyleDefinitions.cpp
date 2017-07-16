//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleDefinitions.h"

namespace eyegui
{
	namespace style
	{
		// One *must* enter new property instances here!
		PropertyStringTuple PropertyStringMapping::value
		{
			// Duration
			{
				{ "AnimationDuration",					property::Duration::AnimationDuration },
				{ "SensorPenetrationIncreaseDuration",	property::Duration::SensorPenetrationIncreaseDuration },
				{ "SensorPenetrationDecreaseDuration",	property::Duration::SensorPenetrationDecreaseDuration },
				{ "ButtonThresholdIncreaseDuration",	property::Duration::ButtonThresholdIncreaseDuration },
				{ "ButtonThresholdDecreaseDuration",	property::Duration::ButtonThresholdDecreaseDuration },
				{ "ButtonPressingDuration",				property::Duration::ButtonPressingDuration },
				{ "DimIncreaseDuration",				property::Duration::DimIncreaseDuration },
				{ "DimDecreaseDuration",				property::Duration::DimDecreaseDuration },
				{ "AdaptiveScaleIncreaseDuration",		property::Duration::AdaptiveScaleIncreaseDuration },
				{ "AdaptiveScaleDecreaseDuration",		property::Duration::AdaptiveScaleDecreaseDuration },
				{ "FlashDuration",						property::Duration::FlashDuration },
				{ "KeyboardKeySelectionDuration",		property::Duration::KeyboardKeySelectionDuration },

				// Experimental
				{ "FutureKeyboardPressDuration",		property::Duration::FutureKeyboardPressDuration },
				{ "FutureKeyboardThresholdDuration",	property::Duration::FutureKeyboardThresholdDuration }
			},

			// Percentage
			{
				{ "DimAlpha",	property::Percentage::DimAlpha }
			},

			// Amount
			{
				{ "SensorInteractionPenetrationAmount",	property::Amount::SensorInteractionPenetrationAmount },
				{ "KeyboardZoomSpeedMultiplier",		property::Amount::KeyboardZoomSpeedMultiplier },
				{ "FlowSpeedMultiplier",				property::Amount::FlowSpeedMultiplier },
				{ "TextEditScrollSpeedMultiplier",		property::Amount::TextEditScrollSpeedMultiplier },
				{ "MaximalAdaptiveScaleIncrease",		property::Amount::MaximalAdaptiveScaleIncrease },

				// Experimental
				{ "FutureKeyboardRepeatKeyThresholdMultiplier",			property::Amount::FutureKeyboardRepeatKeyThresholdMultiplier },
				{ "FutureKeyboardSpaceKeyThresholdMultiplier",			property::Amount::FutureKeyboardSpaceKeyThresholdMultiplier },
				{ "FutureKeyboardBackspaceKeyThresholdMultiplier",		property::Amount::FutureKeyboardBackspaceKeyThresholdMultiplier },
				{ "FutureKeyboardSuggestionLineThresholdMultiplier",	property::Amount::FutureKeyboardSuggestionLineThresholdMultiplier },
				{ "FutureKeyboardRetriggerDelay",						property::Amount::FutureKeyboardRetriggerDelay }
			},

			// Color
			{
				{ "Color",				property::Color::Color },
				{ "BackgroundColor",	property::Color::BackgroundColor },
				{ "HighlightColor",		property::Color::HighlightColor },
				{ "SeparatorColor",		property::Color::SeparatorColor },
				{ "SelectionColor",		property::Color::SelectionColor },
				{ "IconColor",			property::Color::IconColor },
				{ "FontColor",			property::Color::FontColor },
				{ "DimColor",			property::Color::DimColor },
				{ "FlashColor",			property::Color::FlashColor },
				{ "MarkColor",			property::Color::MarkColor },
				{ "PickColor",			property::Color::PickColor },
				{ "ThresholdColor",		property::Color::ThresholdColor }
			},

			// Asset Path
			{
				{ "ButtonHitSound",		property::AssetPath::ButtonHitSound },
				{ "ButtonDownSound",	property::AssetPath::ButtonDownSound },
				{ "KeyPressSound",		property::AssetPath::KeyPressSound }
			}
		};

		// Maps of default values
		struct PropertyDefaultValues
		{
			static std::map<property::Duration, PropertyInfo<property::Duration>::type>		durationDefaults;
			static std::map<property::Percentage, PropertyInfo<property::Percentage>::type>	percentageDefaults;
			static std::map<property::Amount, PropertyInfo<property::Amount>::type>			amountDefaults;
			static std::map<property::Color, PropertyInfo<property::Color>::type>			colorDefaults;
			static std::map<property::AssetPath, PropertyInfo<property::AssetPath>::type>	assetPathDefaults;

		};

		// One *can* enter default values for new property instances here!
		std::map<property::Duration, PropertyInfo<property::Duration>::type> PropertyDefaultValues::durationDefaults =
		{
			{ property::Duration::AnimationDuration,								0.1f },
			{ property::Duration::SensorPenetrationIncreaseDuration,				3.0f },
			{ property::Duration::SensorPenetrationDecreaseDuration,				1.5f },
			{ property::Duration::ButtonThresholdIncreaseDuration,					1.0f },
			{ property::Duration::ButtonThresholdDecreaseDuration,					2.0f },
			{ property::Duration::ButtonPressingDuration,							0.3f },
			{ property::Duration::DimIncreaseDuration,								1.5f },
			{ property::Duration::DimDecreaseDuration,								0.25f },
			{ property::Duration::FlashDuration,									2.0f },
			{ property::Duration::AdaptiveScaleIncreaseDuration,					1.0f },
			{ property::Duration::AdaptiveScaleDecreaseDuration,					1.0f },
			{ property::Duration::KeyboardKeySelectionDuration,						1.25f },

			// Experimental
			{ property::Duration::FutureKeyboardPressDuration,						0.5f },
			{ property::Duration::FutureKeyboardThresholdDuration,					1.0f },
		};

		std::map<property::Percentage, PropertyInfo<property::Percentage>::type> PropertyDefaultValues::percentageDefaults =
		{
			{ property::Percentage::DimAlpha, 0.5f },
		};

		std::map<property::Amount, PropertyInfo<property::Amount>::type> PropertyDefaultValues::amountDefaults =
		{
			{ property::Amount::SensorInteractionPenetrationAmount,					0.5f },
			{ property::Amount::MaximalAdaptiveScaleIncrease,						0.5f },
			{ property::Amount::KeyboardZoomSpeedMultiplier,						1.0f },
			{ property::Amount::FlowSpeedMultiplier,								1.0f },
			{ property::Amount::TextEditScrollSpeedMultiplier,						1.0f },

			// Experimental
			{ property::Amount::FutureKeyboardRetriggerDelay,						0.5f },
			{ property::Amount::FutureKeyboardRepeatKeyThresholdMultiplier,			1.0f },
			{ property::Amount::FutureKeyboardSpaceKeyThresholdMultiplier,			1.0f },
			{ property::Amount::FutureKeyboardBackspaceKeyThresholdMultiplier,		1.0f },
			{ property::Amount::FutureKeyboardSuggestionLineThresholdMultiplier,	1.0f }
		};


		std::map<property::Color, PropertyInfo<property::Color>::type> PropertyDefaultValues::colorDefaults =
		{
			{ property::Color::Color,												glm::vec4(0.6f, 0.6f, 0.6f, 1.0f) },
			{ property::Color::BackgroundColor,										glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ property::Color::HighlightColor,										glm::vec4(1.0f, 1.0f, 0.0f, 0.5f) },
			{ property::Color::SeparatorColor,										glm::vec4(0.2f, 0.2f, 0.2f, 1.0f) },
			{ property::Color::SelectionColor,										glm::vec4(0.0f, 1.0f, 1.0f, 0.5f) },
			{ property::Color::IconColor,											glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ property::Color::FontColor,											glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ property::Color::DimColor,											glm::vec4(0.1f, 0.1f, 0.1f, 0.75f) },
			{ property::Color::FlashColor,											glm::vec4(1.0f, 0.5f, 0.0f, 0.75f) },
			{ property::Color::MarkColor,											glm::vec4(0.0f, 0.5f, 1.0f, 0.5f) },
			{ property::Color::PickColor,											glm::vec4(0.2f, 1.0f, 0.0f, 0.5f) },
			{ property::Color::ThresholdColor,										glm::vec4(0.0f, 1.0f, 1.0f, 0.5f) }
		};

		std::map<property::AssetPath, PropertyInfo<property::AssetPath>::type> PropertyDefaultValues::assetPathDefaults =
		{
			{ property::AssetPath::ButtonHitSound,									std::string() },
			{ property::AssetPath::ButtonDownSound,									std::string() },
			{ property::AssetPath::KeyPressSound,									std::string() }
		};

		// Getter for default value for each property
		typename PropertyInfo<property::Duration>::type	getPropertyDefault(property::Duration t)
		{
			const auto& rMap = PropertyDefaultValues::durationDefaults;
			const auto i = rMap.find(t); return i == rMap.end() ? 0.f : i->second; // first case is fallback
		}

		typename PropertyInfo<property::Percentage>::type getPropertyDefault(property::Percentage t)
		{
			const auto& rMap = PropertyDefaultValues::percentageDefaults;
			const auto i = rMap.find(t); return i == rMap.end() ? 0.f : i->second; // first case is fallback
		}

		typename PropertyInfo<property::Amount>::type getPropertyDefault(property::Amount t)
		{
			const auto& rMap = PropertyDefaultValues::amountDefaults;
			const auto i = rMap.find(t); return i == rMap.end() ? 0.f : i->second; // first case is fallback
		}

		typename PropertyInfo<property::Color>::type getPropertyDefault(property::Color t)
		{
			const auto& rMap = PropertyDefaultValues::colorDefaults;
			const auto i = rMap.find(t); return i == rMap.end() ? glm::vec4(0.f) : i->second; // first case is fallback
		}

		typename PropertyInfo<property::AssetPath>::type getPropertyDefault(property::AssetPath t)
		{
			const auto& rMap = PropertyDefaultValues::assetPathDefaults;
			const auto i = rMap.find(t); return i == rMap.end() ? std::string() : i->second; // first case is fallback
		}
	}
}
