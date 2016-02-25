//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Some helper functions for parser.

#ifndef PARSER_HELPER_H_
#define PARSER_HELPER_H_

#include "externals/GLM/glm/glm.hpp"

#include <sstream>

namespace eyegui
{
    // Parse color
    static glm::vec4 parseColor(std::string value)
    {
        uint hexInt;
        std::stringstream ss;
        ss << std::hex << value;
        ss >> hexInt;

        float red = ((float)((hexInt & 0xFF000000) >> 24)) / (255.0f);
        float green = ((float)((hexInt & 0x00FF0000) >> 16)) / (255.0f);
        float blue = ((float)((hexInt & 0x0000FF00) >> 8)) / (255.0f);
        float alpha = ((float)((hexInt & 0x000000FF) >> 0)) / (255.0f);

        return glm::vec4(red, green, blue, alpha);
    }

    // Safe string (streamline LF, CR and CR+LF endings)
    static void streamlineLineEnding(std::string& rInput)
    {
        // CR+LF -> LF
        std::string::size_type pos = 0;
        while (( pos = rInput.find ("\r\n",pos) ) != std::string::npos)
        {
            // Only remove \r
            rInput.erase(pos, 1);
        }

        // CR -> LF
        std::replace(rInput.begin(), rInput.end(), '\r', '\n');
    }
}

#endif // PARSER_HELPER_H_
