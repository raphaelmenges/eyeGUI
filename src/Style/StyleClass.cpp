// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleClass.h"

namespace eyegui
{
	StyleClass::StyleClass(std::string name, std::weak_ptr<const StyleClass> wpParent)
	{
		// Store name
		mName = name;

		// Store pointer to parent
		mwpParent = wpParent;

		// Fill all pointers in map initially with the ones from parent class
		if (auto spParent = mwpParent.lock()) // does not work for base class
		{
			// Copy maps with pointers, not values itself
			mFloatMap = spParent->mFloatMap;
			mVec4Map = spParent->mVec4Map;
		}
	}

	std::shared_ptr<StyleClass> StyleClass::addChild(std::string name)
	{
		// Just push back to vector. It must be checked by tree if name is globally unique!
		auto spChild = std::shared_ptr<StyleClass>(new StyleClass(name, shared_from_this()));
		mChildren.push_back(spChild);
		return spChild;
	}

	std::shared_ptr<const StyleProperty<float> > StyleClass::fetchProperty(StylePropertyFloat type) const
	{
		return mFloatMap.at(type);
	}

	float StyleClass::getValue(StylePropertyFloat type) const
	{
		return mFloatMap.at(type)->get();
	}

	std::shared_ptr<const StyleProperty<glm::vec4> > StyleClass::fetchProperty(StylePropertyVec4 type) const
	{
		return mVec4Map.at(type);
	}

