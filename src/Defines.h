//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Stuff used all around the library.

#ifndef DEFINES_H_
#define DEFINES_H_

#include "include/eyeGUI.h"
#include "externals/GLM/glm/glm.hpp"

#include <string>

namespace eyegui
{
    // Typedefs
    typedef unsigned int uint;
    typedef unsigned long ulong;
    typedef unsigned char uchar;

    // Constants
    static const std::string EMPTY_STRING_ATTRIBUTE = "";
    static const int EMPTY_INT_ATTRIBUTE = 0;
    static const float EMPTY_PERCENTAGE_ATTRIBUTE = 0;
    static const bool EMPTY_BOOL_ATTRIBUTE = false;
    static const std::string DEFAULT_STYLE_NAME = "default";
    static const std::string LAYOUT_EXTENSION = "xeyegui";
    static const std::string BRICK_EXTENSION = "beyegui";
    static const std::string CONFIG_EXTENSION = "ceyegui";
    static const std::string STYLESHEET_EXTENSION = "seyegui";
    static const std::string LOCALIZATION_EXTENSION = "leyegui";
    static const int NOTIFICATION_MAX_LOOP_COUNT = 10;
    static const float ACCUMULATED_TIME_PERIOD = 1800.0f;
    static const float FONT_CHARACTER_PADDING = 0.07f; // percentage of height
    static const int FONT_MINIMAL_CHARACTER_PADDING = 2;
    static const float RESIZE_WAIT_DURATION = 0.3f;
    static const glm::vec4 RESIZE_BLEND_COLOR = glm::vec4(0.75f, 0.75f, 0.75f, 0.75f);
    static const std::u16string LOCALIZATION_NOT_FOUND = u"";
    static const float KEY_CIRCLE_CHARACTER_SIZE_RATIO = 0.6f;
    static const float KEYBOARD_HORIZONTAL_KEY_DISTANCE = 0.15f;
    static const ImageAlignment STANDARD_IMAGE_ALIGNMENT = ImageAlignment::ZOOMED;
    static const std::u16string TEXT_FLOW_OVERFLOW_MARK = u"...";
    static const float KEY_FOCUS_DURATION = 0.3f;
    static const float KEY_SELECT_DURATION = 0.1f;
    static const float KEY_MIN_SCALE = 0.5f;
    static const char16_t FONT_FALLBACK_CHARACTER = u' ';
    static const float FONT_KEYBOARD_SIZE = 0.1f; // percentage of screen height
    static const uint DICTIONARY_MAX_FOLLOWING_WORDS = 2; // maximal count of words for each fully collected one serving as prefix
    static const uint DICTIONARY_INPUT_REPEAT_IGNORE_DEPTH = 1; // "Aaal" -> "Aal"
    static const uint DICTIONARY_INPUT_REPEAT_PAUSE_DEPTH = 3; // "Al" -> "Aal". Important for fast typing because no repeating letters in input
    static const uint DICTIONARY_INPUT_PAUSE_DEPTH = 0; // "Hus" -> "Haus"
    static const uint DICTIONARY_INPUT_IGNORE_DEPTH = 1; // "Huus" -> "Haus"
    static const uint WORD_SUGGEST_MAX_SUGGESTIONS = 10;
    static const float WORD_SUGGEST_SCROLL_SPEED = 1.5f;
    static const float WORD_SUGGEST_THRESHOLD_SPEED = 0.75f;
    static const float INTERACTION_FADING_DURATION = 0.5f; // used by pressed keys in keyboard and chosen suggestions in word suggest
    static const float WORD_SUGGEST_CHOSEN_ANIMATION_OFFSET = 0.03f; // in relation to height of layout
    static const float WORD_SUGGEST_SUGGESTION_DISTANCE = 4; // in size of letter which represents space
	static const float TEXT_BACKGROUND_SIZE = 1.1f;
	static const float GRID_FILL_EPSILON = 0.01f;  // since floating point precision does not sum everything to 100%
}

#endif // DEFINES_H_
