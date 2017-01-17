//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ElementParser.h"

#include "src/Elements/ElementFactory.h"
#include "Defines.h"
#include "Layout.h"
#include "src/Utilities/OperationNotifier.h"

namespace eyegui
{
    namespace element_parser
    {
        std::unique_ptr<elementsAndIds> parse(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, std::map<std::string, std::string> idMapper)
        {
            // Create map for ids
            std::unique_ptr<idMap> upIdMap = std::unique_ptr<idMap>(new std::map<std::string, Element*>);

            // Parse elements
            std::unique_ptr<Element> upElement = std::move(parseElement(pLayout, pFrame, pAssetManager, pNotificationQueue, xmlElement, pParent, filepath, idMapper, *(upIdMap.get())));

            // Create and fill pair
            std::unique_ptr<elementsAndIds> upPair = std::unique_ptr<elementsAndIds>(new elementsAndIds);
            upPair->first = std::move(upElement);
            upPair->second = std::move(upIdMap);

            // Return pair
            return std::move(upPair);
        }

        std::unique_ptr<Element> parseElement(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap)
        {
            // Names of style classes of element (if xmlElement == NULL, get style of parent if available)
            std::vector<std::string> styles = parseStyleClassesNames(xmlElement, pParent, filepath);

            if (xmlElement == NULL)
            {
                // Fallback
                return(std::move(parseBlank(pLayout, pFrame, pAssetManager, pNotificationQueue, EMPTY_STRING_ATTRIBUTE, styles, 1, 0, false, false, NULL, pParent, filepath)));
            }

            // Unique pointer to element
            std::unique_ptr<Element> upElement;

            // Id of element
            std::string id = parseStringAttribute("id", xmlElement);

            // Try to map id using id mapper
            auto it = rIdMapper.find(id);
            if (it != rIdMapper.end())
            {
                id = it->second;
            }

            // Relative scale of element
            float relativeScale = parseRelativeScale(xmlElement);

            // Border of element
            float border = parsePercentAttribute("border", xmlElement);

            // Dimming
            bool dimming = parseBoolAttribute("dimming", xmlElement);

            // Adaptive scaling
            bool adaptiveScaling = parseBoolAttribute("adaptivescaling", xmlElement);

            std::string value = std::string(xmlElement->Value());
            if (value == "grid")
            {
                upElement = std::move(parseGrid(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath, rIdMapper, rIdMap));
            }
            else if (value == "blank")
            {
                upElement = std::move(parseBlank(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "block")
            {
                upElement = std::move(parseBlock(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "picture")
            {
                upElement = std::move(parsePicture(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "stack")
            {
                upElement = std::move(parseStack(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath, rIdMapper, rIdMap));
            }
            else if (value == "textblock")
            {
                upElement = std::move(parseTextBlock(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "circlebutton")
            {
                upElement = std::move(parseCircleButton(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "boxbutton")
            {
                upElement = std::move(parseBoxButton(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "sensor")
            {
                upElement = std::move(parseSensor(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "dropbutton")
            {
                upElement = std::move(parseDropButton(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath, rIdMapper, rIdMap));
            }
            else if (value == "keyboard")
            {
                upElement = std::move(parseKeyboard(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "wordsuggest")
            {
                upElement = std::move(parseWordSuggest(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
            else if (value == "flow")
            {
                upElement = std::move(parseFlow(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath, rIdMapper, rIdMap));
            }
            else if (value == "progressbar")
            {
                upElement = std::move(parseProgressBar(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
            }
			else if (value == "textedit")
			{
				upElement = std::move(parseTextEdit(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
			}
			// Experimental
			else if (value == "futurekeyboard")
			{
				upElement = std::move(parseFutureKeyboard(pLayout, pFrame, pAssetManager, pNotificationQueue, id, styles, relativeScale, border, dimming, adaptiveScaling, xmlElement, pParent, filepath));
			}
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown element found: " + std::string(xmlElement->Value()), filepath);
            }

            // Return element, but save id before
            if (checkElementId(rIdMap, id, filepath))
            {
                insertElementId(rIdMap, id, upElement.get());
                return (std::move(upElement));
            }
            else
            {
                return NULL;
            }
        }

        std::unique_ptr<Grid> parseGrid(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlGrid, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap)
        {
            // Fetch values for block from xml
            bool consumeInput;
            std::string backgroundFilepath;
            ImageAlignment backgroundAlignment;
            float innerBorder;
            blockHelper(xmlGrid, consumeInput, backgroundFilepath, backgroundAlignment, innerBorder);

            // Show background?
            bool showBackground = parseBoolAttribute("showbackground", xmlGrid);

            // Get first row
            tinyxml2::XMLElement const * xmlRow = xmlGrid->FirstChildElement();
            if (!validateElement(xmlRow, "row"))
            {
                throwError(OperationNotifier::Operation::PARSING, "Row node expected but not found", filepath);
            }

            // Count of rows
            int rows = 0;
            tinyxml2::XMLElement const * xmlRowCounter = xmlRow;

            while (xmlRowCounter != NULL)
            {
                // Checking whether it is really a row element is done later
                rows++;
                xmlRowCounter = xmlRowCounter->NextSiblingElement();
            }

            // Create grid
			ElementFactory fac;
			auto upGrid = fac.build<Grid>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				consumeInput,
				backgroundFilepath,
				backgroundAlignment,
				innerBorder,
				showBackground,
				rows);

            // Go over rows
            for (int i = 0; i < rows; i++)
            {
                // Get first column
                tinyxml2::XMLElement const * xmlColumn = xmlRow->FirstChildElement();
                if (!validateElement(xmlColumn, "column"))
                {
                    throwError(OperationNotifier::Operation::PARSING, "Column node expected but not found", filepath);
                }

                // Get column count
                int columns = 0;
                tinyxml2::XMLElement const * xmlColumnCounter = xmlColumn;

                while (xmlColumnCounter != NULL)
                {
                    // Checking whether it is really a column element is done later
                    columns++;
                    xmlColumnCounter = xmlColumnCounter->NextSiblingElement();
                }

                // Tell grid about column count
                upGrid->prepareColumns(i, columns);

                // Set height of row
                upGrid->setRelativeHeightOfRow(i, parsePercentAttribute("size", xmlRow));

                // Go over columns
                for (int j = 0; j < columns; j++)
                {
                    // Set width of column
                    upGrid->setRelativeWidthOfCell(i, j, parsePercentAttribute("size", xmlColumn));

                    // Determine element in cell
                    tinyxml2::XMLElement const * xmlElement = xmlColumn->FirstChildElement();
                    upGrid->attachElement(i, j, std::move(parseElement(pLayout, pFrame, pAssetManager, pNotificationQueue, xmlElement, upGrid.get(), filepath, rIdMapper, rIdMap)));

                    // Get column sibling if necessary
                    xmlColumn = xmlColumn->NextSiblingElement();
                    if (j < columns - 1)
                    {
                        if (!validateElement(xmlColumn, "column"))
                        {
                            throwError(OperationNotifier::Operation::PARSING, "Column node expected but not found", filepath);
                        }
                    }
                    else
                    {
                        if (xmlColumn != NULL)
                        {
                            throwError(OperationNotifier::Operation::PARSING, "Too many column nodes found", filepath);
                        }
                    }
                }

                // Get row sibling if necessary
                xmlRow = xmlRow->NextSiblingElement();
                if (i < rows - 1)
                {
                    if (!validateElement(xmlRow, "row"))
                    {
                        throwError(OperationNotifier::Operation::PARSING, "Row node expected but not found", filepath);
                    }
                }
                else
                {
                    if (xmlRow != NULL)
                    {
                        throwError(OperationNotifier::Operation::PARSING, "Too many row nodes found", filepath);
                    }
                }
            }

            // In the end, check whether sum of sizes are ok
            if (!upGrid->gridCompletelyFilledBySizes())
            {
                throwError(OperationNotifier::Operation::PARSING, "Sum of relative sizes in grid are not 100 percent", filepath);
            }

            // Return grid and give ownership
            return std::move(upGrid);
        }

        std::unique_ptr<Blank> parseBlank(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBlank, Element* pParent, std::string filepath)
        {
            // Create blank
			ElementFactory fac;
			auto upBlank = fac.build<Blank>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling);
            
			// Return blank
			return (std::move(upBlank));
        }

        std::unique_ptr<Block> parseBlock(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBlock, Element* pParent, std::string filepath)
        {
            // Fetch values for block from xml
            bool consumeInput;
            std::string backgroundFilepath;
            ImageAlignment backgroundAlignment;
            float innerBorder;
            blockHelper(xmlBlock, consumeInput, backgroundFilepath, backgroundAlignment, innerBorder);

            // Create block
			ElementFactory fac;
			auto upBlock = fac.build<Block>(
                        id,
                        styles,
                        pParent,
                        pLayout,
                        pFrame,
                        pAssetManager,
                        pNotificationQueue,
                        relativeScale,
                        border,
                        dimming,
                        adaptiveScaling,
                        consumeInput,
                        backgroundFilepath,
                        backgroundAlignment);

			// Return block
            return (std::move(upBlock));
        }

        std::unique_ptr<Picture> parsePicture(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlPicture, Element* pParent, std::string filepath)
        {
            // Get full path to image file
            std::string imageFilepath = parseStringAttribute("src", xmlPicture);

            // Get alignment
            std::string alignmentValue = parseStringAttribute("alignment", xmlPicture);
            ImageAlignment alignment = ImageAlignment::ORIGINAL;
            if (alignmentValue == EMPTY_STRING_ATTRIBUTE || alignmentValue == "original")
            {
                alignment = ImageAlignment::ORIGINAL;
            }
            else if (alignmentValue == "stretched")
            {
                alignment = ImageAlignment::STRETCHED;
            }
            else if (alignmentValue == "zoomed")
            {
                alignment = ImageAlignment::ZOOMED;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown alignment used for picture: " + alignmentValue, filepath);
            }

            // Create picture
			ElementFactory fac;
			auto upPicture = fac.build<Picture>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				imageFilepath,
				alignment);
            
			// Return picture
			return (std::move(upPicture));
        }

        std::unique_ptr<Stack> parseStack(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlStack, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap)
        {
            // Fetch values for block from xml
            bool consumeInput;
            std::string backgroundFilepath;
            ImageAlignment backgroundAlignment;
            float innerBorder;
            blockHelper(xmlStack, consumeInput, backgroundFilepath, backgroundAlignment, innerBorder);

            // Show background?
            bool showBackground = parseBoolAttribute("showbackground", xmlStack);

			// Get direction of stack
			std::string directionValue = parseStringAttribute("direction", xmlStack);
			Stack::Direction direction = Stack::Direction::AUTOMATIC;
			if (directionValue == EMPTY_STRING_ATTRIBUTE || directionValue == "automatic")
			{
				direction = Stack::Direction::AUTOMATIC;
			}
			else if (directionValue == "horizontal")
			{
				direction = Stack::Direction::HORIZONTAL;
			}
			else if (directionValue == "vertical")
			{
				direction = Stack::Direction::VERTICAL;
			}
			else
			{
				throwError(OperationNotifier::Operation::PARSING, "Unknown direction used in stack: " + directionValue, filepath);
			}

            // Get mode of relative scaling
            std::string relativeScalingValue = parseStringAttribute("relativescaling", xmlStack);
            Stack::RelativeScaling relativeScaling = Stack::RelativeScaling::MAIN_AXIS;
            if (relativeScalingValue == EMPTY_STRING_ATTRIBUTE || relativeScalingValue == "mainaxis")
            {
                relativeScaling = Stack::RelativeScaling::MAIN_AXIS;
            }
            else if (relativeScalingValue == "bothaxes")
            {
                relativeScaling = Stack::RelativeScaling::BOTH_AXES;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown relative scaling used in stack: " + relativeScalingValue, filepath);
            }

            // Get alignment
            std::string alignmentValue = parseStringAttribute("alignment", xmlStack);
            Stack::Alignment alignment = Stack::Alignment::FILL;
            if (alignmentValue == EMPTY_STRING_ATTRIBUTE || alignmentValue == "fill")
            {
                alignment = Stack::Alignment::FILL;
            }
            else if (alignmentValue == "center")
            {
                alignment = Stack::Alignment::CENTER;
            }
            else if (alignmentValue == "head")
            {
                alignment = Stack::Alignment::HEAD;
            }
            else if (alignmentValue == "tail")
            {
                alignment = Stack::Alignment::TAIL;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown alignment used in stack: " + alignmentValue, filepath);
            }

            // Get padding
            float padding = parsePercentAttribute("padding", xmlStack);
            if (padding < 0 || padding > 1)
            {
                throwError(OperationNotifier::Operation::PARSING, "Padding value of stack not in range of 0% to 100%", filepath);
            }

            // Get separator
            float separator = parsePercentAttribute("separator", xmlStack);
            if (separator < 0 || separator > 1)
            {
                throwError(OperationNotifier::Operation::PARSING, "Separator value of stack not in range of 0% to 100%", filepath);
            }

            // Create stack
			ElementFactory fac;
			auto upStack = fac.build<Stack>(
                    id,
                    styles,
                    pParent,
                    pLayout,
                    pFrame,
                    pAssetManager,
                    pNotificationQueue,
                    relativeScale,
                    border,
                    dimming,
                    adaptiveScaling,
                    consumeInput,
                    backgroundFilepath,
                    backgroundAlignment,
                    innerBorder,
                    showBackground,
					direction,
                    relativeScaling,
                    alignment,
                    padding,
                    separator);

            // Insert elements
            tinyxml2::XMLElement const * xmlElement = xmlStack->FirstChildElement();
            while (xmlElement != NULL)
            {
                // Fetch element
                upStack->attachElement(std::move(parseElement(pLayout, pFrame, pAssetManager, pNotificationQueue, xmlElement, upStack.get(), filepath, rIdMapper, rIdMap)));

                // Try to get next sibling element
                xmlElement = xmlElement->NextSiblingElement();
            }

            // Return stack
            return (std::move(upStack));
        }

        std::unique_ptr<TextBlock> parseTextBlock(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlTextBlock, Element* pParent, std::string filepath)
        {
            // Fetch values for block from xml
            bool consumeInput;
            std::string backgroundFilepath;
            ImageAlignment backgroundAlignment;
            float innerBorder;
            blockHelper(xmlTextBlock, consumeInput, backgroundFilepath, backgroundAlignment, innerBorder);

            // Get font size
            FontSize fontSize;
            fontSizeHelper(xmlTextBlock, fontSize, filepath);

            // Get alignment
            std::string alignmentValue = parseStringAttribute("alignment", xmlTextBlock);
            TextFlowAlignment alignment = TextFlowAlignment::LEFT;
            if (alignmentValue == EMPTY_STRING_ATTRIBUTE || alignmentValue == "left")
            {
                alignment = TextFlowAlignment::LEFT;
            }
            else if (alignmentValue == "right")
            {
                alignment = TextFlowAlignment::RIGHT;
            }
            else if (alignmentValue == "center")
            {
                alignment = TextFlowAlignment::CENTER;
            }
            else if (alignmentValue == "justify")
            {
                alignment = TextFlowAlignment::JUSTIFY;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown alignment used in text block: " + alignmentValue, filepath);
            }

            // Get vertical alignment
            std::string verticalAlignmentValue = parseStringAttribute("verticalalignment", xmlTextBlock);
            TextFlowVerticalAlignment verticalAlignment = TextFlowVerticalAlignment::TOP;
            if (verticalAlignmentValue == EMPTY_STRING_ATTRIBUTE || verticalAlignmentValue == "top")
            {
                verticalAlignment = TextFlowVerticalAlignment::TOP;
            }
            else if (verticalAlignmentValue == "center")
            {
                verticalAlignment = TextFlowVerticalAlignment::CENTER;
            }
            else if (verticalAlignmentValue == "bottom")
            {
                verticalAlignment = TextFlowVerticalAlignment::BOTTOM;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown vertical alignment used in text block: " + verticalAlignmentValue, filepath);
            }

            // Get text scale
            float textScale = parsePercentAttribute("textscale", xmlTextBlock, 1.0f);

            // Localization
            std::u16string content;
            std::string key;
            localizationHelper(xmlTextBlock, "content", "key", content, key);

            // Create text block
			ElementFactory fac;
			auto upTextBlock = fac.build<TextBlock>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				consumeInput,
				backgroundFilepath,
				backgroundAlignment,
				innerBorder,
				fontSize,
				alignment,
				verticalAlignment,
				textScale,
				content,
				key);

            // Return text block
            return std::move(upTextBlock);
        }

        std::unique_ptr<CircleButton> parseCircleButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlCircleButton, Element* pParent, std::string filepath)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlCircleButton);

            // Description
            std::u16string desc;
            std::string descKey;
            localizationHelper(xmlCircleButton, "desc", "desckey", desc, descKey);

            // Is button a switch?
            bool isSwitch = parseBoolAttribute("switch", xmlCircleButton);

            // Create circle button
			ElementFactory fac;
			auto upCircleButton = fac.build<CircleButton>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				iconFilepath,
				desc,
				descKey,
				isSwitch);

            // Return circle button
            return (std::move(upCircleButton));
        }

        std::unique_ptr<BoxButton> parseBoxButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlBoxButton, Element* pParent, std::string filepath)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlBoxButton);

            // Description
            std::u16string desc;
            std::string descKey;
            localizationHelper(xmlBoxButton, "desc", "desckey", desc, descKey);

            // Is button a switch?
            bool isSwitch = parseBoolAttribute("switch", xmlBoxButton);

            // Create box button
			ElementFactory fac;
			auto upBoxButton = fac.build<BoxButton>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				iconFilepath,
				desc,
				descKey,
				isSwitch);

            // Return box button
            return (std::move(upBoxButton));
        }

        std::unique_ptr<Sensor> parseSensor(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlSensor, Element* pParent, std::string filepath)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlSensor);

            // Description
            std::u16string desc;
            std::string descKey;
            localizationHelper(xmlSensor, "desc", "desckey", desc, descKey);

            // Create sensor
			ElementFactory fac;
			auto upSensor = fac.build<Sensor>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				iconFilepath,
				desc,
				descKey);

            // Return sensor
            return (std::move(upSensor));
        }

        std::unique_ptr<DropButton> parseDropButton(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlDropButton, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlDropButton);

            // Description
            std::u16string desc;
            std::string descKey;
            localizationHelper(xmlDropButton, "desc", "desckey", desc, descKey);

            // Get usage of available space
            float space = parsePercentAttribute("space", xmlDropButton);
            if (space < 0 || space > 1)
            {
                throwError(OperationNotifier::Operation::PARSING, "Usage of available space in DropButton not in range of 0% to 100%", filepath);
            }

            // Create drop button
			ElementFactory fac;
			auto upDropButton = fac.build<DropButton>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				iconFilepath,
				desc,
				descKey,
				space);

            // Attach inner element
            tinyxml2::XMLElement const * xmlElement = xmlDropButton->FirstChildElement();

            if (xmlElement == NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "DropButton has no inner element", filepath);
            }
            else if (xmlElement->NextSiblingElement() != NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "DropButton has more than one inner element", filepath);
            }

            upDropButton->attachInnerElement(std::move(parseElement(pLayout, pFrame, pAssetManager, pNotificationQueue, xmlElement, upDropButton.get(), filepath, rIdMapper, rIdMap)));

            // Check, that drop button was NOT inserted into some kind of scrolling box
            if (upDropButton->checkForParentType(Element::Type::FLOW))
            {
                throwWarning(OperationNotifier::Operation::PARSING, "DropButton is directly or indirectly child of Flow. Inner element will render on top of frame and not influenced by Flow", filepath);
            }

            // Return drop button
            return (std::move(upDropButton));
        }

        std::unique_ptr<Keyboard> parseKeyboard(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlKeyboard, Element* pParent, std::string filepath)
        {
			// Show background?
			bool instantPress = parseBoolAttribute("instantpress", xmlKeyboard);

            // Create keyboard
			ElementFactory fac;
			auto upKeyboard = fac.build<Keyboard>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				instantPress);
            
			// Return keyboard
			return (std::move(upKeyboard));
        }

        std::unique_ptr<WordSuggest> parseWordSuggest(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlWordSuggest, Element* pParent, std::string filepath)
        {
            // Get font size
            FontSize fontSize;
            fontSizeHelper(xmlWordSuggest, fontSize, filepath);

            // Create word suggest
			ElementFactory fac;
			auto upWordSuggest = fac.build<WordSuggest>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				fontSize);
            
			// Return word suggest
			return (std::move(upWordSuggest));
        }

        std::unique_ptr<Flow> parseFlow(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlFlow, Element* pParent, std::string filepath, std::map<std::string, std::string>& rIdMapper, idMap& rIdMap)
        {
            // Fetch values for block from xml
            bool consumeInput;
            std::string backgroundFilepath;
            ImageAlignment backgroundAlignment;
            float innerBorder;
            blockHelper(xmlFlow, consumeInput, backgroundFilepath, backgroundAlignment, innerBorder);

            // Show background?
            bool showBackground = parseBoolAttribute("showbackground", xmlFlow);

            // Get flow direction
            std::string flowDirectionValue = parseStringAttribute("direction", xmlFlow);
            Flow::Direction flowDirection = Flow::Direction::VERTICAL;
            if (flowDirectionValue == EMPTY_STRING_ATTRIBUTE || flowDirectionValue == "vertical")
            {
                flowDirection = Flow::Direction::VERTICAL;
            }
            else if (flowDirectionValue == "horizontal")
            {
                flowDirection = Flow::Direction::HORIZONTAL;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown direction used in flow: " + flowDirectionValue, filepath);
            }

            // Get space
            float space = parsePercentAttribute("space", xmlFlow);

            // Create flow
			ElementFactory fac;
			auto upFlow = fac.build<Flow>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				consumeInput,
				backgroundFilepath,
				backgroundAlignment,
				innerBorder,
				showBackground,
				flowDirection,
				space);

            // Attach inner element
            tinyxml2::XMLElement const * xmlElement = xmlFlow->FirstChildElement();

            if (xmlElement == NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "Flow has no inner element", filepath);
            }
            else if (xmlElement->NextSiblingElement() != NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "Flow has more than one inner element", filepath);
            }

