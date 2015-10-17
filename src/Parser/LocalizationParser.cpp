//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "LocalizationParser.h"

#include "Defines.h"
#include "Helper.h"
#include "OperationNotifier.h"

#include <string>
#include <fstream>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <vector>

// TODO: Testing
#include <iostream>

namespace eyegui
{
    namespace localization_parser
    {
        std::unique_ptr<localizationMap> parse(std::string filepath)
        {
			// !!! using int16_t here because visual compiler can't take char16_t !!!

            // Check file name
            if (!checkFileNameExtension(filepath, LOCALIZATION_EXTENSION))
            {
                throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
            }

            // Create map
            std::unique_ptr<localizationMap> upMap = std::unique_ptr<localizationMap>(new localizationMap);
		
            // Read file
            std::basic_ifstream<int16_t> in(filepath);

            // Check whether file was found
            if (!in)
            {
                throwError(OperationNotifier::Operation::PARSING, "Localization file not found", filepath);
            }
			
            // Read content (TODO: recognize encoding and to correct import...)
            std::istreambuf_iterator<int16_t> in_begin(in), in_end;
            std::u16string content(in_begin, in_end);

			// Get rid of whitespaces
			std::u16string::iterator end_pos = std::remove(content.begin(), content.end(), u' ');
			content.erase(end_pos, content.end());

			// Get rid of tabs
			end_pos = std::remove(content.begin(), content.end(), u'\t');
			content.erase(end_pos, content.end());

			// Add some new line at the end of content because while loop does not read last line
			content += u"\n";

			// Iterate over lines
			std::u16string delimiter = u"\n";
			size_t pos = 0;
			std::u16string line;

			while ((pos = content.find(delimiter)) != std::u16string::npos)
			{
				line = content.substr(0, pos);
				content.erase(0, pos + delimiter.length());

				parseLine(*(upMap.get()), line, filepath);
			}

			// Return map for localization
			return std::move(upMap);
        }

		void parseLine(localizationMap& rLocalizationMap, std::u16string line, std::string filepath)
		{
			// Get left and right side
			std::u16string delimiter = u"=";
			size_t pos = line.find(delimiter);

			// Convert line to vector of int16_t
			std::vector<int16_t> int16Vector;
			for (int i = 0; i < line.size(); i++)
			{
				int16Vector.push_back((int16_t)line.at(i));
			}

			// Already prepare right side (left in vector)
			std::u16string right = line.erase(0, pos + delimiter.length());

			// Converter for keys and error output
			std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
			
			// Check line
			if (pos == std::u16string::npos)
			{
				throwError(OperationNotifier::Operation::PARSING, "Following line could not be parsed: " + convert.to_bytes(int16Vector.data()), filepath);
			}

			// Fill left side
			std::string left = (std::string)(convert.to_bytes(int16Vector.data()));

			std::cout << left << std::endl;

			// Fill in map
			rLocalizationMap[left] = right;
		}
    }
}
