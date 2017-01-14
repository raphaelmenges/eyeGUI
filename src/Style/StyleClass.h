//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef STYLE_CLASS_H_
#define STYLE_CLASS_H_

#include "src/Style/StyleValue.h"
#include "src/Utilities/Helper.h"
#include "src/Defines.h"

#include <functional>
#include <map>

namespace eyegui
{
	class StyleClass
	{
	public:

		// Constructor, takes name of style class
		StyleClass(std::string name)
		{
			// Fill members
			mName = name;

			// Initialize float values
			typedef StyleValue_float sFloat; // simplify enum access
			typedef std::shared_ptr<StyleValue<float> > spFloat; // simplify shared pointer creation
			std::function<void(sFloat, spFloat)> floatInsert = [&](sFloat styleValueType, spFloat styleValue) // simplify map insertion
			{
				mFloatMap.insert(std::make_pair(styleValueType, styleValue));
			};
			floatInsert(sFloat::AnimationDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.1f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::SensorPenetrationIncreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 3.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::SensorPenetrationDecreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.5f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::ButtonThresholdIncreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::ButtonThresholdDecreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 2.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::ButtonPressingDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.3f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::SensorInteractionPenetrationAmount, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.5f, 0)));
			floatInsert(sFloat::DimIncreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.5f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::DimDecreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.25f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::FlashDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 2.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::MaximalAdaptiveScaleIncrease, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.5f, 0)));
			floatInsert(sFloat::AdaptiveScaleIncreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::AdaptiveScaleDecreaseDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::GazeVisualizationFadeDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 4.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::GazeVisualizationFocusDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 2.0f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::GazeVisualizationRejectThreshold, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.125f, 0)));
			floatInsert(sFloat::GazeVisualizationMinSize, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.02f, 0)));
			floatInsert(sFloat::GazeVisualizationMaxSize, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 0.075f, 0)));
			floatInsert(sFloat::KeyboardZoomSpeedMultiplier, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.0f, 0)));
			floatInsert(sFloat::KeyboardKeySelectionDuration, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.25f, MINIMAL_DURATION_VALUE)));
			floatInsert(sFloat::FlowSpeedMultiplier, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.0f, 0)));
			floatInsert(sFloat::TextEditScrollSpeedMultiplier, spFloat(new NumericStyleValue<float>(STYLE_BASE_CLASS_NAME, 1.0f, 0)));

			// Initialize vec4 values
			typedef StyleValue_vec4 v4Float; // simplify enum access
			typedef std::shared_ptr<StyleValue<glm::vec4> > spVec4; // simplify shared pointer creation
			std::function<void(v4Float, spVec4)> vec4Insert = [&](v4Float styleValueType, spVec4 styleValue) // simplify map insertion
			{
				mVec4Map.insert(std::make_pair(styleValueType, styleValue));
			};
			vec4Insert(v4Float::GazeVisualizationColor, spVec4(new NumericStyleValue<glm::vec4>(STYLE_BASE_CLASS_NAME, glm::vec4(0, 0, 1.f, 0.5f), VEC_4_ZERO, VEC_4_ONE)));
		}

		// Get a value
		std::shared_ptr<const StyleValue<float> > getValue(StyleValue_float type) const { return mFloatMap.at(type); }
		std::shared_ptr<const StyleValue<glm::vec4> > getValue(StyleValue_vec4 type) const { return mVec4Map.at(type); }

		// Set a value
		void setValue(StyleValue_float type, std::string value) { mFloatMap.at(type)->set(stringToFloat(value)); };
		void setValue(StyleValue_vec4 type, std::string value) { mVec4Map.at(type)->set(stringHexRGBAToVec4RGBA(value)); };

		// Get name
		std::string getName() const { return mName; }

	private:

		// Maps with values
		std::map<StyleValue_float, std::shared_ptr<StyleValue<float> > > mFloatMap;
		std::map<StyleValue_vec4, std::shared_ptr<StyleValue<glm::vec4> > > mVec4Map;

		// Name
		std::string mName = "";
	};
}

#endif // STYLE_CLASS_H_