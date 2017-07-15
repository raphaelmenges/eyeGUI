//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Style class has maps of style property pointers. Some of those properties
// are just references to properties from parent classes and other are owned
// by this. If a property is owned, propagation of values of its type from
// parent class is stopped. Children are optionally stored as shared pointer,
// because this is desired for the style tree but not for element style
// classes. Those are already shared pointer in the corresponding element
// and would survive the deletion of the element if stored as shared here.

#ifndef STYLE_CLASS_H_
#define STYLE_CLASS_H_

#include "include/eyeGUI.h"
#include "src/Style/StyleProperty.h"
#include "src/Style/StyleDefinitions.h"
#include "src/Utilities/Helper.h"
#include "src/Defines.h"

#include <functional>

namespace eyegui
{
	// Style class, must be built by builder
	class StyleClass : public std::enable_shared_from_this<StyleClass> // enable shared pointer creation from this
	{
	private: // map interaction must be defined before usage

		// #######################
		// ### MAP INTERACTION ###
		// #######################

		// Get const map corresponding to proptery type
		template<typename Type> std::map<Type, std::shared_ptr<StyleProperty<typename style::PropertyInfo<Type>::type> > > const * getConstMap() const
		{
			return &(std::get<style::PropertyInfo<Type>::idx>(mMaps));
		}

		// Get map corresponding to property type
		template<typename Type> std::map<Type, std::shared_ptr<StyleProperty<typename style::PropertyInfo<Type>::type> > >* getMap()
		{
			return &(std::get<style::PropertyInfo<Type>::idx>(mMaps));
		}

		// Get style property by type
		template<typename Type>
		std::shared_ptr<StyleProperty<typename style::PropertyInfo<Type>::type> > getStyleProperty(Type type) { return (*getMap<Type>())[type]; }

		// Set property pointer in map
		template<typename Type>
		void setMapValue(Type type, std::shared_ptr<StyleProperty<typename style::PropertyInfo<Type>::type> > spProperty) { (*getMap<Type>())[type] = spProperty; }

	public:

		// Add child
		std::shared_ptr<StyleClass> addChild(bool storeShared, std::string name = "");

		// Fetch value from property
		template<typename Type>
		std::shared_ptr<const StyleProperty<typename style::PropertyInfo<Type>::type> > fetchProperty(Type type) const
		{
			return getConstMap<Type>()->at(type);
		}

		// Get value of property
		template<typename Type>
		typename style::PropertyInfo<Type>::type getValue(Type type) const
		{
			return getConstMap<Type>()->at(type)->get();
		}

		// General set value of property and propagate to children
		template<typename Type>
		void setValue(Type type, typename style::PropertyInfo<Type>::type value)
		{
			// Pass to other template
			genericSetValue(type, value);
		}
		
		// Special setters of property values (TODO: there should be a compile time map providing function to map string to value)
		void parseValue(StylePropertyFloat type, std::string value) { setValue(type, stringToFloat(value)); }
		void parseValue(StylePropertyVec4 type, std::string value) { setValue(type, stringHexRGBAToVec4RGBA(value)); }
		void parseValue(StylePropertyString type, std::string value) { setValue(type, value); }

		// Fetch this or child by name. Return empty pointer if not found
		std::shared_ptr<StyleClass> fetchThisOrChild(std::string name);

		// Get name of this style class
		std::string getName() const;

	private:

		// Builder is friend class
		friend class StyleClassBuilder;

		// Constructor
		StyleClass(std::string name, std::weak_ptr<const StyleClass> wpParent, bool& rPleaseFill);

		// Private copy constuctor
		StyleClass(StyleClass const&) {}

		// Private assignment operator
		StyleClass& operator = (StyleClass const&) { return *this; }

		// Called only by builder
		void fill();

		// ####################
		// ### VALUE SETTER ###
		// ####################

		// Set value as template implementation. Does create new property if not existing in instance
		template<typename Type>
		void genericSetValue(Type type, typename style::PropertyInfo<Type>::type value)
		{
			// Check whether property is owned by me
			auto spStoredProperty = this->getStyleProperty(type);
			auto spStoredPropertyStyleClass = spStoredProperty->getStyleClass().lock();
			bool owned = false;
			if (spStoredPropertyStyleClass)
			{
				// Compare raw pointers
				owned = spStoredPropertyStyleClass.get() == this;
			}

			// Set value by either creating a property or just changing it
			if (owned)
			{
				// Just set it
				spStoredProperty->set(value);
			}
			else
			{
				// Add new owned property
				typedef StyleProperty<typename style::PropertyInfo<Type>::type> PropertyType;
				spStoredProperty = std::shared_ptr<PropertyType>(new PropertyType(shared_from_this(), value, &style::PropertyInfo<Type>::constraint, true));
				this->setMapValue(type, spStoredProperty);

				// Propagate it to children
				for (auto& rwpChild : this->mWeakChildren)
				{
					if (auto rspChild = rwpChild.lock())
					{
						rspChild->genericPropagateProperty(type, spStoredProperty);
					}
				}
			}
		}

		// #########################
		// ### VALUE PROPAGATION ###
		// #########################

		// Propagate value from parent to this and children
		template<typename Type>
		void genericPropagateProperty(Type type, std::shared_ptr<StyleProperty<typename style::PropertyInfo<Type>::type> > spProperty)
		{
			// Check whether property is owned by me
			auto spStoredProperty = this->getStyleProperty(type);
			auto spStoredPropertyStyleClass = spStoredProperty->getStyleClass().lock();
			bool owned = false;
			if (spStoredPropertyStyleClass)
			{
				// Compare raw pointers
				owned = spStoredPropertyStyleClass.get() == this;
			}

			// Only store and pass to children if no custom one owned by me
			if (!owned)
			{
				// Store pointer to property
				this->setMapValue(type, spProperty);

				// Propagate it to children
				for (auto& rwpChild : this->mWeakChildren)
				{
					if(auto rspChild = rwpChild.lock())
					{
						rspChild->genericPropagateProperty(type, spProperty);
					}
				}
			} // else: stop propagation here
		}

		// ###############
		// ### MEMBERS ###
		// ###############

		// Name
		std::string mName = "";

		// Maps
		style::PropertyMaps mMaps; // tuple of maps

		// Parent
		std::weak_ptr<const StyleClass> mwpParent; // empty for root

		// Children
		std::vector<std::shared_ptr<StyleClass> > mChildren; // empty for classes belonging to element, only used classes of style tree for storing them
		std::vector<std::weak_ptr<StyleClass> > mWeakChildren; // this is used for all calculations, as it is always contains the children
	};

	// Builder for style class. Takes care of some prerequisites, as style class is always used in shared pointer which produces problems in constructor
	class StyleClassBuilder
	{
	public:

		// Construct a style class (necessary because "shared from" this is used for StyleClass, but this may not be done in its constructor)
		std::shared_ptr<StyleClass> construct(std::string name = "") const;
	};
}

#endif // STYLE_CLASS_H_