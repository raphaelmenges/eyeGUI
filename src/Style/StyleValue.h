//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef STYLE_VALUE_H_
#define STYLE_VALUE_H_

#include "externals/GLM/glm/glm.hpp"

#include <memory>
#include <string>

namespace eyegui
{
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
		KeyboardKeySelectionDuration,
		FlowSpeedMultiplier,
		TextEditScrollSpeedMultiplier
	};

	enum class StyleValue_vec4
	{
		GazeVisualizationColor
	};

	// General style value
	template<typename T>
	class StyleValue
	{
	public:

		// Constructor takes initial value and name of style class which introduced the value
		StyleValue(std::string styleClass, T initialValue) { this->mValue = initialValue; this->mStyleClass = styleClass; }

		// Setter for value
		virtual void set(T value)
		{
			// Store the value
			this->mValue = value;

			// Remember that value has been actively set
			this->mValueSet = true;
		}

		// Propagate the value
		void propagate(T value)
		{
			// Just store the value (relying on already clamped value through initial "set" call in some other instance)
			this->mValue = value;

			// Remember propagation
			this->mValuePropagated = true;
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

		// Indicator whether vlaue has been set via propagation
		bool mValuePropagated = false;

		// Name of style class which introduced this value
		std::string mStyleClass;
	};

	// Numeric style value (which can be clamped)
	template<typename T>
	class NumericStyleValue : public StyleValue<T>
	{
	public:

		// Constructor takes initial value
		NumericStyleValue(std::string styleClass, T initialValue) : StyleValue<T>(styleClass, initialValue) {}
		NumericStyleValue(std::string styleClass, T initialValue, T min) { setMin(min); this->mValue = glm::max(initialValue, min); this->mStyleClass = styleClass; }
		NumericStyleValue(std::string styleClass, T initialValue, T min, T max) { setMin(min); setMax(max); this->mValue = glm::clamp(initialValue, min, max); this->mStyleClass = styleClass; }

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

			// Remember that value has been actively set
			mValueSet = true;
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
}

#endif // STYLE_VALUE_H_