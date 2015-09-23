//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Parser for elements.

#ifndef ELEMENT_PARSER_H_
#define ELEMENT_PARSER_H_

#include "eyeGUI.h"
#include "Rendering/AssetManager.h"
#include "Elements/Elements.h"
#include "External/TinyXML2/tinyxml2.h"
#include "External/GLM/glm/vec4.hpp"

#include <memory>
#include <map>

namespace eyegui
{
    // Typedefs
    typedef std::map<std::string, Element*> idMap;
    typedef std::pair<std::unique_ptr<Element>, std::unique_ptr<std::map<std::string, Element*> > > elementsAndIds;

    // Forward declarations
    class Frame;

    class ElementParser
    {
    public:

        // Constructor
        ElementParser();

        // Destructor
        virtual ~ElementParser();

        // Parsing
        std::unique_ptr<elementsAndIds> parse(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath) const;

        // Element parsing
        std::unique_ptr<Element> parseElement(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<Grid> parseGrid(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlGrid, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<Blank> parseBlank(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlBlank, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<Block> parseBlock(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlBlock, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<Picture> parsePicture(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlPicture, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<Stack> parseStack(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlStack, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<CircleButton> parseCircleButton(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlCircleButton, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<BoxButton> parseBoxButton(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlBoxButton, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<Sensor> parseSensor(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlSensor, Element* pParent, std::string filepath, idMap& rIdMap) const;
        std::unique_ptr<DropButton> parseDropButton(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlDropButton, Element* pParent, std::string filepath, idMap& rIdMap) const;

        // Checking
        bool validateElement(tinyxml2::XMLElement const * xmlElement, const std::string& expectedValue) const;

        // Other parsing
        float parseRelativeScale(tinyxml2::XMLElement const * xmlElement) const;
        std::string parseStyleName(tinyxml2::XMLElement const * xmlElement, Element const * pParent, const std::set<std::string>& rNamesOfAvailableStyles, std::string filepath) const;

        // Attribute parsing
        std::string parseStringAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement) const;
        bool parseBoolAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement) const;
        int parseIntAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement) const;
        float parsePercentAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement) const;

        // IdMap
        bool checkElementId(const idMap& rIdMap, const std::string& rId, std::string filepath) const;
        void insertElementId(idMap& rIdMap, const std::string& rId, Element* pElement) const;
    };
}

#endif // ELEMENT_PARSER_H_