            upFlow->attachInnerElement(std::move(parseElement(pLayout, pFrame, pAssetManager, pNotificationQueue, xmlElement, upFlow.get(), filepath, rIdMapper, rIdMap)));

            // Return flow
            return (std::move(upFlow));
        }

        std::unique_ptr<ProgressBar> parseProgressBar(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlProgressBar, Element* pParent, std::string filepath)
        {
            // Fetch values for block from xml
            bool consumeInput;
            std::string backgroundFilepath;
            ImageAlignment backgroundAlignment;
            float innerBorder;
            blockHelper(xmlProgressBar, consumeInput, backgroundFilepath, backgroundAlignment, innerBorder);

            // Fetch direction of progress
            std::string progressDirectionValue = parseStringAttribute("direction", xmlProgressBar);
            ProgressBar::Direction progressDirection = ProgressBar::Direction::LEFT_TO_RIGHT;
            if (progressDirectionValue == EMPTY_STRING_ATTRIBUTE || progressDirectionValue == "leftright")
            {
                progressDirection = ProgressBar::Direction::LEFT_TO_RIGHT;
            }
            else if (progressDirectionValue == "rightleft")
            {
                progressDirection = ProgressBar::Direction::RIGHT_TO_LEFT;
            }
            else if (progressDirectionValue == "topbottom")
            {
                progressDirection = ProgressBar::Direction::TOP_TO_BOTTOM;
            }
            else if (progressDirectionValue == "bottomtop")
            {
                progressDirection = ProgressBar::Direction::BOTTOM_TO_TOP;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown direction used in progress: " + progressDirectionValue, filepath);
            }

            // Create progress bar
			ElementFactory fac;
			auto upProgressBar = fac.build<ProgressBar>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				consumeInput,
				backgroundFilepath,
				backgroundAlignment,
				innerBorder,
				progressDirection);

            // Return progress bar
            return (std::move(upProgressBar));
        }

		std::unique_ptr<TextEdit> parseTextEdit(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlTextEdit, Element* pParent, std::string filepath)
		{
			// Get font size
			FontSize fontSize;
			fontSizeHelper(xmlTextEdit, fontSize, filepath);

			// Create text edit
			ElementFactory fac;
			auto upTextEdit = fac.build<TextEdit>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				fontSize);
			
			// Return text edit
			return (std::move(upTextEdit));
		}

		std::unique_ptr<FutureKeyboard> parseFutureKeyboard(Layout const * pLayout, Frame* pFrame, AssetManager* pAssetManager, NotificationQueue* pNotificationQueue, std::string id, std::vector<std::string> styles, float relativeScale, float border, bool dimming, bool adaptiveScaling, tinyxml2::XMLElement const * xmlFutureKeyboard, Element* pParent, std::string filepath)
		{
			// Decide mode
			std::string modeValue = parseStringAttribute("mode", xmlFutureKeyboard);
			FutureKeyboard::Mode mode = FutureKeyboard::Mode::ONE_SUGGESTION_LINE;
			if (modeValue == EMPTY_STRING_ATTRIBUTE || modeValue == "onesuggestionline")
			{
				mode = FutureKeyboard::Mode::ONE_SUGGESTION_LINE;
			}
			else if (modeValue == "manysuggestionlines")
			{
				mode = FutureKeyboard::Mode::MANY_SUGGESTION_LINES;
			}
			else if (modeValue == "suggestionsperkey")
			{
				mode = FutureKeyboard::Mode::SUGGESTION_PER_KEY;
			}
			else
			{
				throwError(OperationNotifier::Operation::PARSING, "Unknown mode used in future keyboard: " + modeValue, filepath);
			}

			// Create future keyboard
			ElementFactory fac;
			auto upFutureKeyboard = fac.build<FutureKeyboard>(
				id,
				styles,
				pParent,
				pLayout,
				pFrame,
				pAssetManager,
				pNotificationQueue,
				relativeScale,
				border,
				dimming,
				adaptiveScaling,
				mode);
			
			// Return future keyboard
			return (std::move(upFutureKeyboard));
		}

        void blockHelper(tinyxml2::XMLElement const * xmlBlock, bool& rConsumeInput, std::string& rBackgroundFilepath, ImageAlignment& rBackgroundAlignment, float& rInnerBorder)
        {
            rConsumeInput = parseBoolAttribute("consumeinput", xmlBlock);
            rBackgroundFilepath = parseStringAttribute("backgroundsrc", xmlBlock);

            // Background image alignment
            if(rBackgroundFilepath == EMPTY_STRING_ATTRIBUTE)
            {
                // Not necessary, so just use something
                rBackgroundAlignment = STANDARD_IMAGE_ALIGNMENT;
            }
            else
            {
                std::string backgroundAlignmentValue = parseStringAttribute("backgroundalignment", xmlBlock);
                if(backgroundAlignmentValue == "original")
                {
                    rBackgroundAlignment = ImageAlignment::ORIGINAL;
                }
                else if(backgroundAlignmentValue == "stretched")
                {
                    rBackgroundAlignment = ImageAlignment::STRETCHED;
                }
                else if(backgroundAlignmentValue == "zoomed")
                {
                    rBackgroundAlignment = ImageAlignment::ZOOMED;
                }
                else
                {
                    // Just use the standard
                    rBackgroundAlignment = STANDARD_IMAGE_ALIGNMENT;
                }
            }

            rInnerBorder = parsePercentAttribute("innerborder", xmlBlock);
        }

        void fontSizeHelper(tinyxml2::XMLElement const * xmlElement, FontSize& rFontSize, std::string filepath)
        {
            std::string fontSizeValue = parseStringAttribute("fontsize", xmlElement);
            if (fontSizeValue == EMPTY_STRING_ATTRIBUTE || fontSizeValue == "medium")
            {
                rFontSize = FontSize::MEDIUM;
            }
            else if (fontSizeValue == "tall")
            {
                rFontSize = FontSize::TALL;
            }
            else if (fontSizeValue == "small")
            {
                rFontSize = FontSize::SMALL;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown font size used in element: " + fontSizeValue, filepath);
            }
        }

        void localizationHelper(tinyxml2::XMLElement const * xmlElement, std::string contentAttribute, std::string keyAttribute, std::u16string& rContent, std::string& rKey)
        {
            // Get content
            std::string contentValue = parseStringAttribute(contentAttribute, xmlElement);

            // Xml parser replaces new lines with visible "\n"
            replaceString(contentValue, "\\n", "\n");

            // Convert to utf-16 string
            convertUTF8ToUTF16(contentValue, rContent);

            // Get key for localization
            rKey = parseStringAttribute(keyAttribute, xmlElement);
        }

        bool validateElement(tinyxml2::XMLElement const * xmlElement, const std::string& rExpectedValue)
        {
            if (xmlElement == NULL || std::string(xmlElement->Value()).compare(rExpectedValue) != 0)
            {
                return false;
            }
            return true;
        }

        float parseRelativeScale(tinyxml2::XMLElement const * xmlElement)
        {
            if (xmlElement == NULL)
            {
                return 1;
            }
            else
            {
                float value = parsePercentAttribute("relativescale", xmlElement);
                if (value <= 0)
                {
                    return 1;
                }
                else
                {
                    return value;
                }
            }
        }

		std::vector<std::string> parseStyleClassesNames(tinyxml2::XMLElement const * xmlElement, Element const * pParent, std::string filepath)
        {
            if (xmlElement == NULL)
            {
				// No style given, use the one from parent
				return pParent->getStyleClassesNames();
            }
            else // get value from xml element
            {
                std::string stylesString = parseStringAttribute("style", xmlElement);
				if (stylesString == EMPTY_STRING_ATTRIBUTE)
				{
					// No style found, try to get one from parent
					if (pParent != NULL)
					{
						return pParent->getStyleClassesNames();
					}
					else // no parent and nothing in style attribute
					{
						// Otherwise, set default as style
						return { STYLE_BASE_CLASS_NAME };
					}
				}
				else // something defined in style attribute
				{
					// Split classes by space
					std::string delimiter = " ";
					stylesString += delimiter; // Add final space for simpler loop
					size_t pos = 0;
					std::vector<std::string> styles;
					while ((pos = stylesString.find(delimiter)) != std::string::npos)
					{
						// Read name of class and erase it from content
						std::string style = stylesString.substr(0, pos);
						stylesString.erase(0, pos + delimiter.length());

						// Add style if not empty
						if (!style.empty())
						{
							styles.push_back(style);
						}
					}

					// Delete styles from parents styles which are defined here
					std::vector<std::string> parentStyles;
					if (pParent != NULL)
					{
						// Fetch style class names
						parentStyles = pParent->getStyleClassesNames();

						// Save indices of values that has to be deleted
						std::vector<int> toBeDeleted;

						// Compare styles from this and the ones from parent
						int i = 0;
						for (const auto& rParentStyle : parentStyles)
						{
							bool deleteThis = false;
							for (const auto& rStyle : styles)
							{
								deleteThis |= rParentStyle.compare(rStyle) == 0;
							}

							if (deleteThis) { toBeDeleted.push_back(i); }
							i++;
						}

						// Filter those which are already defined
						for (int j = (int)toBeDeleted.size() - 1; j >= 0; j--)
						{
							parentStyles.erase(parentStyles.begin() + j);
						}

					}

					// Combine this styles and the ones from parent
					styles.insert(styles.end(), parentStyles.begin(), parentStyles.end());

					// Check for empty vector
					if (styles.empty())
					{
						return{ STYLE_BASE_CLASS_NAME };
					}
					else
					{
						return styles;
					}
				}
            }
        }

        std::string parseStringAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, std::string fallback)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                return xmlAttribute->Value();
            }
            else
            {
                return fallback;
            }
        }

        bool parseBoolAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, bool fallback)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                std::string value = xmlAttribute->Value();

                // File format case is not relevant
                std::transform(value.begin(), value.end(), value.begin(), ::tolower);

                return value == "true";
            }
            else
            {
                return fallback;
            }
        }

        int parseIntAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, int fallback)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                return xmlAttribute->IntValue();
            }
            else
            {
                return fallback;
            }
        }

        float parsePercentAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement, float fallback)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                std::string value = xmlAttribute->Value();
                std::string delimiter = "%";
                std::string token = value.substr(0, value.find(delimiter));
                return (stringToFloat(token) / 100.0f);
            }
            else
            {
                return fallback;
            }
        }

        bool checkElementId(const idMap& rIdMap, const std::string& rId, std::string filepath)
        {
            auto it = rIdMap.find(rId);

            if (it != rIdMap.end())
            {
                if (it->second != NULL)
                {
                    throwError(OperationNotifier::Operation::PARSING, "Following id not unique: " + rId, filepath);
                    return false;
                }
            }

            return true;
        }


        void insertElementId(idMap& rIdMap, const std::string& rId, Element* pElement)
        {
            if (rId != EMPTY_STRING_ATTRIBUTE)
            {
                rIdMap[rId] = pElement;
            }
        }
    }
}
