//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for elements. Error checking is done here.

#ifndef ELEMENT_PARSER_H_
#define ELEMENT_PARSER_H_

#include "include/eyeGUI.h"
#include "src/Rendering/AssetManager.h"
#include "src/Utilities/DriftMap.h"
#include "src/NotificationQueue.h"
#include "src/Elements/Elements.h"
#include "externals/TinyXML2/tinyxml2.h"
#include "externals/GLM/glm/vec4.hpp"

#include <memory>
#include <map>

namespace eyegui
{
    // Typedefs
    typedef std::map<std::string, Element*> idMap;
    typedef std::pair<std::unique_ptr<Element>, std::unique_ptr<std::map<std::string, Element*> > > elementsAndIds;

    // Forward declarations
    class Frame;

    namespace element_parser
    {
        // Parsing
        std::unique_ptr<elementsAndIds> parse(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, std::map<std::string, std::string> idMapper = std::map<std::string, std::string>());

        // Element parsing
        std::unique_ptr<Element> parseElement(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<Grid> parseGrid(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlGrid, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<Blank> parseBlank(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBlank, Element* pParent, std::string filepath);
        std::unique_ptr<Block> parseBlock(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBlock, Element* pParent, std::string filepath);
        std::unique_ptr<Picture> parsePicture(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlPicture, Element* pParent, std::string filepath);
        std::unique_ptr<Stack> parseStack(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlStack, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<TextBlock> parseTextBlock(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlTextBlock, Element* pParent, std::string filepath);
        std::unique_ptr<CircleButton> parseCircleButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlCircleButton, Element* pParent, std::string filepath);
        std::unique_ptr<BoxButton> parseBoxButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBoxButton, Element* pParent, std::string filepath);
        std::unique_ptr<Sensor> parseSensor(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlSensor, Element* pParent, std::string filepath);
        std::unique_ptr<DropButton> parseDropButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlDropButton, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<Keyboard> parseKeyboard(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlKeyboard, Element* pParent, std::string filepath);
        std::unique_ptr<WordSuggest> parseWordSuggest(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlWordSuggest, Element* pParent, std::string filepath);
        std::unique_ptr<Flow> parseFlow(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlFlow, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<ProgressBar> parseProgressBar(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlProgressBar, Element* pParent, std::string filepath);
		std::unique_ptr<TextEdit> parseTextEdit(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlTextEdit, Element* pParent, std::string filepath);

		// Experimental
		std::unique_ptr<FutureKeyboard> parseFutureKeyboard(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, DriftMap* pDriftMap, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlFutureKeyboard, Element* pParent, std::string filepath);

		// Helpers
		std::vector<std::string> parseStyleClassesNames(tinyxml2::XMLElement const * xmlElement, Layout const * pLayout, Element const * pParent, std::string filepath);

        // IdMap
        bool checkElementId(const idMap& rIdMap, const std::string& rId, std::string filepath);
        void insertElementId(idMap& rIdMap, const std::string& rId, Element* pElement);
    }
}

#endif // ELEMENT_PARSER_H_
