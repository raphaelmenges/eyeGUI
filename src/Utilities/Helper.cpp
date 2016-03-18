//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Helper.h"

#include "externals/utf8rewind/include/utf8rewind/utf8rewind.h"

namespace eyegui
{
    // ### HELPER HELPERS ###

    bool changeCase(char16_t& rCharacter, size_t(*caseFunction) (const char* input, size_t inputSize, char* target, size_t targetSize, int32_t* errors))
    {
        // Variables which will be filled
        int32_t errors;

        // *** UTF16 -> UTF8 ***

        // First, determine needed size for chars
        size_t originalSize = utf16toutf8(
            (utf16_t const *)(&rCharacter), sizeof(char16_t),
            NULL, 0,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            return false;
        }

        // Reserve space for original chars
        char* originalSpace = (char*)malloc(originalSize);

        // Convert from UTF-16 to UTF-8
        utf16toutf8(
            (utf16_t const *)(&rCharacter), sizeof(char16_t),
            originalSpace, originalSize,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            free(originalSpace);
            return false;
        }

        // *** LOWER CASE ***

        // Determine necessary space for new chars
        size_t newSize = caseFunction(
            originalSpace, originalSize,
            NULL, 0,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            free(originalSpace);
            return false;
        }

        // Reserve space for chars
        char* newSpace = (char*)malloc(newSize);

        // Convert to other case
        newSize = caseFunction(
            originalSpace, originalSize,
            newSpace, newSize,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            free(originalSpace);
            free(newSpace);
            return false;
        }

        // *** UTF8 -> UTF16 ***

        // Determine space necessary for conversion to UTF-16
        size_t newSize16 = utf8toutf16(
            newSpace, newSize,
            NULL, 0,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            free(originalSpace);
            free(newSpace);
            return false;
        }

        uint16_t* newSpace16 = (uint16_t*)malloc(newSize16);

        // Convert from UTF-8 to UTF-16
        utf8toutf16(
            newSpace, newSize,
            newSpace16, newSize16,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            free(originalSpace);
            free(newSpace);
            free(newSpace16);
            return false;
        }

        // Save in given char16_t
        rCharacter = *(char16_t*)newSpace16;

        // Free malloc
        free(originalSpace);
        free(newSpace);
        free(newSpace16);

        return true;
    }


    // ### IMPLEMENTATION ###

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

    bool convertUTF16ToUTF8(const std::u16string& rInput, std::string& rOutput)
    {
        // Variables which will be filled
        int32_t errors;
        size_t size;

        // u16string's size returns count of letters, not byte size...
        const size_t inputSize = rInput.size() * sizeof(char16_t);

        // First, determine needed size in new string
        size = utf16toutf8(
            (utf16_t const *)(rInput.c_str()), inputSize,
            NULL, 0,
            &errors);

        // Only proceed when no error occured
        if (errors == UTF8_ERR_NONE)
        {
            // Use determined size to reserve space
            char* space = (char*)malloc(size);

            // Convert from UTF-16 to UTF-8
            utf16toutf8(
                (utf16_t const *)(rInput.c_str()), inputSize,
                space, size,
                &errors);

            // Copy to referenced output string
            rOutput = std::string(space, (size / sizeof(char)));

            // Free space of malloc
            free(space);
        }

        // Return error check
        return (errors == UTF8_ERR_NONE);
    }

    bool toLower(std::u16string& rString)
    {
        // Variables which will be filled
        int32_t errors;

        // *** UTF16 -> UTF8 ***

        // Change to UTF-8
        std::string string8;
        bool check = convertUTF16ToUTF8(rString, string8);

        // Check for errors
        if (!check)
        {
            return false;
        }

        // *** LOWER CASE ***

        // Determine necessary space for lower chars
        size_t size = utf8tolower(
            string8.c_str(), string8.size(),
            NULL, 0,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            return false;
        }

        // Reserve space for chars
        char* space = (char*)malloc(size);

        // Convert to low
        size_t newSize = utf8tolower(
            string8.c_str(), string8.size(),
            space, size,
            &errors);

        // Check for errors
        if (errors != UTF8_ERR_NONE)
        {
            free(space);
            return false;
        }

        // *** UTF8 -> UTF16 ***
        check = convertUTF8ToUTF16(std::string(space, (size / sizeof(char))), rString);

        return check;
    }

    bool toLower(char16_t& rCharacter)
    {
        return changeCase(rCharacter, utf8tolower);
    }

    bool firstCharacterToUpper(std::u16string& rString)
    {
        // Check whether there is a first character
        if (rString.size() > 0)
        {
            return changeCase(rString[0], utf8toupper);
        }

        // Return true, since nothing went wrong
        return true;
    }

    void streamlineLineEnding(std::string& rInput, bool addNewLineAtEnd)
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

        // Add new lline at end
        if(addNewLineAtEnd)
        {
            rInput += "\n";
        }
    }

    void streamlineLineEnding(std::u16string& rInput, bool addNewLineAtEnd)
    {
        // CR+LF -> LF
        std::string::size_type pos = 0;
        while (( pos = rInput.find (u"\r\n",pos) ) != std::string::npos)
        {
            // Only remove \r
            rInput.erase(pos, 1);
        }

        // CR -> LF
        std::replace(rInput.begin(), rInput.end(), u'\r', u'\n');

                // Add new lline at end
        if(addNewLineAtEnd)
        {
            rInput += u"\n";
        }
    }
}
