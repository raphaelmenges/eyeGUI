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

    bool checkFileNameExtension(std::string filepath, std::string expectedExtension);

    void replaceString(std::string& rInput, const std::string &rTarget, const std::string& rReplacement);

    // Convert pixel space to drawing space (origin top left)
    glm::mat4 calculateDrawMatrix(int layoutWidth, int layoutHeight, int x, int y, int width, int height);

    // Returns whether conversion was successful
    bool convertUTF8ToUTF16(const std::string& rInput, std::u16string& rOutput);

    // Returns whether conversion was successful
    bool convertUTF16ToUTF8(const std::u16string& rInput, std::string& rOutput);

    // Returns whether change in case was successful
    bool toLower(std::u16string& rString);

    // Returns whether change in case was successful
    bool toLower(char16_t& rCharacter);

    // Returns whether change in case of first character was successful
    bool firstCharacterToUpper(std::u16string& rString);

    // Streamline LF, CR and CR+LF endings to LF
    void streamlineLineEnding(std::string& rInput, bool addNewLineAtEnd = false);
    void streamlineLineEnding(std::u16string& rInput, bool addNewLineAtEnd = false);
}

#endif // HELPER_H_
