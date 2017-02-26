//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract superclass for styleable things, like layout and element.

#ifndef STYLEABLE_H_
#define STYLEABLE_H_

#include "src/Style/StyleClass.h"

namespace eyegui
{
	class Styleable
	{
	public:

		// Constructor (can be used both for from laout and element)
		Styleable(
			std::vector<std::string> styles,
			std::function<std::shared_ptr<const StyleClass>(std::string styleClass)> styleFetcher);

		// Destructor
		virtual ~Styleable() = 0;

		// Get names of styles
		std::vector<std::string> getStyleTreeClassesNames() const;

	protected:

		// Getter and setter for computed style value
		virtual float getStyleValue(StylePropertyFloat type) const = 0;
		virtual glm::vec4 getStyleValue(StylePropertyVec4 type) const = 0;

		// Style classes assigned from style tree
		std::vector<std::shared_ptr<const StyleClass> > mStyleTreeClasses; // Parser must guarantee for at least one element

	};
}

#endif // STYLEABLE_H_