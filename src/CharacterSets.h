//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Character sets for generating texture atlases.

#ifndef CHARACTER_SETS_H_
#define CHARACTER_SETS_H_

#include <map>

namespace eyegui
{
	// Text direction defined by character (e.g., Latin is left to right and Hebrew right to left)
	enum class CharacterDirection { LEFT_TO_RIGHT, RIGHT_TO_LEFT, NEUTRAL, PARENTHESIS}; // extra enum for parentheses, as they are rendered mirrored for right to left languages

    namespace charsets
    {
		// Basic Latin characters
        static const std::map<char16_t, CharacterDirection> BASIC
        {
			// Left to right do not need full contructor
            {'0', CharacterDirection::LEFT_TO_RIGHT},
			{'1', CharacterDirection::LEFT_TO_RIGHT},
			{'2', CharacterDirection::LEFT_TO_RIGHT},
			{'3', CharacterDirection::LEFT_TO_RIGHT},
			{'4', CharacterDirection::LEFT_TO_RIGHT},
			{'5', CharacterDirection::LEFT_TO_RIGHT},
			{'6', CharacterDirection::LEFT_TO_RIGHT},
			{'7', CharacterDirection::LEFT_TO_RIGHT},
			{'8', CharacterDirection::LEFT_TO_RIGHT},
			{'9', CharacterDirection::LEFT_TO_RIGHT},
			{'a', CharacterDirection::LEFT_TO_RIGHT},
			{'b', CharacterDirection::LEFT_TO_RIGHT},
			{'c', CharacterDirection::LEFT_TO_RIGHT},
			{'d', CharacterDirection::LEFT_TO_RIGHT},
            {'e', CharacterDirection::LEFT_TO_RIGHT},
			{'f', CharacterDirection::LEFT_TO_RIGHT},
			{'g', CharacterDirection::LEFT_TO_RIGHT},
			{'h', CharacterDirection::LEFT_TO_RIGHT},
			{'i', CharacterDirection::LEFT_TO_RIGHT},
			{'j', CharacterDirection::LEFT_TO_RIGHT},
			{'k', CharacterDirection::LEFT_TO_RIGHT},
			{'l', CharacterDirection::LEFT_TO_RIGHT},
			{'m', CharacterDirection::LEFT_TO_RIGHT},
			{'n', CharacterDirection::LEFT_TO_RIGHT},
			{'o', CharacterDirection::LEFT_TO_RIGHT},
			{'p', CharacterDirection::LEFT_TO_RIGHT},
			{'q', CharacterDirection::LEFT_TO_RIGHT},
			{'r', CharacterDirection::LEFT_TO_RIGHT},
            {'s', CharacterDirection::LEFT_TO_RIGHT},
			{'t', CharacterDirection::LEFT_TO_RIGHT},
			{'u', CharacterDirection::LEFT_TO_RIGHT},
			{'v', CharacterDirection::LEFT_TO_RIGHT},
			{'w', CharacterDirection::LEFT_TO_RIGHT},
			{'x', CharacterDirection::LEFT_TO_RIGHT},
			{'y', CharacterDirection::LEFT_TO_RIGHT},
			{'z', CharacterDirection::LEFT_TO_RIGHT},
			{'A', CharacterDirection::LEFT_TO_RIGHT},
			{'B', CharacterDirection::LEFT_TO_RIGHT},
			{'C', CharacterDirection::LEFT_TO_RIGHT},
			{'D', CharacterDirection::LEFT_TO_RIGHT},
			{'E', CharacterDirection::LEFT_TO_RIGHT},
			{'F', CharacterDirection::LEFT_TO_RIGHT},
            {'G', CharacterDirection::LEFT_TO_RIGHT},
			{'H', CharacterDirection::LEFT_TO_RIGHT},
			{'I', CharacterDirection::LEFT_TO_RIGHT},
			{'J', CharacterDirection::LEFT_TO_RIGHT},
			{'K', CharacterDirection::LEFT_TO_RIGHT},
			{'L', CharacterDirection::LEFT_TO_RIGHT},
			{'M', CharacterDirection::LEFT_TO_RIGHT},
			{'N', CharacterDirection::LEFT_TO_RIGHT},
			{'O', CharacterDirection::LEFT_TO_RIGHT},
			{'P', CharacterDirection::LEFT_TO_RIGHT},
			{'Q', CharacterDirection::LEFT_TO_RIGHT},
			{'R', CharacterDirection::LEFT_TO_RIGHT},
			{'S', CharacterDirection::LEFT_TO_RIGHT},
			{'T', CharacterDirection::LEFT_TO_RIGHT},
            {'U', CharacterDirection::LEFT_TO_RIGHT},
			{'V', CharacterDirection::LEFT_TO_RIGHT},
			{'W', CharacterDirection::LEFT_TO_RIGHT},
			{'X', CharacterDirection::LEFT_TO_RIGHT},
			{'Y', CharacterDirection::LEFT_TO_RIGHT},
			{'Z', CharacterDirection::LEFT_TO_RIGHT},

			// Character neutral to text direction
			{u'-', CharacterDirection::NEUTRAL}, 
			{u'_', CharacterDirection::NEUTRAL},
			{u',', CharacterDirection::NEUTRAL},
			{u';', CharacterDirection::NEUTRAL},
			{u'.', CharacterDirection::NEUTRAL},
			{u':', CharacterDirection::NEUTRAL},
			{u'+', CharacterDirection::NEUTRAL},
			{u'*', CharacterDirection::NEUTRAL},
            {u'~', CharacterDirection::NEUTRAL},
			{u'#', CharacterDirection::NEUTRAL},
			{u'/', CharacterDirection::NEUTRAL},
			{u'?', CharacterDirection::NEUTRAL},
			{u'!', CharacterDirection::NEUTRAL},
			{u'%', CharacterDirection::NEUTRAL},
			{u'#', CharacterDirection::NEUTRAL},
			{u' ', CharacterDirection::NEUTRAL},
			{u'|', CharacterDirection::NEUTRAL},
			{u'$', CharacterDirection::NEUTRAL},
			{u'@', CharacterDirection::NEUTRAL},
			{u'^', CharacterDirection::NEUTRAL},
            {u'"', CharacterDirection::NEUTRAL},
			{u'=', CharacterDirection::NEUTRAL},
			{u'&', CharacterDirection::NEUTRAL},
			{u'\\', CharacterDirection::NEUTRAL},
			{u'/', CharacterDirection::NEUTRAL},
			{u'\u00A7', CharacterDirection::NEUTRAL}, // paragraph

			// Parentheses
			{u'(', CharacterDirection::PARENTHESIS},
			{u')', CharacterDirection::PARENTHESIS},
			{u'[', CharacterDirection::PARENTHESIS},
			{u']', CharacterDirection::PARENTHESIS},
			{u'{', CharacterDirection::PARENTHESIS},
			{u'}', CharacterDirection::PARENTHESIS},
			{u'<', CharacterDirection::PARENTHESIS},
			{u'>', CharacterDirection::PARENTHESIS},

            // Experimental
            {u'\u21AA', CharacterDirection::NEUTRAL}, // return symbol
            {u'\u21A4', CharacterDirection::NEUTRAL}, // backspace symbol
            {u'\u2423', CharacterDirection::NEUTRAL}, // space symbol
            {u'\u2191', CharacterDirection::NEUTRAL}, // shift symbol
            {u'\u21BA', CharacterDirection::NEUTRAL}, // repeat symbol
			{u'\u00B0', CharacterDirection::NEUTRAL } // degree symbol
        };

