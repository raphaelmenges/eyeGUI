// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleClass.h"

namespace eyegui
{
	StyleClass::StyleClass(std::string name, std::weak_ptr<const StyleClass> wpParent, bool& rPleaseFill)
	{
		// Store name
		mName = name;

		// Store pointer to parent
		mwpParent = wpParent;

		// Fill all pointers in map initially with the ones from parent class
		if (auto spParent = mwpParent.lock()) // does not work for root class
		{
			// Copy maps with pointers, not values itself
			mFloatMap = spParent->mFloatMap;
			mVec4Map = spParent->mVec4Map;
			mStringMap = spParent->mStringMap;

			// Please do not fill maps
			rPleaseFill = false;
		}
		else // no parent, so this is some root
		{
			// Please do fill maps
			rPleaseFill = true;
		}
	}

	std::shared_ptr<StyleClass> StyleClass::addChild(bool storeShared, std::string name)
	{
		// Create child
		bool pleaseFill = false;
		auto spChild = std::shared_ptr<StyleClass>(new StyleClass(name, shared_from_this(), pleaseFill));
		if (pleaseFill) { spChild->fill(); } // must be always false

		// Store it optionally as shared pointer so memory is kept. Used by tree
		if (storeShared) { mChildren.push_back(spChild); }

		// Store always weak pointer, used for both element and tree based classes
		mWeakChildren.push_back(spChild);

		// Return shared pointer to child
		return spChild;
	}

	void StyleClass::setValue(StylePropertyFloat type, float value)
	{
		// Pass to template
		genericSetValue(type, value);
	}

	void StyleClass::setValue(StylePropertyVec4 type, glm::vec4 value)
	{
		// Pass to template
		genericSetValue(type, value);
	}

	void StyleClass::setValue(StylePropertyString type, std::string value)
	{
		// Pass to template
		genericSetValue(type, value);
	}

	std::shared_ptr<StyleClass> StyleClass::fetchThisOrChild(std::string name)
	{
		// Ok, this is the searched class since name is equal to searched one
		if (this->mName == name)
		{
			return shared_from_this();
		}
		else
		{
			// Go over children
			for (auto& rwpChild : mWeakChildren)
			{
				// Return result if it is something found
				if (auto rspChild = rwpChild.lock())
				{
					auto result = rspChild->fetchThisOrChild(name);
					if (result)
					{
						return result;
					}
				}
			}
		}

		// Return empty pointer as fallback
		return std::shared_ptr<StyleClass>();
	}

	std::string StyleClass::getName() const
	{
		return mName;
	}

