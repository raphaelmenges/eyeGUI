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
		// Basic Latin characters
        static const std::set<char16_t> BASIC
        {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',
            'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
            's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
            'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X', 'Y', 'Z', '-', '_', ',', ';', '.', ':', '+', '*',
            '~', '#', '/', '?', '!', '%', '#', ' ', '<', '>', '|', '$', '@', '^',
            '"', '=', '&', '(', ')', '[', ']', '{', '}', '\\', '\'',
            u'\u00A7', // paragraph

            // Experimental
            u'\u21AA', // return symbol
            u'\u21A4', // backspace symbol
            u'\u2423', // space symbol
            u'\u2191', // shift symbol
            u'\u21BA', // repeat symbol
			u'\u00B0' // degree symbol
        };

		// Additional German letters
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

		// Modern Hebrew
		static const std::set<char16_t> ISRAEL_HEBREW
		{
			u'\u05E9',
			u'\u05D3',
			u'\u05D2',
			u'\u05DB',
			u'\u05E2',
			u'\u05D9',
			u'\u05D7',
			u'\u05DC',
			u'\u05DA',
			u'\u05E3',
			u'\u05D6',
			u'\u05E1',
			u'\u05D1',
			u'\u05D4',
			u'\u05E0',
			u'\u05DE',
			u'\u05E6',
			u'\u05EA',
			u'\u05E5',
			u'\u20AA',
			u'\u05F2',
			u'\u05F1',
			u'\u05F0',
			u'\u05E7',
			u'\u05E8',
			u'\u05D0',
			u'\u05D8',
			u'\u05D5',
			u'\u05DF',
			u'\u05DD',
			u'\u05E4'
		};
    }
}

#endif // CHARACTER_SETS_H_
