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
	// Text direction defined by character
	enum class CharacterDirection { LEFT_TO_RIGHT, RIGHT_TO_LEFT, NEUTRAL, PARENTHESIS}; // extra enum for parentheses, as they are rendered mirrored for right to left languages

	// Simple class for character defintion including direction
	class Character
	{
	public:

		// Constructors
		Character(char16_t character, CharacterDirection direction)
		{
			this->character = character;
			this->direction = direction;
		}
		Character(char16_t character) : Character(character, CharacterDirection::LEFT_TO_RIGHT)
		{}

		// Implementing operators
		bool operator< (const Character &right) const
		{
			return character < right.character;
		}

		// Members
		char16_t character;
		CharacterDirection direction;
	};

    namespace charsets
    {
		// Basic Latin characters
        static const std::set<Character> BASIC
        {
			// Left to right do not need full contructor
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd',
            'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
            's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
            'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
            'U', 'V', 'W', 'X', 'Y', 'Z',

			// Character neutral to text direction
			Character(u'-', CharacterDirection::NEUTRAL), 
			Character(u'_', CharacterDirection::NEUTRAL),
			Character(u',', CharacterDirection::NEUTRAL),
			Character(u';', CharacterDirection::NEUTRAL),
			Character(u'.', CharacterDirection::NEUTRAL),
			Character(u':', CharacterDirection::NEUTRAL),
			Character(u'+', CharacterDirection::NEUTRAL),
			Character(u'*', CharacterDirection::NEUTRAL),
            Character(u'~', CharacterDirection::NEUTRAL),
			Character(u'#', CharacterDirection::NEUTRAL),
			Character(u'/', CharacterDirection::NEUTRAL),
			Character(u'?', CharacterDirection::NEUTRAL),
			Character(u'!', CharacterDirection::NEUTRAL),
			Character(u'%', CharacterDirection::NEUTRAL),
			Character(u'#', CharacterDirection::NEUTRAL),
			Character(u' ', CharacterDirection::NEUTRAL),
			Character(u'|', CharacterDirection::NEUTRAL),
			Character(u'$', CharacterDirection::NEUTRAL),
			Character(u'@', CharacterDirection::NEUTRAL),
			Character(u'^', CharacterDirection::NEUTRAL),
            Character(u'"', CharacterDirection::NEUTRAL),
			Character(u'=', CharacterDirection::NEUTRAL),
			Character(u'&', CharacterDirection::NEUTRAL),
			Character(u'\\', CharacterDirection::NEUTRAL),
			Character(u'/', CharacterDirection::NEUTRAL),
			Character(u'\u00A7', CharacterDirection::NEUTRAL), // paragraph

			// Parentheses
			Character(u'(', CharacterDirection::PARENTHESIS),
			Character(u')', CharacterDirection::PARENTHESIS),
			Character(u'[', CharacterDirection::PARENTHESIS),
			Character(u']', CharacterDirection::PARENTHESIS),
			Character(u'{', CharacterDirection::PARENTHESIS),
			Character(u'}', CharacterDirection::PARENTHESIS),
			Character(u'<', CharacterDirection::PARENTHESIS),
			Character(u'>', CharacterDirection::PARENTHESIS),

            // Experimental
            u'\u21AA', // return symbol
            u'\u21A4', // backspace symbol
            u'\u2423', // space symbol
            u'\u2191', // shift symbol
            u'\u21BA', // repeat symbol
			u'\u00B0' // degree symbol
        };

		// Additional German letters
        static const std::set<Character> GERMANY_GERMAN
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
		static const std::set<Character> ISRAEL_HEBREW
		{
			Character(u'\u05E9', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D3', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D2', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05DB', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E2', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D9', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D7', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05DC', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05DA', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E3', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D6', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E1', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D1', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D4', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E0', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05DE', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E6', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05EA', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E5', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u20AA', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05F2', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05F1', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05F0', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E7', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E8', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D0', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D8', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05D5', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05DF', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05DD', CharacterDirection::RIGHT_TO_LEFT),
			Character(u'\u05E4', CharacterDirection::RIGHT_TO_LEFT)
		};
    }
}

#endif // CHARACTER_SETS_H_
