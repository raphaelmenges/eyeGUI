//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO: Constraints

#ifndef STYLE_VALUE_H_
#define STYLE_VALUE_H_

#include "externals/GLM/glm/glm.hpp"

#include <string>
#include <memory>

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

		// Constructor takes initial value and pointer to style class that holds this value
		StyleValue(std::weak_ptr<const StyleClass> wpStyleClass, T value) { this->mwpStyleClass = wpStyleClass; this->mValue = value; }

		// Getter for style class that holds this
		std::weak_ptr<const StyleClass> getStyleClass() const {	return mwpStyleClass; };

		// Setter for value
		virtual void set(T value) {	this->mValue = value; }

		// Getter for value
		T get() const { return this->mValue; }

	protected:

		// Pointer to style class that holds this value
		std::weak_ptr<const StyleClass> mwpStyleClass;

		// Value
		T mValue;
	};
}

#endif // STYLE_VALUE_H_