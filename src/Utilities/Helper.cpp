//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Helper.h"

#include "externals\utf8rewind\include\utf8rewind\utf8rewind.h"

namespace eyegui
{
    float clamp(float value, float lowerBound, float upperBound)
    {
        return value < lowerBound ? lowerBound : (value > upperBound ? upperBound : value);
    }

    bool checkFileNameExtension(std::string filepath, std::string expectedExtension)
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

    void replaceString(std::string& rInput, const std::string &rTarget, const std::string& rReplacement)
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

    glm::mat4 calculateDrawMatrix(int layoutWidth, int layoutHeight, int x, int y, int width, int height)
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

	bool convertUTF8ToUTF16(const std::string& rInput, std::u16string& rOutput)
	{
		// Variables which will be filled
		int32_t errors;
		size_t size;

		// First, determine needed size in new string
		size = utf8toutf16(
			rInput.c_str(), rInput.size(),
			NULL, 0,
			&errors);

		// Only proceed when no error occured
		if (errors == UTF8_ERR_NONE)
		{
			// Use determined size to reserve space
			uint16_t* space = (uint16_t*)malloc(size);

			// Convert from UTF-8 to UTF-16
			utf8toutf16(
				rInput.c_str(), rInput.size(),
				space, size,
				&errors);

			// Copy to referenced output string
			rOutput = std::u16string((char16_t*)space, (size / sizeof(char16_t)));

			// Free space of malloc
			free(space);
		}

		// Return error check
		return (errors == UTF8_ERR_NONE);
	}
}