		// Additional German letters
        static const std::map<char16_t, CharacterDirection> GERMANY_GERMAN
        {
            {u'\u00c4', CharacterDirection::LEFT_TO_RIGHT}, // AE
            {u'\u00e4', CharacterDirection::LEFT_TO_RIGHT}, // ae
            {u'\u00d6', CharacterDirection::LEFT_TO_RIGHT}, // OE
            {u'\u00f6', CharacterDirection::LEFT_TO_RIGHT}, // oe
            {u'\u00dc', CharacterDirection::LEFT_TO_RIGHT}, // UE
            {u'\u00fc', CharacterDirection::LEFT_TO_RIGHT}, // ue
            {u'\u00df', CharacterDirection::LEFT_TO_RIGHT}, // sz
            {u'\u20AC', CharacterDirection::LEFT_TO_RIGHT}  // Euro
        };

		// Modern Hebrew
		static const std::map<char16_t, CharacterDirection> ISRAEL_HEBREW
		{
			{u'\u05E9', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D3', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D2', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05DB', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E2', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D9', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D7', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05DC', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05DA', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E3', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D6', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E1', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D1', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D4', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E0', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05DE', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E6', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05EA', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E5', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u20AA', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05F2', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05F1', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05F0', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E7', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E8', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D0', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D8', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05D5', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05DF', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05DD', CharacterDirection::RIGHT_TO_LEFT},
			{u'\u05E4', CharacterDirection::RIGHT_TO_LEFT}
		};
    }
}

#endif // CHARACTER_SETS_H_
