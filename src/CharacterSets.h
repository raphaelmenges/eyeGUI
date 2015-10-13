//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Character sets for generating texture atlases.

#ifndef CHARACTER_SETS_H_
#define CHARACTER_SETS_H_

#include <set>

namespace eyegui
{
    namespace charsets
    {
        static const std::set<char16_t> BASIC
        {
            u'0', u'1'
        };

        static const std::set<char16_t> GERMANY_GERMAN
        {
            u'a', u'b', u'\u00E4'
        };
    }
}

#endif // CHARACTER_SETS_H_
