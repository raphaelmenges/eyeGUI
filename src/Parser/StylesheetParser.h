//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for stylesheets extracting style structs.

#ifndef STYLESHEET_PARSER_H_
#define STYLESHEET_PARSER_H_

#include "eyeGUI.h"
#include "Style.h"

#include <memory>
#include <map>

namespace eyegui
{
	class StylesheetParser
	{
	public:

		// Constructor
		StylesheetParser();

		// Parsing
		std::unique_ptr<std::map<std::string, Style> > parse(std::string filepath);

	private:

		// Parses whole line and fills value in style struct
		void parseValue(std::string value, Style& rStyle, std::string filepath) const;

		// Parse color
		glm::vec4 parseColor(std::string value) const;
	};
}

#endif // STYLESHEET_PARSER_H_
