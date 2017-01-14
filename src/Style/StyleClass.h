//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Style class has maps of style value pointers. Some of those values
// are just references to values from other classes and some are owned
// by this. If a value is owned, propagation of values of its type
// from parent class is stopped.

#ifndef STYLE_CLASS_H_
#define STYLE_CLASS_H_

#include "src/Style/StyleValue.h"
#include "src/Utilities/Helper.h"
#include "src/Defines.h"

#include <functional>
#include <map>

namespace eyegui
{
	// Style class
	class StyleClass : public std::enable_shared_from_this<StyleClass> // enable shared pointer creation from this
	{
	public:

		// Constructor. Parent for base class is zero
		StyleClass(std::string name, std::weak_ptr<const StyleClass> wpParent);

		// Add child. Tree must check whether name is globally unique
		std::shared_ptr<StyleClass> addChild(std::string name);

		// Fetch float value
		std::shared_ptr<const StyleValue<float> > fetchValue(StyleType_float type) const;

		// Fetch vec4 value
		std::shared_ptr<const StyleValue<glm::vec4> > fetchValue(StyleType_vec4 type) const;

		// Set float value, propagate to children
		void setValue(StyleType_float type, std::string value) { setValue(type, stringToFloat(value)); }
		void setValue(StyleType_float type, float rawValue);
		
		// Set vec4 value, propagate to children
		void setValue(StyleType_vec4 type, std::string value) { setValue(type, stringHexRGBAToVec4RGBA(value)); }
		void setValue(StyleType_vec4 type, glm::vec4 rawValue);

		// Fetch this or child by name. Return empty pointer if not found
		std::shared_ptr<StyleClass> fetchThisOrChild(std::string name);

		// Must be only called on base style class and before any children are added!
		void fillWithBaseValues();

		// Get name
		std::string getName() const;

	private:

		// #################################
		// ### MAPS HELPER FOR TEMPLATES ###
		// #################################

		// Get style class of value
		std::shared_ptr<StyleValue<float> > getStyleValue(StyleType_float type) { return this->mFloatMap[type]; }
		std::shared_ptr<StyleValue<glm::vec4> > getStyleValue(StyleType_vec4 type) { return this->mVec4Map[type]; }

		// Set raw value of value in map
		void setStyleValue(StyleType_float type, std::shared_ptr<StyleValue<float> > spValue) { this->mFloatMap[type] = spValue; }
		void setStyleValue(StyleType_vec4 type, std::shared_ptr<StyleValue<glm::vec4> > spValue) { this->mVec4Map[type] = spValue; }

		// ####################
		// ### VALUE SETTER ###
		// ####################

		// Set value as template implementation
		template<typename RawType, typename Type, typename RawValue>
		void genericSetValue(Type type, RawValue rawValue)
		{
			// Check whether value is owned by me
			auto spStoredValue = this->getStyleValue(type);
			auto spStoredValueStyleClass = spStoredValue->getStyleClass().lock();
			bool owned = false;
			if (spStoredValueStyleClass)
			{
				// Compare raw pointers
				owned = spStoredValueStyleClass.get() == this;
			}

			// Set value by either creating a value or just changing it
			if (owned)
			{
				// Just set it
				spStoredValue->set(rawValue);
			}
			else
			{
				// Add new owned value while copying constraint from it since it is of the same type
				spStoredValue = std::shared_ptr<StyleValue<RawType> >(new StyleValue<RawType>(shared_from_this(), rawValue, spStoredValue->getConstraint()));
				this->setStyleValue(type, spStoredValue);

				// Propagate it to children
				for (auto& rspChild : this->mChildren)
				{
					rspChild->genericPropagateValue<RawType, Type>(type, spStoredValue);
				}
			}
		}

		// #########################
		// ### VALUE PROPAGATION ###
		// #########################

		// Propagate value from parent to this and children
		template<typename RawType, typename Type>
		void genericPropagateValue(Type type, std::shared_ptr<StyleValue<RawType> > spValue)
		{
			// Check whether value is owned by me
			auto spStoredValue = this->getStyleValue(type);
			auto spStoredValueStyleClass = spStoredValue->getStyleClass().lock();
			bool owned = false;
			if (spStoredValueStyleClass)
			{
				// Compare raw pointers
				owned = spStoredValueStyleClass.get() == this;
			}

			// Only store and pass to children if no custom one owned by me
			if (!owned)
			{
				// Store pointer to value
				this->setStyleValue(type, spValue);

				// Propagate it to children
				for (auto& rspChild : this->mChildren)
				{
					rspChild->genericPropagateValue(type, spValue);
				}
			}
		}

		// ###############
		// ### MEMBERS ###
		// ###############

		// Name
		std::string mName = "";

		// Maps
		std::map<StyleType_float, std::shared_ptr<StyleValue<float> > > mFloatMap;
		std::map<StyleType_vec4, std::shared_ptr<StyleValue<glm::vec4> > > mVec4Map;

		// Parent
		std::weak_ptr<const StyleClass> mwpParent;

		// Children
		std::vector<std::shared_ptr<StyleClass> > mChildren;
	};
}

#endif // STYLE_CLASS_H_