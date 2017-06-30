//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Character sets for generating texture atlases and indication of direction.

#ifndef CHARACTER_SETS_H_
#define CHARACTER_SETS_H_

#include <map>

namespace eyegui
{
	// Text direction defined by character (e.g., Latin is left to right and Hebrew right to left)
	enum class CharacterDirection { LeftToRight, RightToLeft, Neutral, Parenthesis}; // extra enum for parentheses, as they are rendered mirrored for right to left languages

    namespace charsets
    {
		// Basic Latin characters
        static const std::map<char16_t, CharacterDirection> BASIC
        {
			// Left to right do not need full contructor
            {'0', CharacterDirection::LeftToRight},
			{'1', CharacterDirection::LeftToRight},
			{'2', CharacterDirection::LeftToRight},
			{'3', CharacterDirection::LeftToRight},
			{'4', CharacterDirection::LeftToRight},
			{'5', CharacterDirection::LeftToRight},
			{'6', CharacterDirection::LeftToRight},
			{'7', CharacterDirection::LeftToRight},
			{'8', CharacterDirection::LeftToRight},
			{'9', CharacterDirection::LeftToRight},
			{'a', CharacterDirection::LeftToRight},
			{'b', CharacterDirection::LeftToRight},
			{'c', CharacterDirection::LeftToRight},
			{'d', CharacterDirection::LeftToRight},
            {'e', CharacterDirection::LeftToRight},
			{'f', CharacterDirection::LeftToRight},
			{'g', CharacterDirection::LeftToRight},
			{'h', CharacterDirection::LeftToRight},
			{'i', CharacterDirection::LeftToRight},
			{'j', CharacterDirection::LeftToRight},
			{'k', CharacterDirection::LeftToRight},
			{'l', CharacterDirection::LeftToRight},
			{'m', CharacterDirection::LeftToRight},
			{'n', CharacterDirection::LeftToRight},
			{'o', CharacterDirection::LeftToRight},
			{'p', CharacterDirection::LeftToRight},
			{'q', CharacterDirection::LeftToRight},
			{'r', CharacterDirection::LeftToRight},
            {'s', CharacterDirection::LeftToRight},
			{'t', CharacterDirection::LeftToRight},
			{'u', CharacterDirection::LeftToRight},
			{'v', CharacterDirection::LeftToRight},
			{'w', CharacterDirection::LeftToRight},
			{'x', CharacterDirection::LeftToRight},
			{'y', CharacterDirection::LeftToRight},
			{'z', CharacterDirection::LeftToRight},
			{'A', CharacterDirection::LeftToRight},
			{'B', CharacterDirection::LeftToRight},
			{'C', CharacterDirection::LeftToRight},
			{'D', CharacterDirection::LeftToRight},
			{'E', CharacterDirection::LeftToRight},
			{'F', CharacterDirection::LeftToRight},
            {'G', CharacterDirection::LeftToRight},
			{'H', CharacterDirection::LeftToRight},
			{'I', CharacterDirection::LeftToRight},
			{'J', CharacterDirection::LeftToRight},
			{'K', CharacterDirection::LeftToRight},
			{'L', CharacterDirection::LeftToRight},
			{'M', CharacterDirection::LeftToRight},
			{'N', CharacterDirection::LeftToRight},
			{'O', CharacterDirection::LeftToRight},
			{'P', CharacterDirection::LeftToRight},
			{'Q', CharacterDirection::LeftToRight},
			{'R', CharacterDirection::LeftToRight},
			{'S', CharacterDirection::LeftToRight},
			{'T', CharacterDirection::LeftToRight},
            {'U', CharacterDirection::LeftToRight},
			{'V', CharacterDirection::LeftToRight},
			{'W', CharacterDirection::LeftToRight},
			{'X', CharacterDirection::LeftToRight},
			{'Y', CharacterDirection::LeftToRight},
			{'Z', CharacterDirection::LeftToRight},

			// Character Neutral to text direction
			{u'-', CharacterDirection::Neutral}, 
			{u'_', CharacterDirection::Neutral},
			{u',', CharacterDirection::Neutral},
			{u';', CharacterDirection::Neutral},
			{u'.', CharacterDirection::Neutral},
			{u':', CharacterDirection::Neutral},
			{u'+', CharacterDirection::Neutral},
			{u'*', CharacterDirection::Neutral},
            {u'~', CharacterDirection::Neutral},
			{u'#', CharacterDirection::Neutral},
			{u'/', CharacterDirection::Neutral},
			{u'?', CharacterDirection::Neutral}, // for Latin / Hebrew, this is ok. Arabic needs it mirrored
			{u'!', CharacterDirection::Neutral},
			{u'%', CharacterDirection::Neutral},
			{u'#', CharacterDirection::Neutral},
			{u' ', CharacterDirection::Neutral},
			{u'|', CharacterDirection::Neutral},
			{u'$', CharacterDirection::Neutral},
			{u'@', CharacterDirection::Neutral},
			{u'^', CharacterDirection::Neutral},
            {u'"', CharacterDirection::Neutral},
			{u'=', CharacterDirection::Neutral},
			{u'&', CharacterDirection::Neutral},
			{u'\\', CharacterDirection::Neutral},
			{u'/', CharacterDirection::Neutral},
			{u'\u00A7', CharacterDirection::Neutral}, // paragraph
			{u'\'', CharacterDirection::Neutral },
			{u'\u05F3', CharacterDirection::Neutral },

			// Parentheses
			{u'(', CharacterDirection::Parenthesis},
			{u')', CharacterDirection::Parenthesis},
			{u'[', CharacterDirection::Parenthesis},
			{u']', CharacterDirection::Parenthesis},
			{u'{', CharacterDirection::Parenthesis},
			{u'}', CharacterDirection::Parenthesis},
			{u'<', CharacterDirection::Parenthesis},
			{u'>', CharacterDirection::Parenthesis},

            // Experimental
            {u'\u21AA', CharacterDirection::Neutral}, // return symbol
            {u'\u21A4', CharacterDirection::Neutral}, // backspace symbol
            {u'\u2423', CharacterDirection::Neutral}, // space symbol
            {u'\u2191', CharacterDirection::Neutral}, // shift symbol
            {u'\u21BA', CharacterDirection::Neutral}, // repeat symbol
			{u'\u00B0', CharacterDirection::Neutral}  // degree symbol
        };

