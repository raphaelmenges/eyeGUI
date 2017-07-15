//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Layout manages frames with elements. There is one main frame which is
// screen filling and a free number of floating frames. Does mapping of id to
// element pointer. Has notification queue where elements in frames can enqueue
// their notifications.

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "include/eyeGUI.h"
#include "src/Styleable.h"
#include "src/Frame.h"
#include "src/Parser/BrickParser.h"
#include "src/NotificationQueue.h"
#include "src/Utilities/LerpValue.h"
#include "src/Utilities/OperationNotifier.h"

#include <memory>
#include <map>
#include <set>
#include <utility>

namespace eyegui
{
    // Forward declaration
    class GUI;
	class StyleClass;

    class Layout : public Styleable
    {
    public:

        // Friends (may use private methods)
        friend class NotificationQueue;

        // Constructor
        Layout(std::string name, GUI const * pGUI, AssetManager* pAssetManager, std::vector<std::string> styles);

        // Destructor
        virtual ~Layout();

        // Get name
        std::string getName() const;

        // Updating
        void update(float tpf, Input* pInput);

        // Drawing
        void draw() const;

        // Make resize necessary
        void makeResizeNecessary(bool force, bool instant = false);

        // Attach element to main frame as root
        void attachElementToMainFrameAsRoot(
            std::unique_ptr<Element> upElement,
            std::unique_ptr<std::map<std::string, Element*> > upIds);

        // Get notificaton queue
        NotificationQueue* getNotificationQueue() const;

        // Get global keyboard layout
        KeyboardLayout getKeyboardLayout() const;

        // Get main frame of layout
        Frame* getMainFrame();

        // Get layout resolution
        uint getLayoutWidth() const;
        uint getLayoutHeight() const;

        // Get time from GUI
        float getAccPeriodicTime() const;

        // Get string content from localization
        std::u16string getContentFromLocalization(std::string key) const;

		// Check visibility
		bool isVisible() const;

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

        // Set interactive element as highlighted
        void highlightInteractiveElement(std::string id, bool doHighlight);

        // Set icon of icon element
        void setIconOfIconElement(std::string id, std::string iconFilepath);

        // Set icon of icon element
        void setIconOfIconElement(
            std::string id,
            std::string name,
            int width,
            int height,
            ColorFormat format,
            unsigned char const * pIconData,
            bool flipY);

        // Set image of picture
        void setImageOfPicture(
            std::string id,
            std::string name,
            int width,
            int height,
            ColorFormat format,
            unsigned char const * pData,
            bool flipY);

		// Set image of picture
		void setImageOfPicture(
			std::string id,
			std::string name);

		// Fetch image
		void fetchImage(
			std::string name,
			int width,
			int height,
			ColorFormat format,
			unsigned char const * pData,
			bool flipY);

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

        // Set content of text block
        void setContentOfTextBlock(std::string id, std::u16string content);
        void setContentOfTextBlock(std::string id, std::string content);

        // Set key of text block
        void setKeyOfTextBlock(std::string id, std::string key);

        // Set fast typing for keyboard
        void setFastTypingOfKeyboard(std::string id, bool useFastTyping);

        // Set case of keyboard
        void setCaseOfKeyboard(std::string id, KeyboardCase keyboardCase);

        // Get count of keymaps in keyboard
        uint getCountOfKeymapsInKeyboard(std::string id) const;

        // Set keymap of keyboard by index
        void setKeymapOfKeyboard(std::string id, uint keymapIndex);

		// Classify currently selected key
		void classifyKey(std::string id, bool accept);

        // Suggest words
        void suggestWords(std::string id, std::u16string input, uint dictionaryIndex, std::u16string& rBestSuggestion);
        void suggestWords(std::string id, std::string input, uint dictionaryIndex, std::string& rBestSuggestion);

        // Clear suggestions
        void clearSuggestions(std::string id);

        // Set space of flow
        void setSpaceOfFlow(std::string id, float space);

        // Set progress in progress bar
        void setProgress(std::string id, float progress);

		// Move cursor a certain amount of letters
		void moveCursorOverLettersInTextEdit(std::string id, int letterCount);

		// Move cursor a certain amount of words
		void moveCursorOverWordsInTextEdit(std::string id, int wordCount);

