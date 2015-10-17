//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "LocalizationParser.h"

#include "Defines.h"
#include "Helper.h"
#include "OperationNotifier.h"

#include <fstream>
#include <locale>
#include <codecvt>

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

            // Read filestd::basic_ifstream<char16_t> ifs(path);
            std::basic_ifstream<char16_t> in(filepath);

            // Check whether file was found
            if (!in)
            {
                throwError(OperationNotifier::Operation::PARSING, "Localization file not found", filepath);
            }

            // Read content
            std::istreambuf_iterator<char16_t> in_begin(in), in_end;
            std::u16string content(in_begin, in_end);

            // Go through lines and add entries to map
            // TODO...

			return upMap;
        }
    }
}
