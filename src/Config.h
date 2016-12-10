//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef CONFIG_H_
#define CONFIG_H_

#include "src/Utilities/Helper.h"
#include "src/Defines.h"
#include "externals/GLM/glm/glm.hpp"

#include <map>

enum class StyleValue_float
{
	AnimationDuration,
	SensorPenetrationIncreaseDuration,
	SensorPenetrationDecreaseDuration,
	ButtonThresholdIncreaseDuration,
	ButtonThresholdDecreaseDuration,
	ButtonPressingDuration,
	SensorInteractionPenetrationAmount,
	DimIncreaseDuration,
	DimDecreaseDuration,
	FlashDuration,
	MaximalAdaptiveScaleIncrease,
	AdaptiveScaleIncreaseDuration,
	AdaptiveScaleDecreaseDuration,
	GazeVisualizationFadeDuration,
	GazeVisualizationFocusDuration,
	GazeVisualizationRejectThreshold,
	GazeVisualizationMinSize,
	GazeVisualizationMaxSize,
	KeyboardZoomSpeedMultiplier,
	KeyboardKeyPressDuration,
	FlowSpeedMultiplier,
	TextEditScrollSpeedMultiplier
};

enum class StyleValue_vec4
{
	GazeVisualizationColor
};


// Fancy macros for saving lot of writing and preserving overview. Should be only used within this header file
#define StyleValueMember(name, type, value) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new StyleValue<type>(value, STYLE_BASE_CLASS_NAME));
#define StyleValueMemberMin(name, type, value, min) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new NumericStyleValue<type>(value, STYLE_BASE_CLASS_NAME, min));
#define StyleValueMemberMinMax(name, type, value, min, max) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new NumericStyleValue<type>(value, STYLE_BASE_CLASS_NAME, min, max));

namespace eyegui
{
	// General style value
	template<typename T>
	class StyleValue
	{
	public:

		// Constructor takes initial value and name of style class which introduced the value
		StyleValue(T initialValue, std::string styleClass) { this->mValue = initialValue; this->mStyleClass = styleClass; }

		// Setter for value
		virtual void set(T value)
		{
			// Just store the value
			this->mValue = value;

			// Remember that value has been actively set
			mValueSet = true;
		}

		// Getter for value
		T get() const { return this->mValue; }

		// Check whether value has been actively set
		bool isSet() const { return this->mValueSet; }

	protected:

		// Protected default constructor called by subclass
		StyleValue() {}

		// Value
		T mValue;

		// Indicator whether value has been actively set
		bool mValueSet = false;

		// Name of style class which introduced this value
		std::string mStyleClass;
	};

	// Numeric style value (which can be clamped)
	template<typename T>
	class NumericStyleValue : public StyleValue<T>
	{
	public:

		// Constructor takes initial value
		NumericStyleValue(T initialValue, std::string styleClass) : StyleValue<T>(initialValue, styleClass) {}
		NumericStyleValue(T initialValue, std::string styleClass, T min) { setMin(min); this->mValue = glm::max(initialValue, min); this->mStyleClass = styleClass; }
		NumericStyleValue(T initialValue, std::string styleClass, T min, T max) { setMin(min); setMax(max); this->mValue = glm::clamp(initialValue, min, max); this->mStyleClass = styleClass; }

		// Setter for value. Includes clamping
		virtual void set(T value) override
		{
			if (mMinimumSet && mMaximumSet)
			{
				this->mValue = glm::clamp(value, mMinimum, mMaximum);
			}
			else if (mMinimumSet)
			{
				this->mValue = glm::max(value, mMinimum);
			}
			else
			{
				this->mValue = value;
			}
		}

	protected:

		// Set minimum
		void setMin(T min)
		{
			mMinimum = min;
			mMinimumSet = true;
		}

		// Set maximum
		void setMax(T max)
		{
			mMaximum = max;
			mMaximumSet = true;
		}

		// Minimum
		T mMinimum;
		bool mMinimumSet = false;

		// Maximum
		T mMaximum;
		bool mMaximumSet = false;
	};

    class Config
    {
	public:

