//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "LayoutParser.h"

#include "StylesheetParser.h"
#include "Layout.h"
#include "OperationNotifier.h"
#include "Helper.h"
#include "PathBuilder.h"

namespace eyegui
{
    namespace layout_parser
    {
        std::unique_ptr<Layout> parse(GUI* pGUI, AssetManager* pAssetManager, std::string filepath)
        {
            // Check file name
            if (!checkFileNameExtension(filepath, LAYOUT_EXTENSION))
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

            // First xml element should be a layout
            tinyxml2::XMLElement* xmlLayout = doc.FirstChildElement();
            if (!element_parser::validateElement(xmlLayout, "layout"))
            {
                throwError(OperationNotifier::Operation::PARSING, "No layout node as root in XML found", filepath);
            }

            // Get path to stylesheet
            std::string stylesheetFilepath = element_parser::parseStringAttribute("stylesheet", xmlLayout);

            // Create layout
            std::unique_ptr<Layout> upLayout = std::unique_ptr<Layout>(new Layout(pGUI, pAssetManager, stylesheetFilepath));

            // Then there should be an element
            tinyxml2::XMLElement* xmlRoot = xmlLayout->FirstChildElement();

            // Check for existence
            if (xmlRoot == NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "Layout has no element", filepath);
            }

            // Create, parse further internal an attach
            std::unique_ptr<elementsAndIds> upPair;
            upPair = std::move(element_parser::parse(upLayout.get(), upLayout->getMainFrame(), pAssetManager, upLayout->getNotificationQueue(), xmlRoot, NULL, filepath));
            upLayout->attachElementToMainFrameAsRoot(std::move(upPair->first), std::move(upPair->second));

            // Return ready to use layout
            return(std::move(upLayout));
        }
    }
}
