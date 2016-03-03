//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "BrickParser.h"

#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/PathBuilder.h"

namespace eyegui
{
    namespace brick_parser
    {
        std::unique_ptr<elementsAndIds> parse(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, Element* pParent, std::string filepath, std::map<std::string, std::string> idMapper)
        {
            // Check file name
            if (!checkFileNameExtension(filepath, BRICK_EXTENSION))
            {
                throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
            }

            // Read file
            tinyxml2::XMLDocument doc;
            doc.LoadFile(buildPath(filepath).c_str());
            if (doc.Error())
            {
                throwError(OperationNotifier::Operation::PARSING, std::string("XML could not be parsed: ") + doc.ErrorName(), filepath);
            }

            // Get first xml element
            tinyxml2::XMLElement* xmlElement = doc.FirstChildElement();

            // Collect values to return
            std::unique_ptr<elementsAndIds> upPair = std::unique_ptr<elementsAndIds>(new elementsAndIds);
            upPair = std::move(element_parser::parse(pLayout, pFrame, pAssetManager, pNotificationQueue, xmlElement, pParent, filepath, idMapper));

            // Return the pair
            return std::move(upPair);
        }
    }
}
