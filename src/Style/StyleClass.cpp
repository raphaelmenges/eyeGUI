// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleClass.h"

namespace eyegui
{
	// Using string mapping tuple as this must be always complete
	namespace style_class_helper
	{
		// Extract size of tuple containing mapping from string to property class
		constexpr int tupleSize = std::tuple_size<style::PropertyStringTuple>::value;

		// Going over tuple elements and fill provided maps
		template<std::size_t I = 0>
		inline typename std::enable_if<I == tupleSize, void>::type // base case of I == number of tuple elements
			fillDefaults(const style::PropertyStringTuple& rStringMaps, style::PropertyMaps& rMaps, std::weak_ptr<const StyleClass> wpStyleClass) // takes tuple with string to property map and maps to fill with pointers
		{}

		template<std::size_t I = 0>
		inline typename std::enable_if<I < tupleSize, void>::type // default case
			fillDefaults(const style::PropertyStringTuple& rStringMaps, style::PropertyMaps& rMaps, std::weak_ptr<const StyleClass> wpStyleClass)
		{
			// Extract reference to string map
			const auto& rStringMap = std::get<I>(rStringMaps);

			// Determine type of property (this is mapped type of this string mapping)
			typedef typename std::tuple_element<I, style::PropertyStringTuple>::type::mapped_type PropertyType;

			// Determine property value type
			typedef typename style::PropertyInfo<PropertyType>::type ValueType;

			// Get index in provided rMap to fill value into
			constexpr int index = style::PropertyInfo<PropertyType>::idx;
	
			// Go over map for this property
			for (const auto& entry : rStringMap)
			{
				// Instance of property
				PropertyType property = entry.second;

				// Default value
				ValueType value = style::getPropertyDefault(property);

				// Add entry to map within StyleClass
				std::get<index>(rMaps)[property] = std::shared_ptr<StyleProperty<ValueType> >(new StyleProperty<ValueType>(wpStyleClass, value, &style::PropertyInfo<PropertyType>::constraint));
			}
			
			// Proceed to next tuple element
			fillDefaults<I + 1>(rStringMaps, rMaps, wpStyleClass); // recursion call
		}
	}

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
			mMaps = spParent->mMaps;

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
		// Delegate to fancy template
		style_class_helper::fillDefaults(style::PropertyStringMapping::value, this->mMaps, shared_from_this());
	}

	std::shared_ptr<StyleClass> StyleClassBuilder::construct(std::string name) const
	{
		bool pleaseFill = false;
		auto spStyleClass = std::shared_ptr<StyleClass>(new StyleClass(name, std::weak_ptr<StyleClass>(), pleaseFill));
		if (pleaseFill)	{ spStyleClass->fill();	}
		return spStyleClass;
	}
}