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
    static float clamp(float value, float lowerBound, float upperBound)
    {
        return value < lowerBound ? lowerBound : (value > upperBound ? upperBound : value);
    }

    static bool checkFileNameExtension(std::string filepath, std::string expectedExtension)
    {
        // Extract token behind last dot
        std::string delimiter = ".";
        size_t pos = 0;
        std::string token;
        while ((pos = filepath.find(delimiter)) != std::string::npos)
        {
            token = filepath.substr(0, pos);
            filepath.erase(0, pos + delimiter.length());
        }

        // File format case is not relevant
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);

        // Check token
        return (filepath.compare(expectedExtension) == 0);
    }

    static void replaceString(std::string& rInput, const std::string &rTarget, const std::string& rReplacement)
    {
        // Check whether there is a target
        if(rTarget.empty())
        {
            return;
        }

        // Find target and replace it
        size_t startPos = 0;
        while((startPos = rInput.find(rTarget, startPos)) != std::string::npos)
        {
            rInput.replace(startPos, rTarget.length(), rReplacement);
            startPos += rReplacement.length();
        }
    }

    // Convert pixel space to drawing space (origin top left)
    static glm::mat4 calculateDrawMatrix(int layoutWidth, int layoutHeight, int x, int y, int width, int height)
    {
        // Get values from layout
        float floatLayoutWidth = (float)(layoutWidth);
        float floatLayoutHeight = (float)(layoutHeight);

        // Create identity
        glm::mat4 matrix = glm::mat4(1.0f);

        // Width and height from zero to one
        float glWidth = width / floatLayoutWidth;
        float glHeight = height / floatLayoutHeight;

        // Translation
        matrix = glm::translate(
            matrix,
            glm::vec3(x / floatLayoutWidth,
                (1.0f - (y / floatLayoutHeight)) - glHeight,
                0));

        // Scaling
        matrix = glm::scale(matrix, glm::vec3(glWidth, glHeight, 1));

        // Projection
        matrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f) * matrix;

        return matrix;
    }
}

#endif // HELPER_H_
