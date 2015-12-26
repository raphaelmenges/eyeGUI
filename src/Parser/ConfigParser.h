//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for configuration files.

#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include "include/eyeGUI.h"
#include "Config.h"

namespace eyegui
{
    namespace config_parser
    {
        // Parsing
        Config parse(std::string filepath);

        // Fill value
        void fillValue(Config& rConfig, std::string attribute, std::string value, std::string filepath);
    }
}

#endif // CONFIG_PARSER_H_
