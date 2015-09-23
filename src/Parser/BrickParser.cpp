//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "BrickParser.h"

#include "OperationNotifier.h"
#include "Helper.h"

namespace eyegui
{
    BrickParser::BrickParser()
    {
        // Nothing to do
    }

    BrickParser::~BrickParser()
    {
        // Nothing to do
    }

    std::unique_ptr<elementsAndIds> BrickParser::parse(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, Element* pParent, std::string filepath)
    {
        // Check file name
        if (!checkFileNameExtension(filepath, BRICK_EXTENSION))
        {
            throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
        }

        // Read file
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filepath.c_str());
        if (doc.Error())
        {
            throwError(OperationNotifier::Operation::PARSING, std::string("XML could not be parsed: ") + doc.ErrorName(), filepath);
        }

        // Get first xml element
        tinyxml2::XMLElement* xmlElement = doc.FirstChildElement();

        // Collect values to return
        std::unique_ptr<elementsAndIds> upPair = std::unique_ptr<elementsAndIds>(new elementsAndIds);
        upPair = std::move(mElementParser.parse(pLayout, pFrame, pAssetManager, xmlElement, pParent, filepath));

        // Return the pair
        return std::move(upPair);
    }
}
