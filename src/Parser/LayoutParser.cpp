//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "LayoutParser.h"

#include "src/Layout.h"
#include "ElementParser.h"
#include "src/Parser/ParserHelpers.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/PathBuilder.h"

namespace eyegui
{
    namespace layout_parser
    {
        std::unique_ptr<Layout> parse(GUI const * pGUI, AssetManager* pAssetManager, DriftMap* pDriftMap, std::string filepath)
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
            if (!parser_helpers::validateElement(xmlLayout, "layout"))
            {
                throwError(OperationNotifier::Operation::PARSING, "No layout node as root in XML found", filepath);
            }

            // Extract name of layout
            std::string name = extractFileName(filepath);

			// Extract style classes
			std::string stylesString = parser_helpers::parseStringAttribute("style", xmlLayout);

			// Split classes by space
			std::string delimiter = " ";
			stylesString += delimiter; // Add final space for simpler loop
			size_t pos = 0;
			std::vector<std::string> styles;
			std::set<std::string> uniqueChecker;
			while ((pos = stylesString.find(delimiter)) != std::string::npos)
			{
				// Read name of class and erase it from content
				std::string style = stylesString.substr(0, pos);
				stylesString.erase(0, pos + delimiter.length());

				// Add name to set to check uniques
				bool unique = uniqueChecker.insert(style).second;

				// Add style if not empty and unique
				if (unique && !style.empty())
				{
					styles.push_back(style);
				}
			}
			
			// Catch empty styles
			if (styles.empty()) { styles.push_back(STYLE_BASE_CLASS_NAME); }

            // Create layout
            std::unique_ptr<Layout> upLayout = std::unique_ptr<Layout>(new Layout(name, pGUI, pAssetManager, pDriftMap, styles));

            // Then there should be an element
            tinyxml2::XMLElement* xmlRoot = xmlLayout->FirstChildElement();

            // Check for existence
            if (xmlRoot == NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "Layout has no element", filepath);
            }

            // Create, parse further internal an attach
            std::unique_ptr<elementsAndIds> upPair;
            upPair = std::move(element_parser::parse(upLayout.get(), upLayout->getMainFrame(), pAssetManager, pDriftMap, upLayout->getNotificationQueue(), xmlRoot, NULL, filepath));
            upLayout->attachElementToMainFrameAsRoot(std::move(upPair->first), std::move(upPair->second));

            // Return ready to use layout
            return(std::move(upLayout));
        }
    }
}
