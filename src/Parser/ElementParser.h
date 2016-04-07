//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for elements. Error checking is done here.

#ifndef ELEMENT_PARSER_H_
#define ELEMENT_PARSER_H_

#include "include/eyeGUI.h"
#include "Rendering/AssetManager.h"
#include "NotificationQueue.h"
#include "Elements/Elements.h"
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
        std::unique_ptr<elementsAndIds> parse(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, std::map<std::string, std::string> idMapper = std::map<std::string, std::string>());

        // Element parsing
        std::unique_ptr<Element> parseElement(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<Grid> parseGrid(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlGrid, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<Blank> parseBlank(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBlank, Element* pParent, std::string filepath);
        std::unique_ptr<Block> parseBlock(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBlock, Element* pParent, std::string filepath);
        std::unique_ptr<Picture> parsePicture(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlPicture, Element* pParent, std::string filepath);
        std::unique_ptr<Stack> parseStack(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlStack, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<TextBlock> parseTextBlock(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlTextBlock, Element* pParent, std::string filepath);
        std::unique_ptr<CircleButton> parseCircleButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlCircleButton, Element* pParent, std::string filepath);
        std::unique_ptr<BoxButton> parseBoxButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBoxButton, Element* pParent, std::string filepath);
        std::unique_ptr<Sensor> parseSensor(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlSensor, Element* pParent, std::string filepath);
        std::unique_ptr<DropButton> parseDropButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlDropButton, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);
        std::unique_ptr<Keyboard> parseKeyboard(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlKeyboard, Element* pParent, std::string filepath);
        std::unique_ptr<WordSuggest> parseWordSuggest(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlWordSuggest, Element* pParent, std::string filepath);
        std::unique_ptr<Flow> parseFlow(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::string styleName, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlFlow, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap);

        // Helper
        void blockHelper(tinyxml2::XMLElement const * xmlBlock, bool& rConsumeInput, std::string& rBackgroundFilepath, ImageAlignment& rBackgroundAlignment, float& rInnerBorder);
        void fontSizeHelper(tinyxml2::XMLElement const * xmlElement, FontSize& rFontSize, std::string filepath);
        void localizationHelper(tinyxml2::XMLElement const * xmlElement, std::string contentAttribute, std::string keyAttribute, std::u16string& rContent, std::string& rKey);

        // Checking
        bool validateElement(tinyxml2::XMLElement const * xmlElement, const std::string& rExpectedValue);

        // Other parsing
        float parseRelativeScale(tinyxml2::XMLElement const * xmlElement);
        std::string parseStyleName(tinyxml2::XMLElement const * xmlElement, Element const * pParent, const std::set<std::string>& rNamesOfAvailableStyles, std::string filepath);

        // Attribute parsing
        std::string parseStringAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, std::string fallback = EMPTY_STRING_ATTRIBUTE);
        bool parseBoolAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, bool fallback = EMPTY_BOOL_ATTRIBUTE);
        int parseIntAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, int fallback = EMPTY_INT_ATTRIBUTE);
        float parsePercentAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, float fallback = EMPTY_PERCENTAGE_ATTRIBUTE);

        // IdMap
        bool checkElementId(const idMap& rIdMap, const std::string& rId, std::string filepath);
        void insertElementId(idMap& rIdMap, const std::string& rId, Element* pElement);
    }
}

#endif // ELEMENT_PARSER_H_