	void StyleClass::fill()
	{
		// TODO NOTE: ADD INITIAL VALUES AND CONSTRAINTS OF NEW PROPERTIES HERE! Also remember to add string mapping in StylePropertyNameMapper.cpp

		// Float constraints
		const std::function<float(float)> durationConstraint = [](float value)
		{
			return std::move(glm::max(MINIMAL_DURATION_VALUE, value));
		};
		const std::function<float(float)> positiveConstraint = [](float value)
		{
			return std::move(glm::max(0.f, value));
		};
		const std::function<float(float)> zeroAndOneConstraint = [](float value)
		{
			return std::move(glm::clamp(value, 0.f, 1.f));
		};

		// Initialize float properties
		typedef StylePropertyFloat sFloat; // simplify enum access
		typedef std::shared_ptr<StyleProperty<float> > spFloat; // simplify shared pointer creation
		mFloatMap[sFloat::AnimationDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 0.1f, durationConstraint));
		mFloatMap[sFloat::SensorPenetrationIncreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 3.0f, durationConstraint));
		mFloatMap[sFloat::SensorPenetrationDecreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.5f, durationConstraint));
		mFloatMap[sFloat::ButtonThresholdIncreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint));
		mFloatMap[sFloat::ButtonThresholdDecreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 2.0f, durationConstraint));
		mFloatMap[sFloat::ButtonPressingDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 0.3f, durationConstraint));
		mFloatMap[sFloat::SensorInteractionPenetrationAmount] = spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, positiveConstraint));
		mFloatMap[sFloat::DimIncreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.5f, durationConstraint));
		mFloatMap[sFloat::DimDecreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 0.25f, durationConstraint));
		mFloatMap[sFloat::DimAlpha] = spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, zeroAndOneConstraint));
		mFloatMap[sFloat::FlashDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 2.0f, durationConstraint));
		mFloatMap[sFloat::MaximalAdaptiveScaleIncrease] = spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, positiveConstraint));
		mFloatMap[sFloat::AdaptiveScaleIncreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint));
		mFloatMap[sFloat::AdaptiveScaleDecreaseDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint));
		mFloatMap[sFloat::KeyboardZoomSpeedMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));
		mFloatMap[sFloat::KeyboardKeySelectionDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.25f, durationConstraint));
		mFloatMap[sFloat::FlowSpeedMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));
		mFloatMap[sFloat::TextEditScrollSpeedMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));

		// Experimental
		mFloatMap[sFloat::FutureKeyboardPressDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, durationConstraint));
		mFloatMap[sFloat::FutureKeyboardRetriggerDelay] = spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, positiveConstraint));
		mFloatMap[sFloat::FutureKeyboardThresholdDuration] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint));
		mFloatMap[sFloat::FutureKeyboardRepeatKeyThresholdMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));
		mFloatMap[sFloat::FutureKeyboardSpaceKeyThresholdMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));
		mFloatMap[sFloat::FutureKeyboardBackspaceKeyThresholdMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));
		mFloatMap[sFloat::FutureKeyboardSuggestionLineThresholdMultiplier] = spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint));

		// Vec4 constraints
		const std::function<glm::vec4(glm::vec4)> colorConstraint = [](glm::vec4 value)
		{
			return std::move(glm::clamp(value, VEC_4_ZERO, VEC_4_ONE));
		};

		// Initialize vec4 properties
		typedef StylePropertyVec4 sVec4; // simplify enum access
		typedef std::shared_ptr<StyleProperty<glm::vec4> > spVec4; // simplify shared pointer creation
		mVec4Map[sVec4::Color] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), colorConstraint));
		mVec4Map[sVec4::BackgroundColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), colorConstraint));
		mVec4Map[sVec4::HighlightColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), colorConstraint));
		mVec4Map[sVec4::SeparatorColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), colorConstraint));
		mVec4Map[sVec4::SelectionColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 1.0f, 1.0f, 0.5f), colorConstraint));
		mVec4Map[sVec4::IconColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), colorConstraint));
		mVec4Map[sVec4::FontColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), colorConstraint));
		mVec4Map[sVec4::DimColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.1f, 0.1f, 0.1f, 0.75f), colorConstraint));
		mVec4Map[sVec4::FlashColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 0.5f, 0.0f, 0.75f), colorConstraint));
		mVec4Map[sVec4::MarkColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 0.5f, 1.0f, 0.5f), colorConstraint));
		mVec4Map[sVec4::PickColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.2f, 1.0f, 0.0f, 0.5f), colorConstraint));
		mVec4Map[sVec4::ThresholdColor] = spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 1.0f, 1.0f, 0.5f), colorConstraint));

		// Initialize string properties
		typedef StylePropertyString sString; // simplify enum access
		typedef std::shared_ptr<StyleProperty<std::string> > spString; // simplify shared pointer creation
		mStringMap[sString::SoundButtonDown] = spString(new StyleProperty<std::string>(shared_from_this(), std::string()));
	}

	std::shared_ptr<StyleClass> StyleClassBuilder::construct(std::string name) const
	{
		bool pleaseFill = false;
		auto spStyleClass = std::shared_ptr<StyleClass>(new StyleClass(name, std::weak_ptr<StyleClass>(), pleaseFill));
		if (pleaseFill)	{ spStyleClass->fill();	}
		return spStyleClass;
	}
}