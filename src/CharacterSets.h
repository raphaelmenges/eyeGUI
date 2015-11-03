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
			'G', 'H', 'I', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
			'V', 'W', 'X', 'Y', 'Z', '-', '_', ',', ';', '.', ':', '+', '*', '~',
			'#', '/', '?', '!', '%', '#', ' ', '<', '>', '|'
		};

		static const std::set<char16_t> GERMANY_GERMAN
		{
			u'\u00c4', u'\u00e4', u'\u00d6', u'\u00f6', u'\u00dc', u'\u00fc', u'\u00df'
		};
	}
}

#endif // CHARACTER_SETS_H_
