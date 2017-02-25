//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Style class has maps of style property pointers. Some of those properties
// are just references to properties from other classes and some are owned
// by this. If a property is owned, propagation of values of its type
// from parent class is stopped.

#ifndef STYLE_CLASS_H_
#define STYLE_CLASS_H_

#include "src/Style/StyleProperty.h"
#include "src/Utilities/Helper.h"
#include "src/Defines.h"

#include <functional>
#include <map>

namespace eyegui
{
	// Style class, must be built by builder
	class StyleClass : public std::enable_shared_from_this<StyleClass> // enable shared pointer creation from this
	{
	public:

		// Add child. Caller must check whether name is globally unique
		std::shared_ptr<StyleClass> addChild(std::string name);

		// Fetch float property
		std::shared_ptr<const StyleProperty<float> > fetchProperty(StylePropertyFloat type) const;

		// Get float value from property (TODO: const reference?)
		float getValue(StylePropertyFloat type) const;

		// Fetch vec4 property
		std::shared_ptr<const StyleProperty<glm::vec4> > fetchProperty(StylePropertyVec4 type) const;

		// Get vec4 value from property (TODO: const reference?)
		glm::vec4 getValue(StylePropertyVec4 type) const;

		// Set value of float porperty and propagate to children (TODO: maybe do not set it at value parsing failure)
		void setValue(StylePropertyFloat type, std::string value) { setValue(type, stringToFloat(value)); }
		void setValue(StylePropertyFloat type, float value);
		
		// Set vec4 value and propagate to children
		void setValue(StylePropertyVec4 type, std::string value) { setValue(type, stringHexRGBAToVec4RGBA(value)); }
		void setValue(StylePropertyVec4 type, glm::vec4 value);

		// Fetch this or child by name. Return empty pointer if not found
		std::shared_ptr<StyleClass> fetchThisOrChild(std::string name);

		// Get name of this style class
		std::string getName() const;

	private:

		// Enumeration to decide type of construction via builder
		enum class ConstructionType
		{
			BASE_CLASS, // used as root of style tree
			ELEMENT_CLASS // used as class within element
		};

		// Builder is friend class
		friend class StyleClassBuilder;

		// Constructor
		StyleClass(std::string name, std::weak_ptr<const StyleClass> wpParent);

		// Private copy constuctor
		StyleClass(StyleClass const&) {}

		// Private assignment operator
		StyleClass& operator = (StyleClass const&) { return *this; }

		// Called only by builder
		void fill(ConstructionType constructionType);

		// #################################
		// ### MAPS HELPER FOR TEMPLATES ###
		// #################################

		// Get style property pointer by type
		std::shared_ptr<StyleProperty<float> > getStyleProperty(StylePropertyFloat type) { return this->mFloatMap[type]; }
		std::shared_ptr<StyleProperty<glm::vec4> > getStyleProperty(StylePropertyVec4 type) { return this->mVec4Map[type]; }

		// Set property pointer in map
		void setMapValue(StylePropertyFloat type, std::shared_ptr<StyleProperty<float> > spProperty) { this->mFloatMap[type] = spProperty; }
		void setMapValue(StylePropertyVec4 type, std::shared_ptr<StyleProperty<glm::vec4> > spProperty) { this->mVec4Map[type] = spProperty; }

		// ####################
		// ### VALUE SETTER ###
		// ####################

		// Set value as template implementation. Does create new property if not existing in instance
		template<typename Type, typename Value>
		void genericSetValue(Type type, Value value)
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
				// Add new owned property while copying constraint from existing since it is of the same type
				spStoredProperty = std::shared_ptr<StyleProperty<Value> >(new StyleProperty<Value>(shared_from_this(), value, spStoredProperty->getConstraint()));
				this->setMapValue(type, spStoredProperty);

				// Propagate it to children
				for (auto& rspChild : this->mChildren)
				{
					rspChild->genericPropagateProperty(type, spStoredProperty);
				}
			}
		}

		// #########################
		// ### VALUE PROPAGATION ###
		// #########################

		// Propagate value from parent to this and children
		template<typename Type, typename ValueType>
		void genericPropagateProperty(Type type, std::shared_ptr<StyleProperty<ValueType> > spProperty)
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
				for (auto& rspChild : this->mChildren)
				{
					rspChild->genericPropagateProperty(type, spProperty);
				}
			} // else: stop propagation here
		}

		// ###############
		// ### MEMBERS ###
		// ###############

		// Name
		std::string mName = "";

		// Maps
		std::map<StylePropertyFloat, std::shared_ptr<StyleProperty<float> > > mFloatMap;
		std::map<StylePropertyVec4, std::shared_ptr<StyleProperty<glm::vec4> > > mVec4Map;

		// Parent
		std::weak_ptr<const StyleClass> mwpParent;

		// Children
		std::vector<std::shared_ptr<StyleClass> > mChildren;
	};

	// Builder for style class. Takes care of some prerequisites
	class StyleClassBuilder
	{
	public:

		// Construct a style class
		std::shared_ptr<StyleClass> constructBaseClass(std::string name) const; // used for root node of tree
		std::shared_ptr<StyleClass> constructElementClass(std::weak_ptr<const StyleClass> mwpParent) const; // used for class of element
	};
}

#endif // STYLE_CLASS_H_