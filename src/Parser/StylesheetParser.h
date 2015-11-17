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
    namespace stylesheet_parser
    {
        // Parsing
        std::unique_ptr<std::map<std::string, Style> > parse(std::string filepath);

        // Parses whole line and fills value in style struct
        void parseLine(std::string line, Style& rStyle);

        // Fill value (also used by interface etc)
        void fillValue(Style& rStyle, std::string attribute, glm::vec4 value);
    }
}

#endif // STYLESHEET_PARSER_H_
