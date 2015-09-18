//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for configuration files.

#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include "eyeGUI.h"
#include "Config.h"

namespace eyegui
{
	class ConfigParser
	{
	public:

		// Constructor
		ConfigParser();

		// Parsing
		Config parse(std::string filepath);
	};
}

#endif // CONFIG_PARSER_H_
