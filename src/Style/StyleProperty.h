//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Single style property. Applies given constraint on itself when value is set.
// Has pointer to style class who owns this property.

#ifndef STYLE_PROPERTY_H_
#define STYLE_PROPERTY_H_

#include "externals/GLM/glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>

namespace eyegui
{
	// Forward declaration
	class StyleClass;

	// Style property
	template<typename T>
	class StyleProperty
	{
	public:

		// Constructor takes pointer to class thats owns the property and an initial value. Optionally a constraint that is checked at set can be added 
		StyleProperty(std::weak_ptr<const StyleClass> wpStyleClass, T value, std::function<T(T)> constraint = [](T value) {return value; })
		{
			this->mwpStyleClass = wpStyleClass;
			this->mConstraint = std::move(constraint);
			this->mValue = this->mConstraint(value);
		}

		// Get style class that owns this
		std::weak_ptr<const StyleClass> getStyleClass() const {	return mwpStyleClass; };

		// Set value while applying the constraint
		virtual void set(T value) { this->mValue = this->mConstraint(value); }

		// Get value
		T get() const { return this->mValue; }

		// Get copy of constraint
		std::function<T(T)> getConstraint() const { return this->mConstraint; }

	protected:

		// Pointer to style class that holds this property. Might be an invalid pointer
		std::weak_ptr<const StyleClass> mwpStyleClass;

		// Value
		T mValue;

		// Constraint function
		std::function<T(T)> mConstraint;
	};
}

#endif // STYLE_PROPERTY_H_