//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Static helper functions.

#ifndef HELPER_H_
#define HELPER_H_

#include "externals/GLM/glm/glm.hpp"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

#include <string>
#include <algorithm>

namespace eyegui
{
    float clamp(float value, float lowerBound, float upperBound);
	int clamp(int value, int lowerBound, int upperBound);

    bool checkFileNameExtension(std::string filepath, std::string expectedExtension);

    // Extracts file name without extension
    std::string extractFileName(std::string filepath);

    void replaceString(std::string& rInput, const std::string &rTarget, const std::string& rReplacement);

    // Convert pixel space to drawing space (origin top left)
    glm::mat4 calculateDrawMatrix(int layoutWidth, int layoutHeight, int x, int y, int width, int height);

    // Returns whether conversion was successful
    bool convertUTF8ToUTF16(const std::string& rInput, std::u16string& rOutput);

    // Returns whether conversion was successful
    bool convertUTF16ToUTF8(const std::u16string& rInput, std::string& rOutput);

    // Returns UTF8 string. Is empty if fails
    std::string convertUTF16ToUTF8(const std::u16string& rInput);

    // Returns UTF16 string. Is empty if fails
    std::u16string convertUTF8ToUTF16(const std::string& rInput);

    // Returns whether change in case was successful
    bool toLower(std::u16string& rString);

    // Returns whether change in case was successful
    bool toLower(char16_t& rCharacter);

    // Returns whether change in case was successful
    bool toUpper(std::u16string& rString);

    // Returns whether change in case was successful
    bool toUpper(char16_t& rCharacter);

    // Returns whether change in case of first character was successful
    bool firstCharacterToUpper(std::u16string& rString);

    // Streamline LF, CR and CR+LF endings to LF
    void streamlineLineEnding(std::string& rInput, bool addNewLineAtEnd = false);
    void streamlineLineEnding(std::u16string& rInput, bool addNewLineAtEnd = false);

    // String to float converter. Returns -1 at failure
    float stringToFloat(std::string value);

	// String hex RGBA to vec4 RGBA (TODO: maybe return bool at failure or so)
	glm::vec4 stringHexRGBAToVec4RGBA(std::string value);

    // Check whether coordinate within rectangle
    bool insideRect(int rectX, int rectY, int width, int height, int x, int y);
}

#endif // HELPER_H_
