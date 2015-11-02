//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for layouts. Delegates element parser for actual work.

#ifndef LAYOUT_PARSER_H_
#define LAYOUT_PARSER_H_

#include "ElementParser.h"

namespace eyegui
{
	// Forward declaration
	class GUI;
	class Layout;

	namespace layout_parser
	{
		// Parsing
		std::unique_ptr<Layout> parse(GUI* pGUI, AssetManager* pAssetManager, std::string filepath);
	}
}

#endif // LAYOUT_PARSER_H_