        // Move cursor to start
        void moveCursorToStartInTextEdit(std::string id);

        // Move cursor to end
        void moveCursorToEndInTextEdit(std::string id);

		// Add content at cursor
		void addContentAtCursorInTextEdit(std::string id, std::u16string content);

		// Set content
		void setContentOfTextEdit(std::string id, std::u16string content);

		// Get content
		std::u16string getContentOfTextEdit(std::string id) const;

		// Delete content
		void deleteContentAtCursorInTextEdit(std::string id, int letterCount);

		// Get content of acitve entity
		std::u16string getActiveEntityContentInTextEdit(std::string id) const;

		// Set content of acitve entity
		void setActiveEntityContentInTextEdit(std::string id, std::u16string content);

        // Add brick to stack
        void addBrickToStack(
            std::string id,
            std::string filepath,
            std::map<std::string, std::string> idMapper);

        // Register button listener
        void registerButtonListener(std::string id, std::weak_ptr<ButtonListener> wpListener);

        // Register sensor listener
        void registerSensorListener(std::string id, std::weak_ptr<SensorListener> wpListener);

        // Register keyboard listener
        void registerKeyboardListener(std::string id, std::weak_ptr<KeyboardListener> wpListener);

        // Register word suggest listener
        void registerWordSuggestListener(std::string id, std::weak_ptr<WordSuggestListener> wpListener);

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

		// Flash element
		void flash(std::string id);

        // Check whether element is highlighted
        bool isInteractiveElementHighlighted(std::string id) const;

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

        // Get visibility of descriptions
        DescriptionVisibility getDescriptionVisibility() const;

        // Get font size for descriptions
        FontSize getDescriptionFontSize() const;

		// Fetch style class from style tree
		std::shared_ptr<const StyleClass> fetchStyleTreeClass(std::string name) const;

		// Get raw values of styling property
		template<typename Type>
		typename style::PropertyInfo<Type>::type getStyleValue(Type type) const
		{
			// Go over style classes of tree
			std::shared_ptr<const StyleProperty<typename style::PropertyInfo<Type>::type> > spStyleProperty;
			for (const auto& rspStyleClass : mStyleTreeClasses)
			{
				// Check whether property is really set or just base
				spStyleProperty = rspStyleClass->fetchProperty(type);
				if (spStyleProperty->isSet()) // just base, try next class
				{
					continue;
				}
				else // no base, use this property's value
				{
					break;
				}
			}
			return spStyleProperty->get();
		}

		// Set value of style owned by element
		template<typename Type>
		void parseElementStylePropertyValue(std::string id, Type stylePropertyType, std::string value)
		{
			Element* pElement = fetchElement(id);
			if (pElement != NULL)
			{
				pElement->parseElementStylePropertyValue(stylePropertyType, value);
			}
			else
			{
				throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
			}
		}

		// Execute query on elements
		void executeOnElements(std::function<void(Element*)> function)
		{
			mupMainFrame->executeOnElements(function);
			for (auto& rFrame : mFloatingFrames)
			{
				rFrame->executeOnElements(function);
			}
		}

		// Experimental

		// Register future keyboard listener
		void registerFutureKeyboardListener(std::string id, std::weak_ptr<eyegui_experimental::FutureKeyboardListener> wpListener);

		// Set suggestions of line
		void setFutureKeyboardLineSuggestions(
			std::string id,
			std::u16string suggestionA,
			std::u16string suggestionB,
			std::u16string suggestionC);

		// Set suggestion in future key
		void setFutureKeySuggestion(
			std::string id,
			std::string keyId,
			std::u16string suggestion);

		// Get content of future keyboard
		std::u16string getFutureKeyboardContent(std::string id) const;

		// Set next future keyboard sentence
		void nextFutureKeyboardSentence(
			std::string id,
			std::u16string sentence);

		// Clear predisplay of future keyboard
		void clearFutureKeyboardPredisplay(std::string id);

    private:

        // Resize function
        void internalResizing(bool force, bool instant = false);

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
        std::string mName;
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
        InteractiveElement* mpSelectedInteractiveElement;
        std::unique_ptr<NotificationQueue> mupNotificationQueue;
        bool mForceResize;
    };
}

#endif // LAYOUT_H_
