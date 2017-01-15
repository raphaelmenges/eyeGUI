//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for style sheet files.

#ifndef STYLE_PARSER_H_
#define STYLE_PARSER_H_

#include "include/eyeGUI.h"
#include "src/Style/StyleTree.h"

namespace eyegui
{
    namespace style_parser
    {
        // Parse file and attach to existing style tree
        void parse(std::shared_ptr<StyleTree> spStyleTree, std::string filepath);
    }
}

#endif // STYLE_PARSER_H_
