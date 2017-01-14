//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Style class holds map of style value pointers. Some of those values
// are just references to values from other classes and some are held
// by this. If a value is held, propagation of values of its type
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

		// Set float value, propagated to children
		void setValue(StyleType_float type, std::string value) { setValue(type, stringToFloat(value)); }
		void setValue(StyleType_float type, float rawValue);
		
		// Set vec4 value, propagated to children
		void setValue(StyleType_vec4 type, std::string value) { setValue(type, stringHexRGBAToVec4RGBA(value)); }
		void setValue(StyleType_vec4 type, glm::vec4 rawValue);

		// Fetch this or child by name. Return empty pointer if not found
		std::shared_ptr<StyleClass> fetchThisOrChild(std::string name);

		// Must be only called on base style class and before any children are added!
		void fillWithBaseValues();

		// Get name
		std::string getName() const;

	private:

		// ###################
		// ### MAPS GETTER ###
		// ###################

		// Template to get maps in templates at compile time
		template<typename RawType, typename Type>
		std::map<Type, std::shared_ptr<StyleValue<RawType> > >& getMapReference()
		{
			// Does break when called (reference to local variable)
			return std::map<Type, std::shared_ptr<StyleValue<RawType> > >();
		}

		// Specialization for float map
		template<>
		std::map<StyleType_float, std::shared_ptr<StyleValue<float> > >& getMapReference()
		{
			return mFloatMap;
		}

		// Specialization for vec4 map
		template<>
		std::map<StyleType_vec4, std::shared_ptr<StyleValue<glm::vec4> > >& getMapReference()
		{
			return mVec4Map;
		}

		// ####################
		// ### VALUE SETTER ###
		// ####################

		// Set value as template implementation
		template<typename RawType, typename Type, typename RawValue>
		void setValue(Type type, RawValue rawValue)
		{
			// Get map reference
			auto& rMap = getMapReference<RawType, Type>();

			// Check whether value is owned by me
			auto spValue = rMap.at(type);
			auto spValueStyleClass = spValue->getStyleClass().lock();
			auto spThisStyleClass = mwpParent.lock();
			bool owned = false;
			if (spValueStyleClass && spThisStyleClass)
			{
				// Compare raw pointers
				owned = spValueStyleClass.get() == spThisStyleClass.get();
			}

			// Set value by either creating a value or just changing it
			if (owned)
			{
				// Just set it
				spValue->set(rawValue);
			}
			else
			{
				// Add value
				spValue = std::shared_ptr<StyleValue<RawType> >(new StyleValue<RawType>(shared_from_this(), rawValue));
				rMap[type] = spValue;

				// Propagate it to children
				for (auto& rspChild : mChildren)
				{
					rspChild->propagateValue<RawType, Type>(type, spValue);
				}
			}
		}

		// #########################
		// ### VALUE PROPAGATION ###
		// #########################

		// Propagate value from parent to this and children
		template<typename RawType, typename Type>
		void propagateValue(Type type, std::shared_ptr<StyleValue<RawType> > spValue)
		{
			// Get map reference
			auto& rMap = getMapReference<RawType, Type>();

			// Check whether value is owned by me
			auto spStoredValue = rMap.at(type);
			auto spStoredValueStyleClass = spStoredValue->getStyleClass().lock();
			auto spThisStyleClass = mwpParent.lock();
			bool owned = false;
			if (spStoredValueStyleClass && spThisStyleClass)
			{
				// Compare raw pointers
				owned = spStoredValueStyleClass.get() == spThisStyleClass.get();
			}

			// Only store and pass to children if no custom one owned by me
			if (!owned)
			{
				// Store pointer to value
				rMap[type] = spValue;

				// Propagate it to children
				for (auto& rspChild : mChildren)
				{
					rspChild->propagateValue(type, spValue);
				}
			}
		}

		// ###############
		// ### MEMBERS ###
		// ###############

		// Maps with pointers to values
		std::map<StyleType_float, std::shared_ptr<StyleValue<float> > > mFloatMap;
		std::map<StyleType_vec4, std::shared_ptr<StyleValue<glm::vec4> > > mVec4Map;

		// Name
		std::string mName = "";

		// Parent
		std::weak_ptr<const StyleClass> mwpParent;

		// Children
		std::vector<std::shared_ptr<StyleClass> > mChildren;
	};
}

#endif // STYLE_CLASS_H_