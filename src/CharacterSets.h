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
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',
            'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
            's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
            'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X', 'Y', 'Z', '-', '_', ',', ';', '.', ':', '+', '*',
            '~', '#', '/', '?', '!', '%', '#', ' ', '<', '>', '|', '$', '@', '^',
            '"', '=', '&', '(', ')', '[', ']', '{', '}', '\\',
            u'\u00A7' // Paragraph
        };

        static const std::set<char16_t> GERMANY_GERMAN
        {
            u'\u00c4', // AE
            u'\u00e4', // ae
            u'\u00d6', // OE
            u'\u00f6', // oe
            u'\u00dc', // UE
            u'\u00fc', // ue
            u'\u00df', // sz
            u'\u20AC'  // Euro
        };
    }
}

#endif // CHARACTER_SETS_H_
