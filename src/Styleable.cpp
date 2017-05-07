//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "src/Styleable.h"

namespace eyegui
{
	Styleable::Styleable(
		std::vector<std::string> styles,
		std::function<std::shared_ptr<const StyleClass>(std::string styleClass)> styleFetcher)
	{
		for (const auto& rStyle : styles)
		{
			mStyleTreeClasses.push_back(styleFetcher(rStyle));
		}
	}

	Styleable::~Styleable() {};

	std::vector<std::string> Styleable::getStyleTreeClassesNames() const
	{
		// Collect names
		std::vector<std::string> names;
		for (const auto& rspStyleClass : mStyleTreeClasses)
		{
			names.push_back(rspStyleClass->getName());
		}
		return names;
	}
}