		// Additional German letters
        static const std::map<char16_t, CharacterDirection> GERMANY_GERMAN
        {
            {u'\u00c4', CharacterDirection::LeftToRight}, // AE
            {u'\u00e4', CharacterDirection::LeftToRight}, // ae
            {u'\u00d6', CharacterDirection::LeftToRight}, // OE
            {u'\u00f6', CharacterDirection::LeftToRight}, // oe
            {u'\u00dc', CharacterDirection::LeftToRight}, // UE
            {u'\u00fc', CharacterDirection::LeftToRight}, // ue
            {u'\u00df', CharacterDirection::LeftToRight}, // sz
            {u'\u20AC', CharacterDirection::LeftToRight}  // Euro
        };

		// Modern Hebrew
		static const std::map<char16_t, CharacterDirection> ISRAEL_HEBREW
		{
			{u'\u05E9', CharacterDirection::RightToLeft},
			{u'\u05D3', CharacterDirection::RightToLeft},
			{u'\u05D2', CharacterDirection::RightToLeft},
			{u'\u05DB', CharacterDirection::RightToLeft},
			{u'\u05E2', CharacterDirection::RightToLeft},
			{u'\u05D9', CharacterDirection::RightToLeft},
			{u'\u05D7', CharacterDirection::RightToLeft},
			{u'\u05DC', CharacterDirection::RightToLeft},
			{u'\u05DA', CharacterDirection::RightToLeft},
			{u'\u05E3', CharacterDirection::RightToLeft},
			{u'\u05D6', CharacterDirection::RightToLeft},
			{u'\u05E1', CharacterDirection::RightToLeft},
			{u'\u05D1', CharacterDirection::RightToLeft},
			{u'\u05D4', CharacterDirection::RightToLeft},
			{u'\u05E0', CharacterDirection::RightToLeft},
			{u'\u05DE', CharacterDirection::RightToLeft},
			{u'\u05E6', CharacterDirection::RightToLeft},
			{u'\u05EA', CharacterDirection::RightToLeft},
			{u'\u05E5', CharacterDirection::RightToLeft},
			{u'\u20AA', CharacterDirection::RightToLeft},
			{u'\u05F2', CharacterDirection::RightToLeft},
			{u'\u05F1', CharacterDirection::RightToLeft},
			{u'\u05F0', CharacterDirection::RightToLeft},
			{u'\u05E7', CharacterDirection::RightToLeft},
			{u'\u05E8', CharacterDirection::RightToLeft},
			{u'\u05D0', CharacterDirection::RightToLeft},
			{u'\u05D8', CharacterDirection::RightToLeft},
			{u'\u05D5', CharacterDirection::RightToLeft},
			{u'\u05DF', CharacterDirection::RightToLeft},
			{u'\u05DD', CharacterDirection::RightToLeft},
			{u'\u05E4', CharacterDirection::RightToLeft}
		};

