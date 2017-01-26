//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Helper functions for parsers.

#ifndef PARSER_HELPER_H_
#define PARSER_HELPER_H_

#include "include/eyeGUI.h"
#include "src/Elements/Elements.h"
#include "src/Defines.h"
#include "externals/TinyXML2/tinyxml2.h"

namespace eyegui
{
	namespace parser_helpers
	{
		// Helpers
		void blockHelper(tinyxml2::XMLElement const * xmlBlock, bool& rConsumeInput, std::string& rBackgroundFilepath, ImageAlignment& rBackgroundAlignment, float& rInnerBorder);
		void fontSizeHelper(tinyxml2::XMLElement const * xmlElement, FontSize& rFontSize, std::string filepath);
		void localizationHelper(tinyxml2::XMLElement const * xmlElement, std::string contentAttribute, std::string keyAttribute, std::u16string& rContent, std::string& rKey);

		// Checking
		bool validateElement(tinyxml2::XMLElement const * xmlElement, const std::string& rExpectedValue);

		// Attribute parsing
		float parseRelativeScale(tinyxml2::XMLElement const * xmlElement);
		std::string parseStringAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, std::string fallback = EMPTY_STRING_ATTRIBUTE);
		bool parseBoolAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, bool fallback = EMPTY_BOOL_ATTRIBUTE);
		int parseIntAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, int fallback = EMPTY_INT_ATTRIBUTE);
		float parsePercentAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, float fallback = EMPTY_PERCENTAGE_ATTRIBUTE);
	}
}

#endif // PARSER_HELPER_H_