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

		// Constructor
		Styleable(std::vector<std::string> styles, std::function<std::shared_ptr<const StyleClass>(std::string styleName)> styleFetcher)
		{
			for (const auto& rStyle : styles)
			{
				mStyleTreeClasses.push_back(styleFetcher(rStyle));
			}
		}

		// Destructor
		virtual ~Styleable() = 0;

		// Get names of styles
		std::vector<std::string> getStyleTreeClassesNames() const
		{
			// Collect names
			std::vector<std::string> names;
			for (const auto& rspStyleClass : mStyleTreeClasses)
			{
				names.push_back(rspStyleClass->getName());
			}
			return names;
		}

	protected:

		// Getter and setter for style value
		virtual float getStyleValue(StylePropertyFloat type) const = 0;
		virtual glm::vec4 getStyleValue(StylePropertyVec4 type) const = 0;

		// StyleClasses from GUI style tree
		std::vector<std::shared_ptr<const StyleClass> > mStyleTreeClasses; // Parser must guarantee for at least one element

	};
}

#endif // STYLEABLE_H_