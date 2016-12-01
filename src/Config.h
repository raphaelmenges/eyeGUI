//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef CONFIG_H_
#define CONFIG_H_

// Fancy macros for saving lot of writing and preserving overview. Should be only used within this header file
#define StyleValueMember(name, type, value) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new StyleValue<type>(value, STYLE_BASE_CLASS_NAME));
#define StyleValueMemberMin(name, type, value, min) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new NumericStyleValue<type>(value, STYLE_BASE_CLASS_NAME, min));
#define StyleValueMemberMinMax(name, type, value, min, max) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new NumericStyleValue<type>(value, STYLE_BASE_CLASS_NAME, min, max));

#include "src/Defines.h"
#include "externals/GLM/glm/glm.hpp"
#include <map>

namespace eyegui
{
	// General style value
	template<typename T>
	class StyleValue
	{
	public:

		// Constructor takes initial value and name of style class which introduced the value
		StyleValue(T value, std::string styleClass) { this->mValue = value; this->mStyleClass = styleClass; }

		// Setter for value
		virtual void setValue(T value)
		{
			// Just store the value
			this->mValue = value;
		}

		// Getter for value
		T getValue() const { return this->mValue; }

	protected:

		// Protected default constructor called by subclass
		StyleValue() {}

		// Value
		T mValue;

		// Name of style class which introduced this value
		std::string mStyleClass;
	};

	// Numeric style value (which can be clamped)
	template<typename T>
	class NumericStyleValue : public StyleValue<T>
	{
	public:

		// Constructor takes initial value
		NumericStyleValue(T value, std::string styleClass) : StyleValue<T>(value, styleClass) {}
		NumericStyleValue(T value, std::string styleClass, T min) { setMin(min); this->mValue = glm::max(value, min); this->mStyleClass = styleClass; }
		NumericStyleValue(T value, std::string styleClass, T min, T max) { setMin(min); setMax(max); this->mValue = glm::clamp(value, min, max); this->mStyleClass = styleClass; }

		// Setter for value. Includes clamping
		virtual void setValue(T value) override
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

        // Initialize with fallback values
        std::string filepath;
		StyleValueMemberMin		(animationDuration,					float,		0.1f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(sensorPenetrationIncreaseDuration, float,		3.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(sensorPenetrationDecreaseDuration, float,		1.5f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(buttonThresholdIncreaseDuration,	float,		1.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(buttonThresholdDecreaseDuration,	float,		2.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(buttonPressingDuration,			float,		0.3f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(sensorInteractionPenetrationAmount, float,		0.5f, 0);
		StyleValueMemberMin		(dimIncreaseDuration,				float,		1.5f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(dimDecreaseDuration,				float,		0.25f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(flashDuration,						float,		2.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(maximalAdaptiveScaleIncrease,		float,		0.5f, 0);
		StyleValueMemberMin		(adaptiveScaleIncreaseDuration,		float,		1.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(adaptiveScaleDecreaseDuration,		float,		1.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMinMax	(gazeVisualizationColor,			glm::vec4,	glm::vec4(0, 0, 1.f, 0.5f), VEC_4_ZERO, VEC_4_ONE);
		StyleValueMemberMin		(gazeVisualizationFadeDuration,		float,		4.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(gazeVisualizationFocusDuration,	float,		2.0f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(gazeVisualizationRejectThreshold,	float,		0.125f, 0);
		StyleValueMemberMin		(gazeVisualizationMinSize,			float,		0.02f, 0);
		StyleValueMemberMin		(gazeVisualizationMaxSize,			float,		0.075f, 0);
		StyleValueMemberMin		(keyboardZoomSpeedMultiplier,		float,		1.0f, 0);
		StyleValueMemberMin		(keyboardKeyPressDuration,			float,		1.25f, MINIMAL_DURATION_VALUE);
		StyleValueMemberMin		(flowSpeedMultiplier,				float,		1.0f, 0);
		StyleValueMemberMin		(textEditScrollSpeedMultiplier,		float,		1.0f, 0);

        // Experimental
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
