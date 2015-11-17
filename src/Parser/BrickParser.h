//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for bricks. Delegates element parser for actual work.

#ifndef BRICK_PARSER_H_
#define BRICK_PARSER_H_

#include "ElementParser.h"

namespace eyegui
{
    namespace brick_parser
    {
        // Parsing
        std::unique_ptr<elementsAndIds> parse(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, Element* pParent, std::string filepath, std::map<std::string, std::string> idMapper);
    }
}

#endif // BRICK_PARSER_H_
