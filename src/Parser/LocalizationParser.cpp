//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "LocalizationParser.h"

#include "Defines.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/PathBuilder.h"

#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>

namespace eyegui
{
    namespace localization_parser
    {
        std::unique_ptr<LocalizationMap> parse(std::string filepath)
        {
            // Check file name
            if (!checkFileNameExtension(filepath, LOCALIZATION_EXTENSION))
            {
                throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
            }

            // Create map
            std::unique_ptr<LocalizationMap> upMap = std::unique_ptr<LocalizationMap>(new LocalizationMap);

            // Read file
            std::ifstream in(buildPath(filepath));

            // Check whether file was found
            if (!in)
            {
                throwError(OperationNotifier::Operation::PARSING, "Localization file not found", filepath);
            }

            // Convert input file to string
            std::stringstream strStream;
            strStream << in.rdbuf();
            std::string content = strStream.str();

            // Close file
            in.close();

            // Streamline line endings
            streamlineLineEnding(content);

            // Add some new line at the end of content because while loop does not read last line
            content += "\n";

            // Some values for iteration
            uint lineCount = 1;
            std::string delimiter = "\n";
            size_t pos = 0;
            std::string line;

            // Iterate through lines
            while ((pos = content.find(delimiter)) != std::string::npos)
            {
                // Extract line
                line = content.substr(0, pos);
                content.erase(0, pos + delimiter.length());

                if (!line.empty())
                {
                    // Extract key and value from line
                    parseLine(*(upMap.get()), line, lineCount, filepath);
                }

                // Next line
                lineCount++;
            }

            // Return map for localization
            return std::move(upMap);
        }

        void parseLine(LocalizationMap& rLocalizationMap, std::string line, uint lineCount, std::string filepath)
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
            if (!convertUTF8ToUTF16(right, utf16right))
            {
                throwError(OperationNotifier::Operation::PARSING, "Invalid UTF-8 encoding detected at line " + std::to_string(lineCount), filepath);
            }

            // Fill in map
            rLocalizationMap[left] = utf16right;
        }
    }
}
