//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "LocalizationParser.h"

#include "Defines.h"
#include "Helper.h"
#include "OperationNotifier.h"
#include "PathBuilder.h"
#include "externals/utfcpp/source/utf8.h"

#include <string>
#include <fstream>
#include <algorithm>

namespace eyegui
{
    namespace localization_parser
    {
        std::unique_ptr<localizationMap> parse(std::string filepath)
        {
            // Check file name
            if (!checkFileNameExtension(filepath, LOCALIZATION_EXTENSION))
            {
                throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
            }

            // Create map
            std::unique_ptr<localizationMap> upMap = std::unique_ptr<localizationMap>(new localizationMap);

            // Read file
            std::ifstream in(buildPath(filepath));

            // Check whether file was found
            if (!in)
            {
                throwError(OperationNotifier::Operation::PARSING, "Localization file not found", filepath);
            }

            // Some values for iteration
            unsigned int line_count = 1;
            std::string line;

            // Iterate through lines
            while (getline(in, line))
            {
                // Check for invalid utf-8
                std::string::iterator end_pos = utf8::find_invalid(line.begin(), line.end());
                if (end_pos != line.end()) {
                    throwError(OperationNotifier::Operation::PARSING, "Invalid UTF-8 encoding detected at line " + line_count, filepath);
                }

                // Extract key and value from line
                parseLine(*(upMap.get()), line, filepath);

                // Next line
                line_count++;
            }

            // Return map for localization
            return std::move(upMap);
        }

        void parseLine(localizationMap& rLocalizationMap, std::string line, std::string filepath)
        {
            // Split left and right side
            std::string delimiter = "=";
            size_t pos = 0;

            if ((pos = line.find(delimiter)) == std::string::npos)
            {
                throwError(OperationNotifier::Operation::PARSING, "Following line could not be parsed: " + line, filepath);
                return;
            }

            std::string left = line.substr(0, pos);

            // Get rid of whitespaces on left side
            std::string::iterator end_pos = std::remove(left.begin(), left.end(), ' ');
            left.erase(end_pos, left.end());

            // Get rid of tabs on left side
            end_pos = std::remove(left.begin(), left.end(), '\t');
            left.erase(end_pos, left.end());

            // Extract right side
            line.erase(0, pos + delimiter.length());
            std::string right = line;

            // Convert right side to utf-16
            std::u16string utf16right;
            utf8::utf8to16(right.begin(), right.end(), back_inserter(utf16right));

            // Fill in map
            rLocalizationMap[left] = utf16right;
        }
    }
}