		// Constructor
		Config()
		{
			// Initialize float values
			typedef StyleValue_float sFloat; // simplify enum access
			typedef std::shared_ptr<StyleValue<float> > spFloat; // simplify shared pointer creation
			std::function<void(sFloat, spFloat)> floatInsert = [&](sFloat styleValueType, spFloat styleValue) // simplify map insertion
			{
				mFloatMap.insert(std::make_pair(styleValueType, styleValue));
			};
			floatInsert(sFloat::AnimationDuration,					spFloat(new NumericStyleValue<float>(0.1f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::SensorPenetrationIncreaseDuration,	spFloat(new NumericStyleValue<float>(3.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::SensorPenetrationDecreaseDuration,	spFloat(new NumericStyleValue<float>(1.5f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::ButtonThresholdIncreaseDuration,	spFloat(new NumericStyleValue<float>(1.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::ButtonThresholdDecreaseDuration,	spFloat(new NumericStyleValue<float>(2.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::ButtonPressingDuration,				spFloat(new NumericStyleValue<float>(0.3f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::SensorInteractionPenetrationAmount, spFloat(new NumericStyleValue<float>(0.5f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::DimIncreaseDuration,				spFloat(new NumericStyleValue<float>(1.5f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::DimDecreaseDuration,				spFloat(new NumericStyleValue<float>(0.25f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::FlashDuration,						spFloat(new NumericStyleValue<float>(2.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::MaximalAdaptiveScaleIncrease,		spFloat(new NumericStyleValue<float>(0.5f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::AdaptiveScaleIncreaseDuration,		spFloat(new NumericStyleValue<float>(1.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::AdaptiveScaleDecreaseDuration,		spFloat(new NumericStyleValue<float>(1.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::GazeVisualizationFadeDuration,		spFloat(new NumericStyleValue<float>(4.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::GazeVisualizationFocusDuration,		spFloat(new NumericStyleValue<float>(2.0f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::GazeVisualizationRejectThreshold,	spFloat(new NumericStyleValue<float>(0.125f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::GazeVisualizationMinSize,			spFloat(new NumericStyleValue<float>(0.02f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::GazeVisualizationMaxSize,			spFloat(new NumericStyleValue<float>(0.075f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::KeyboardZoomSpeedMultiplier,		spFloat(new NumericStyleValue<float>(1.0f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::KeyboardKeyPressDuration,			spFloat(new NumericStyleValue<float>(1.25f, STYLE_BASE_CLASS_NAME, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::FlowSpeedMultiplier,				spFloat(new NumericStyleValue<float>(1.0f, STYLE_BASE_CLASS_NAME, 0)));
			floatInsert(sFloat::TextEditScrollSpeedMultiplier,		spFloat(new NumericStyleValue<float>(1.0f, STYLE_BASE_CLASS_NAME, 0)));

			// Initialize vec4 values
			typedef StyleValue_vec4 v4Float; // simplify enum access
			typedef std::shared_ptr<StyleValue<glm::vec4> > spVec4; // simplify shared pointer creation
			std::function<void(v4Float, spVec4)> vec4Insert = [&](v4Float styleValueType, spVec4 styleValue) // simplify map insertion
			{
				mVec4Map.insert(std::make_pair(styleValueType, styleValue));
			};
			vec4Insert(v4Float::GazeVisualizationColor,				spVec4(new NumericStyleValue<glm::vec4>(glm::vec4(0, 0, 1.f, 0.5f), STYLE_BASE_CLASS_NAME, VEC_4_ZERO, VEC_4_ONE)));
		}

		// Get a value
		std::shared_ptr<const StyleValue<float> > getValue(StyleValue_float type) const { return mFloatMap.at(type); }
		std::shared_ptr<const StyleValue<glm::vec4> > getValue(StyleValue_vec4 type) const { return mVec4Map.at(type); }

		// Set a value
		void setValue(StyleValue_float type, std::string value) { mFloatMap.at(type)->set(stringToFloat(value)); };
		void setValue(StyleValue_vec4 type, std::string value) { mVec4Map.at(type)->set(stringHexRGBAToVec4RGBA(value)); };

        // Initialize with fallback values
        std::string filepath;

		std::map < StyleValue_float, std::shared_ptr<StyleValue<float> > > mFloatMap;
		std::map < StyleValue_vec4, std::shared_ptr<StyleValue<glm::vec4> > > mVec4Map;

        // Experimental (TODO: change to new system too)
        float futureKeyboardPressDuration = 0.5f;
        float futureKeyboardRetriggerDelay = 0.5f;
        float futureKeyboardThresholdDuration = 1.f;
		float futureKeyboardRepeatKeyThresholdMultiplier = 1.f;
		float futureKeyboardSpaceKeyThresholdMultiplier = 1.f;
		float futureKeyboardBackspaceKeyThresholdMultiplier = 1.f;
		float futureKeyboardSuggestionLineThresholdMultiplier = 1.f;

	protected:

		std::string mName = STYLE_BASE_CLASS_NAME; // todo make common superclass for both base style and style (and get rid of name "config")
    };
}

#endif // CONFIG_H_
