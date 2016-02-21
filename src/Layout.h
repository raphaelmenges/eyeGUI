//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Layout manages frames with elements. There is one main frame which is
// screen filling and a free number of floating frames. Does mapping of id to
// element pointer. Is owner of the stylesheet used by the roots in frames
// and all their children. Has notification queue where elements in frames
// can enqueue their notifications.

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "include/eyeGUI.h"
#include "Frame.h"
#include "Style.h"
#include "Config.h"
#include "Parser/StylesheetParser.h"
#include "Parser/BrickParser.h"
#include "NotificationQueue.h"
#include "LerpValue.h"

#include <memory>
#include <map>
#include <set>
#include <utility>

namespace eyegui
{
    // Forward declaration
    class GUI;

    class Layout
    {
    public:

        // Friends (may use private methods)
        friend class NotificationQueue;

        // Constructor
        Layout(GUI const * pGUI, AssetManager* pAssetManager, std::string stylesheetFilepath);

        // Destructor
        virtual ~Layout();

        // Updating
        void update(float tpf, Input* pInput);

        // Drawing
        void draw() const;

        // Make resize necessary
        void makeResizeNecessary();

        // Attach element to main frame as root
        void attachElementToMainFrameAsRoot(
            std::unique_ptr<Element> upElement,
            std::unique_ptr<std::map<std::string, Element*> > upIds);

        // Get notificaton queue
        NotificationQueue* getNotificationQueue() const;

        // Get pointer to config of owning GUI
        Config const * getConfig() const;

        // Get main frame of layout
        Frame* getMainFrame();

        // Get layout resolution
        uint getLayoutWidth() const;
        uint getLayoutHeight() const;

        // Get time from GUI
        float getAccPeriodicTime() const;

        // Get pointer to styles of this layout. Returns NULL if not found
        Style const * getStyleFromStylesheet(std::string styleName) const;

        // Get string content from localization
        std::u16string getContentFromLocalization(std::string key) const;

        // Returns set of names of the available styles for this layout
        std::set<std::string> getNamesOfAvailableStyles() const;

        // Set visibility
        void setVisibility(bool visible, bool fade);

        // One may want not to use input for this layout
        void useInput(bool useInput);

        // Get relative position and size of element
        RelativePositionAndSize getRelativePositionAndSizeOfElement(std::string id) const;

        // Get absolute pixel position and size of element
        AbsolutePositionAndSize getAbsolutePositionAndSizeOfElement(std::string id) const;

        // Set element activity
        void setElementActivity(std::string id, bool active, bool fade);

        // Check activity of element
        bool isElementActive(std::string id) const;

        // Set whether element is dimming
        void setElementDimming(std::string id, bool dimming);

        // Set whether element is marking
        void setElementMarking(std::string id, bool marking, int depth);

        // Set hiding of element
        void setElementHiding(std::string id, bool hidden);

        // Check whether element is dimming
        bool isElementDimming(std::string id) const;

        // Check whether element is marking
        bool isElementMarking(std::string id) const;

        // Set style of element
        void setStyleOfElement(std::string id, std::string style);

        // Set interactive element as highlighted
        void highlightInteractiveElement(std::string id, bool doHighlight);

        // Set icon of icon interactive element
        void setIconOfIconInteractiveElement(std::string id, std::string iconFilepath);

        // Interact with interactive element
        void interactWithInteractiveElement(std::string id);

        // Hit button
        void hitButton(std::string id);

        // Button down
        void buttonDown(std::string id, bool immediately);

        // Button up
        void buttonUp(std::string id, bool immediately);

        // Is button a switch?
        bool isButtonSwitch(std::string id) const;

        // Penetrate sensor
        void penetrateSensor(std::string id, float amount);

        // Set content of text block with 16 bit string
        void setContentOfTextBlock(std::string id, std::u16string content);

        // Set content of text block with 8 bit string
        void setContentOfTextBlock(std::string id, std::string content);

        // Set key of text block
        void setKeyOfTextBlock(std::string id, std::string key);

        // Register button listener
        void registerButtonListener(std::string id, std::weak_ptr<ButtonListener> wpListener);

        // Register sensor listener
        void registerSensorListener(std::string id, std::weak_ptr<SensorListener> wpListener);

        // Register keyboard listener
        void registerKeyboardListener(std::string id, std::weak_ptr<KeyboardListener> wpListener);

        // Select interactive element by id
        void selectInteractiveElement(std::string id);

        // Returns, whether successfull
        bool selectInteractiveElement(InteractiveElement* pInteractiveElement);

        // Deselect current element
        void deselectInteractiveElement();

