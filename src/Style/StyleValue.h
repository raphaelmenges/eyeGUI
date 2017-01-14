//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Singe style value. Applies given constraint on itself when value is set.
// Has pointer to owner of the value.

#ifndef STYLE_VALUE_H_
#define STYLE_VALUE_H_

#include "externals/GLM/glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>

namespace eyegui
{
	// Forward declaration
	class StyleClass;

	enum class StyleType_float
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

	enum class StyleType_vec4
	{
		GazeVisualizationColor
	};

	template<typename T>
	class StyleValue
	{
	public:

		// Constructor pointer to class thats owns the value and an initial value. Optionally can be added a constraint that is checked at set
		StyleValue(std::weak_ptr<const StyleClass> wpStyleClass, T value, std::function<T(T)> constraint = [](T value) {return value; })
		{
			this->mwpStyleClass = wpStyleClass;
			this->mConstraint = std::move(constraint);
			this->mValue = this->mConstraint(value);
		}

		// Get style class that owns this
		std::weak_ptr<const StyleClass> getStyleClass() const {	return mwpStyleClass; };

		// Set value
		virtual void set(T value) { this->mValue = this->mConstraint(value); }

		// Get value
		T get() const { return this->mValue; }

		// Get constraint
		std::function<T(T)> getConstraint() const { return this->mConstraint; }

	protected:

		// Pointer to style class that holds this value
		std::weak_ptr<const StyleClass> mwpStyleClass;

		// Value
		T mValue;

		// Constraint function
		std::function<T(T)> mConstraint;
	};
}

#endif // STYLE_VALUE_H_