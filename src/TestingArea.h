//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Testing area for new structures.

#ifndef TESTING_AREA_H_
#define TESTING_AREA_H_

#include "src/Style/StyleTree.h"
#include <iostream>

namespace eyegui
{
	void testingMain()
	{
		// TODO
		StyleTree styleTree;
		auto header = styleTree.addStyleClass("header");
		auto headerButton = styleTree.addStyleClass("headerButton", "header");
		auto footer = styleTree.addStyleClass("footer");

		
		std::cout << header->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;

		styleTree.setValue("headerButton", StyleType_float::AnimationDuration, "0.2");
		std::cout << header->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;

		styleTree.setValue("header", StyleType_float::AnimationDuration, "0.2");
		std::cout << header->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;

		styleTree.setValue("header", StyleType_float::AnimationDuration, "0");
		std::cout << header->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StyleType_float::AnimationDuration)->get() << std::endl;
	}
}

#endif // TESTING_AREA_H_