//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef CONFIG_H_
#define CONFIG_H_

#define StyleValueMember(name, type, value) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new StyleValue<type>(value));
#define StyleValueMemberMin(name, type, value, min) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new NumericStyleValue<type>(value, min));
#define StyleValueMemberMinMax(name, type, value, min, max) std::shared_ptr<StyleValue<type> > name = std::shared_ptr<StyleValue<type> >(new NumericStyleValue<type>(value, min, max));

#include "externals/GLM/glm/glm.hpp"

namespace eyegui
{
	// General style value
	template<typename T>
	class StyleValue
	{
	public:

		// Constructor takes initial value
		StyleValue(T value) { mValue = value; }

		// Setter for value
		virtual void setValue(T value)
		{
			// Just store the value
			mValue = value;
		}

		// Getter for value
		T getValue() const { return mValue; }

	protected:

		// Protected default constructor called by subclass
		StyleValue() {}

		// Value
		T mValue;
	};

	// Numeric style value (which can be clamped)
	template<typename T>
	class NumericStyleValue : public StyleValue<T>
	{
	public:

		// Constructor takes initial value
		NumericStyleValue(T value) : StyleValue<T>(value) {}
		NumericStyleValue(T value, T min) { setMin(min); mValue = glm::max(value, min); }
		NumericStyleValue(T value, T min, T max) { setMin(min); setMax(max); mValue = glm::clamp(value, min, max); }

		// Setter for value. Includes clamping
		virtual void setValue(T value) override
		{
			if (mMinimumSet && mMaximumSet)
			{
				mValue = glm::clamp(value, mMinimum, mMaximum);
			}
			else if (mMinimumSet)
			{
				mValue = glm::max(value, mMinimum);
			}
			else
			{
				mValue = value;
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

    struct Config
    {
        // Initialize with fallback values
        std::string filepath;
        float animationDuration = 0.1f;
        float sensorPenetrationIncreaseDuration = 3.0f;
        float sensorPenetrationDecreaseDuration = 1.5f;
        float buttonThresholdIncreaseDuration = 1.0f;
        float buttonThresholdDecreaseDuration = 2.0f;
		float buttonPressingDuration = 0.3f;
        float sensorInteractionPenetrationAmount = 0.5f;
        float dimIncreaseDuration = 1.5f;
        float dimDecreaseDuration = 0.25f;
		float flashDuration = 2.0f;
        float maximalAdaptiveScaleIncrease = 0.5f;
        float adaptiveScaleIncreaseDuration = 1.0f;
        float adaptiveScaleDecreaseDuration = 1.0f;
        glm::vec4 gazeVisualizationColor = glm::vec4(0,0,1,0.5f);
        float gazeVisualizationFadeDuration = 4.0f;
        float gazeVisualizationFocusDuration = 2.0f;
        float gazeVisualizationRejectThreshold = 0.125f;
        float gazeVisualizationMinSize = 0.02f;
        float gazeVisualizationMaxSize = 0.075f;
        float keyboardZoomSpeedMultiplier = 1.0f;
		float keyboardKeyPressDuration = 1.25f;
        float flowSpeedMultiplier = 1.0f;
		float textEditScrollSpeedMultiplier = 5.0f;

        // Experimental
        float futureKeyboardPressDuration = 0.5f;
        float futureKeyboardRetriggerDelay = 0.5f;
        float futureKeyboardThresholdDuration = 1.f;
		float futureKeyboardRepeatKeyThresholdMultiplier = 1.f;
		float futureKeyboardSpaceKeyThresholdMultiplier = 1.f;
		float futureKeyboardBackspaceKeyThresholdMultiplier = 1.f;
		float futureKeyboardSuggestionLineThresholdMultiplier = 1.f;

		StyleValueMember(mspTest, float, 0.1f);
		StyleValueMemberMin(mspTest2, float, 0.1f, 0.2f);
		StyleValueMemberMinMax(mspTest3, float, 0.1f, 0.2f, 0.3f);
    };
}

#endif // CONFIG_H_
