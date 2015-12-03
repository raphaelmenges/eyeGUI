//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef STYLE_H_
#define STYLE_H_

#include "externals/GLM/glm/glm.hpp"
#include <string>

namespace eyegui
{
    struct Style
    {
        // Initialize with fallback values
        std::string filepath;
        glm::vec4 color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        glm::vec4 backgroundColor = glm::vec4(0, 0, 0, 0);
        glm::vec4 highlightColor = glm::vec4(1, 1, 0, 0.5f);
        glm::vec4 separatorColor = glm::vec4(0.2f, 0.2f, 0.2f, 1);
        glm::vec4 selectionColor = glm::vec4(0, 1, 1, 0.5f);
        glm::vec4 iconColor = glm::vec4(1, 1, 1, 1);
        glm::vec4 fontColor = glm::vec4(1, 1, 1, 1);
        glm::vec4 dimColor = glm::vec4(0.5f, 0.5f, 0.5f, 0.2f);
    };
}

#endif // STYLE_H_