		// Modern Greek
		static const std::map<char16_t, CharacterDirection> GREECE_GREEK
		{
			// Lowercase letters
			{u'\u03B8', CharacterDirection::LeftToRight},
			{u'\u03C9', CharacterDirection::LeftToRight},
			{u'\u03B5', CharacterDirection::LeftToRight},
			{u'\u03C1', CharacterDirection::LeftToRight},
			{u'\u03C4', CharacterDirection::LeftToRight},
			{u'\u03C8', CharacterDirection::LeftToRight},
			{u'\u03C5', CharacterDirection::LeftToRight},
			{u'\u03B9', CharacterDirection::LeftToRight},
			{u'\u03BF', CharacterDirection::LeftToRight},
			{u'\u03C0', CharacterDirection::LeftToRight}, // end of first row
			{u'\u03B1', CharacterDirection::LeftToRight},
			{u'\u03C3', CharacterDirection::LeftToRight},
			{u'\u03B4', CharacterDirection::LeftToRight},
			{u'\u03C6', CharacterDirection::LeftToRight},
			{u'\u03B3', CharacterDirection::LeftToRight},
			{u'\u03B7', CharacterDirection::LeftToRight},
			{u'\u03C2', CharacterDirection::LeftToRight},
			{u'\u03BA', CharacterDirection::LeftToRight},
			{u'\u03BB', CharacterDirection::LeftToRight}, // end of second row
			{u'\u03B6', CharacterDirection::LeftToRight},
			{u'\u03C7', CharacterDirection::LeftToRight},
			{u'\u03BE', CharacterDirection::LeftToRight},
			{u'\u03C9', CharacterDirection::LeftToRight},
			{u'\u03B2', CharacterDirection::LeftToRight},
			{u'\u03BD', CharacterDirection::LeftToRight},
			{u'\u03BC', CharacterDirection::LeftToRight}, // end of third row
			// Uppercase letters
			{u'\u0398', CharacterDirection::LeftToRight},
			{u'\u03A9', CharacterDirection::LeftToRight},
			{u'\u0395', CharacterDirection::LeftToRight},
			{u'\u03A1', CharacterDirection::LeftToRight},
			{u'\u03A4', CharacterDirection::LeftToRight},
			{u'\u03A8', CharacterDirection::LeftToRight},
			{u'\u03A5', CharacterDirection::LeftToRight},
			{u'\u0399', CharacterDirection::LeftToRight},
			{u'\u039F', CharacterDirection::LeftToRight},
			{u'\u03A0', CharacterDirection::LeftToRight}, // end of first row
			{u'\u0391', CharacterDirection::LeftToRight},
			{u'\u03A3', CharacterDirection::LeftToRight},
			{u'\u0394', CharacterDirection::LeftToRight},
			{u'\u03A6', CharacterDirection::LeftToRight},
			{u'\u0393', CharacterDirection::LeftToRight},
			{u'\u0397', CharacterDirection::LeftToRight},
			{u'\u03C2', CharacterDirection::LeftToRight},
			{u'\u039A', CharacterDirection::LeftToRight},
			{u'\u039B', CharacterDirection::LeftToRight}, // end of second row
			{u'\u0396', CharacterDirection::LeftToRight},
			{u'\u03A7', CharacterDirection::LeftToRight},
			{u'\u039E', CharacterDirection::LeftToRight},
			{u'\u03A9', CharacterDirection::LeftToRight},
			{u'\u0392', CharacterDirection::LeftToRight},
			{u'\u039D', CharacterDirection::LeftToRight},
			{u'\u039C', CharacterDirection::LeftToRight}, // end of third row
			// Extra letters in lowercase
			{u'\u03AC', CharacterDirection::LeftToRight},
			{u'\u03AD', CharacterDirection::LeftToRight},
			{u'\u03AE', CharacterDirection::LeftToRight},
			{u'\u03AF', CharacterDirection::LeftToRight},
			{u'\u03CA', CharacterDirection::LeftToRight},
			{u'\u0390', CharacterDirection::LeftToRight},
			{u'\u03CC', CharacterDirection::LeftToRight},
			{u'\u03CD', CharacterDirection::LeftToRight},
			{u'\u03CB', CharacterDirection::LeftToRight},
			{u'\u03B0', CharacterDirection::LeftToRight},
			{u'\u03CE', CharacterDirection::LeftToRight},
			// Extra letters in uppercase
			{u'\u0386', CharacterDirection::LeftToRight},
			{u'\u0388', CharacterDirection::LeftToRight},
			{u'\u0389', CharacterDirection::LeftToRight},
			{u'\u038A', CharacterDirection::LeftToRight},
			{u'\u03AA', CharacterDirection::LeftToRight},
			{u'\u0390', CharacterDirection::LeftToRight},
			{u'\u038C', CharacterDirection::LeftToRight},
			{u'\u038E', CharacterDirection::LeftToRight},
			{u'\u03AB', CharacterDirection::LeftToRight},
			{u'\u03B0', CharacterDirection::LeftToRight},
			{u'\u038F', CharacterDirection::LeftToRight}
		};
    }
}

#endif // CHARACTER_SETS_H_
