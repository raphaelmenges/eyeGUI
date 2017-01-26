//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ParserHelpers.h"
#include "src/Utilities/OperationNotifier.h"

namespace eyegui
{
	namespace parser_helpers
	{
		void blockHelper(tinyxml2::XMLElement const * xmlBlock, bool& rConsumeInput, std::string& rBackgroundFilepath, ImageAlignment& rBackgroundAlignment, float& rInnerBorder)
		{
			rConsumeInput = parseBoolAttribute("consumeinput", xmlBlock);
			rBackgroundFilepath = parseStringAttribute("backgroundsrc", xmlBlock);

			// Background image alignment
			if (rBackgroundFilepath == EMPTY_STRING_ATTRIBUTE)
			{
				// Not necessary, so just use something
				rBackgroundAlignment = STANDARD_IMAGE_ALIGNMENT;
			}
			else
			{
				std::string backgroundAlignmentValue = parseStringAttribute("backgroundalignment", xmlBlock);
				if (backgroundAlignmentValue == "original")
				{
					rBackgroundAlignment = ImageAlignment::ORIGINAL;
				}
				else if (backgroundAlignmentValue == "stretched")
				{
					rBackgroundAlignment = ImageAlignment::STRETCHED;
				}
				else if (backgroundAlignmentValue == "zoomed")
				{
					rBackgroundAlignment = ImageAlignment::ZOOMED;
				}
				else
				{
					// Just use the standard
					rBackgroundAlignment = STANDARD_IMAGE_ALIGNMENT;
				}
			}

			rInnerBorder = parsePercentAttribute("innerborder", xmlBlock);
		}

		void fontSizeHelper(tinyxml2::XMLElement const * xmlElement, FontSize& rFontSize, std::string filepath)
		{
			std::string fontSizeValue = parseStringAttribute("fontsize", xmlElement);
			if (fontSizeValue == EMPTY_STRING_ATTRIBUTE || fontSizeValue == "medium")
			{
				rFontSize = FontSize::MEDIUM;
			}
			else if (fontSizeValue == "tall")
			{
				rFontSize = FontSize::TALL;
			}
			else if (fontSizeValue == "small")
			{
				rFontSize = FontSize::SMALL;
			}
			else
			{
				throwError(OperationNotifier::Operation::PARSING, "Unknown font size used in element: " + fontSizeValue, filepath);
			}
		}

		void localizationHelper(tinyxml2::XMLElement const * xmlElement, std::string contentAttribute, std::string keyAttribute, std::u16string& rContent, std::string& rKey)
		{
			// Get content
			std::string contentValue = parseStringAttribute(contentAttribute, xmlElement);

			// Xml parser replaces new lines with visible "\n"
			replaceString(contentValue, "\\n", "\n");

			// Convert to utf-16 string
			convertUTF8ToUTF16(contentValue, rContent);

			// Get key for localization
			rKey = parseStringAttribute(keyAttribute, xmlElement);
		}

		bool validateElement(tinyxml2::XMLElement const * xmlElement, const std::string& rExpectedValue)
		{
			if (xmlElement == NULL || std::string(xmlElement->Value()).compare(rExpectedValue) != 0)
			{
				return false;
			}
			return true;
		}

		float parseRelativeScale(tinyxml2::XMLElement const * xmlElement)
		{
			if (xmlElement == NULL)
			{
				return 1;
			}
			else
			{
				float value = parsePercentAttribute("relativescale", xmlElement);
				if (value <= 0)
				{
					return 1;
				}
				else
				{
					return value;
				}
			}
		}

		std::string parseStringAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, std::string fallback)
		{
			tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
			if (xmlAttribute != NULL)
			{
				return xmlAttribute->Value();
			}
			else
			{
				return fallback;
			}
		}

		bool parseBoolAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, bool fallback)
		{
			tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
			if (xmlAttribute != NULL)
			{
				std::string value = xmlAttribute->Value();

				// File format case is not relevant
				std::transform(value.begin(), value.end(), value.begin(), ::tolower);

				return value == "true";
			}
			else
			{
				return fallback;
			}
		}

		int parseIntAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, int fallback)
		{
			tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
			if (xmlAttribute != NULL)
			{
				return xmlAttribute->IntValue();
			}
			else
			{
				return fallback;
			}
		}

		float parsePercentAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, float fallback)
		{
			tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
			if (xmlAttribute != NULL)
			{
				std::string value = xmlAttribute->Value();
				std::string delimiter = "%";
				std::string token = value.substr(0, value.find(delimiter));
				return (stringToFloat(token) / 100.0f);
			}
			else
			{
				return fallback;
			}
		}
	}
}