        // Interact with selected interactive element
        void interactWithSelectedInteractiveElement();

        // Select next interactive element
        bool selectNextInteractiveElement();

        // Reset elements
        void resetElements();

        // Check for existence of id
        bool checkForId(std::string id) const;

        // Check whether element is highlighted
        bool isInteractiveElementHighlighted(std::string id) const;

        // Change value of style attribute
        void setValueOfStyleAttribute(std::string styleName, std::string attribute, glm::vec4 value);

        // Replace any element with block
        void replaceElementWithBlock(
            std::string id,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            bool fade);

        // Replace any element with picture
        void replaceElementWithPicture(std::string id, std::string filepath, ImageAlignment alignment, bool fade);

        // Replace any element with blank
        void replaceElementWithBlank(std::string id, bool fade);

        // Replace any element with circle button
        void replaceElementWithCircleButton(std::string id, std::string iconFilepath, bool isSwitch, bool fade);

        // Replace any element with box button
        void replaceElementWithBoxButton(std::string id, std::string iconFilepath, bool isSwitch, bool fade);

        // Replace any element with sensor
        void replaceElementWithSensor(std::string id, std::string iconFilepath, bool fade);

        // Replace any element with text block
        void replaceElementWithTextBlock(
            std::string id,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            FontSize fontSize,
            TextFlowAlignment alignment,
            TextFlowVerticalAlignment verticalAlignment,
			float textScale,
            std::u16string content,
            float innerBorder,
            std::string key,
            bool fade);

        // Replace any element with a brick of elements
        void replaceElementWithBrick(std::string id, std::string filepath, std::map<std::string, std::string> idMapper, bool fade);

        // Add floating frame with brick
        uint addFloatingFrameWithBrick(
            std::string filepath,
            float relativePositionX,
            float relativePositionY,
            float relativeSizeX,
            float relativeSizeY,
            std::map<std::string, std::string> idMapper,
            bool visible,
            bool fade);

        // Set visibilty of floating frame
        void setVisibiltyOfFloatingFrame(uint frameIndex, bool visible, bool fade);

        // Reset elements of floating frame
        void resetFloatingFramesElements(uint frameIndex);

        // Remove floating frame
        void removeFloatingFrame(uint frameIndex, bool fade);

        // Translate floating frame
        void translateFloatingFrame(uint frameIndex, float translateX, float translateY);

        // Scale floating frame
        void scaleFloatingFrame(uint frameIndex, float scaleX, float scaleY);

        // Set position of floating frame
        void setPositionOfFloatingFrame(uint frameIndex, float relativePositionX, float relativePositionY);

        // Set size of floating frame
        void setSizeOfFloatingFrame(uint frameIndex, float relativeSizeX, float relativeSizeY);

        // Move floating frame to front
        void moveFloatingFrameToFront(uint frameIndex);

        // Move floating frame to back
        void moveFloatingFrameToBack(uint frameIndex);

        // Get relative position and size of floating frame
        RelativePositionAndSize getRelativePositionAndSizeOfFloatingFrame(unsigned int frameIndex) const;

        // Get absolute position and size of floating frame
        AbsolutePositionAndSize getAbsolutePositionAndSizeOfFloatingFrame(unsigned int frameIndex) const;

    private:

        // Resize function
        void internalResizing();

        // Fetch pointer to element by id
        Element* fetchElement(std::string id) const;

        // Internal replacement helper, returns whether successful
        bool replaceElement(Element* pTarget, std::unique_ptr<Element> upElement, bool fade);

        // Insert id of element
        void insertId(Element* pElement);

        // Insert map of ids
        void insertIds(std::unique_ptr<idMap> upIdMap);

        // Fetch pointer to frame
        Frame* fetchFloatingFrame(uint frameIndex) const;

        // Move floating frame by id
        void moveFloatingFrame(int oldIndex, int newIndex);

        // Members
        GUI const * mpGUI;
        AssetManager* mpAssetManager;
        std::unique_ptr<Frame> mupMainFrame;
        std::unique_ptr<std::map<std::string, Element*> > mupIds;
        std::vector<std::unique_ptr<Frame> > mFloatingFrames;
        std::vector<int> mFloatingFramesOrderingIndices;
        std::vector<int> mDyingFloatingFramesIndices;
        LerpValue mAlpha;
        bool mVisible;
        bool mResizeNecessary;
        bool mUseInput;
        std::unique_ptr<std::map<std::string, Style> > mupStyles;
        InteractiveElement* mpSelectedInteractiveElement;
        std::unique_ptr<NotificationQueue> mupNotificationQueue;
    };
}

#endif // LAYOUT_H_
