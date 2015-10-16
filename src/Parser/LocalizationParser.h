//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef LOCALIZATION_PARSER_H_
#define LOCALIZATION_PARSER_H_

#include "eyeGUI.h"

#include <memory>
#include <map>

namespace eyegui
{
    // Typedefs
    typedef std::map<std::string, std::u16string> localizationMap;

    namespace localization_parser
    {
        // Parsing
        std::unique_ptr<localizationMap> parse(std::string filepath);
    }
}

#endif // LOCALIZATION_PARSER_H_