	glm::vec4 StyleClass::getValue(StylePropertyVec4 type) const
	{
		return mVec4Map.at(type)->get();
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
			for (auto& rspChild : mChildren)
			{
				// Return result if it is something found
				auto result = rspChild->fetchThisOrChild(name);
				if (result)
				{
					return result;
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

	void StyleClass::fill(ConstructionType constructionType)
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
		std::function<void(sFloat, spFloat)> floatInsert = [&](sFloat type, spFloat&& value) // simplify map insertion
		{
			if (constructionType == ConstructionType::BASE_CLASS)
			{
				value->markBase(); // tell property that is in base style class
				mFloatMap[type] = value; // use value for style class in base
			}
			else // element class
			{
				mFloatMap[type] = value; // TODO think about this
			}
		};
		floatInsert(sFloat::AnimationDuration,					spFloat(new StyleProperty<float>(shared_from_this(), 0.1f, durationConstraint)));
		floatInsert(sFloat::SensorPenetrationIncreaseDuration,	spFloat(new StyleProperty<float>(shared_from_this(), 3.0f, durationConstraint)));
		floatInsert(sFloat::SensorPenetrationDecreaseDuration,	spFloat(new StyleProperty<float>(shared_from_this(), 1.5f, durationConstraint)));
		floatInsert(sFloat::ButtonThresholdIncreaseDuration,	spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint)));
		floatInsert(sFloat::ButtonThresholdDecreaseDuration,	spFloat(new StyleProperty<float>(shared_from_this(), 2.0f, durationConstraint)));
		floatInsert(sFloat::ButtonPressingDuration,				spFloat(new StyleProperty<float>(shared_from_this(), 0.3f, durationConstraint)));
		floatInsert(sFloat::SensorInteractionPenetrationAmount, spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, positiveConstraint)));
		floatInsert(sFloat::DimIncreaseDuration,				spFloat(new StyleProperty<float>(shared_from_this(), 1.5f, durationConstraint)));
		floatInsert(sFloat::DimDecreaseDuration,				spFloat(new StyleProperty<float>(shared_from_this(), 0.25f, durationConstraint)));
		floatInsert(sFloat::DimAlpha,							spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, zeroAndOneConstraint)));
		floatInsert(sFloat::FlashDuration,						spFloat(new StyleProperty<float>(shared_from_this(), 2.0f, durationConstraint)));
		floatInsert(sFloat::MaximalAdaptiveScaleIncrease,		spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, positiveConstraint)));
		floatInsert(sFloat::AdaptiveScaleIncreaseDuration,		spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint)));
		floatInsert(sFloat::AdaptiveScaleDecreaseDuration,		spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint)));
		floatInsert(sFloat::KeyboardZoomSpeedMultiplier,		spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));
		floatInsert(sFloat::KeyboardKeySelectionDuration,		spFloat(new StyleProperty<float>(shared_from_this(), 1.25f, durationConstraint)));
		floatInsert(sFloat::FlowSpeedMultiplier,				spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));
		floatInsert(sFloat::TextEditScrollSpeedMultiplier,		spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));

		// Experimental
		floatInsert(sFloat::FutureKeyboardPressDuration,						spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, durationConstraint)));
		floatInsert(sFloat::FutureKeyboardRetriggerDelay,						spFloat(new StyleProperty<float>(shared_from_this(), 0.5f, positiveConstraint)));
		floatInsert(sFloat::FutureKeyboardThresholdDuration,					spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, durationConstraint)));
		floatInsert(sFloat::FutureKeyboardRepeatKeyThresholdMultiplier,			spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));
		floatInsert(sFloat::FutureKeyboardSpaceKeyThresholdMultiplier,			spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));
		floatInsert(sFloat::FutureKeyboardBackspaceKeyThresholdMultiplier,		spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));
		floatInsert(sFloat::FutureKeyboardSuggestionLineThresholdMultiplier,	spFloat(new StyleProperty<float>(shared_from_this(), 1.0f, positiveConstraint)));

		// Vec4 constraints
		const std::function<glm::vec4(glm::vec4)> colorConstraint = [](glm::vec4 value)
		{
			return std::move(glm::clamp(value, VEC_4_ZERO, VEC_4_ONE));
		};

		// Initialize vec4 properties
		typedef StylePropertyVec4 sVec4; // simplify enum access
		typedef std::shared_ptr<StyleProperty<glm::vec4> > spVec4; // simplify shared pointer creation
		std::function<void(sVec4, spVec4)> vec4Insert = [&](sVec4 type, spVec4&& value) // simplify map insertion
		{
			if (constructionType == ConstructionType::BASE_CLASS)
			{
				value->markBase(); // tell property that is in base style class
				mVec4Map[type] = value; // use value for style class in base
			}
			else // element class
			{
				mVec4Map[type] = value; // TODO think about this
			}
		};
		vec4Insert(sVec4::Color,									spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), colorConstraint)));
		vec4Insert(sVec4::BackgroundColor,							spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), colorConstraint)));
		vec4Insert(sVec4::HighlightColor,							spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), colorConstraint)));
		vec4Insert(sVec4::SeparatorColor,							spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), colorConstraint)));
		vec4Insert(sVec4::SelectionColor,							spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 1.0f, 1.0f, 0.5f), colorConstraint)));
		vec4Insert(sVec4::IconColor,								spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), colorConstraint)));
		vec4Insert(sVec4::FontColor,								spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), colorConstraint)));
		vec4Insert(sVec4::DimColor,									spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.1f, 0.1f, 0.1f, 0.75f), colorConstraint)));
		vec4Insert(sVec4::FlashColor,								spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(1.0f, 0.5f, 0.0f, 0.75f), colorConstraint)));
		vec4Insert(sVec4::MarkColor,								spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 0.5f, 1.0f, 0.5f), colorConstraint)));
		vec4Insert(sVec4::PickColor,								spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.2f, 1.0f, 0.0f, 0.5f), colorConstraint)));
		vec4Insert(sVec4::ThresholdColor,							spVec4(new StyleProperty<glm::vec4>(shared_from_this(), glm::vec4(0.0f, 1.0f, 1.0f, 0.5f), colorConstraint)));
	}

	std::shared_ptr<StyleClass> StyleClassBuilder::constructBaseClass(std::string name) const
	{
		auto spStyleClass = std::shared_ptr<StyleClass>(new StyleClass(name, std::shared_ptr<const StyleClass>()));
		spStyleClass->fill(StyleClass::ConstructionType::BASE_CLASS); // otherwise, class would have no content in maps
		return spStyleClass;
	}

	std::shared_ptr<StyleClass> StyleClassBuilder::constructElementClass(std::weak_ptr<const StyleClass> mwpParent) const
	{
		auto spStyleClass = std::shared_ptr<StyleClass>(new StyleClass("", mwpParent));
		spStyleClass->fill(StyleClass::ConstructionType::ELEMENT_CLASS); // otherwise, class would have no content in maps
		return spStyleClass;
	}
}