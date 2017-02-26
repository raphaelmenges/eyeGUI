//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "include/eyeGUI.h"

#include "GUI.h"
#include "Layout.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/PathBuilder.h"
#include "src/Rendering/Textures/PixelTexture.h"

// TODO: Testing
#include "src/TestingArea.h"

// Version
static const std::string VERSION_STRING = "0.9";

namespace eyegui
{
    GUI* GUIBuilder::construct() const
    {
		// TODO: Testing
		testingMain();

        return new GUI(
            width,
            height,
            fontFilepath,
            characterSet,
            localizationFilepath,
            vectorGraphicsDPI,
            fontTallSize,
            fontMediumSize,
            fontSmallSize,
            descriptionFontSize,
            resizeInvisibleLayouts);
    }

    Layout* addLayout(GUI* pGUI, std::string filepath, int layer, bool visible)
    {
        return pGUI->addLayout(filepath, layer, visible);
    }

    void removeLayout(GUI* pGUI, Layout const * pLayout)
    {
        pGUI->removeLayout(pLayout);
    }

    void updateGUI(GUI* pGUI)
    {
        pGUI->update();
    }

    Input updateGUI(GUI* pGUI, float tpf, const Input input)
    {
        return pGUI->update(tpf, input);
    }

    void drawGUI(GUI const * pGUI)
    {
        pGUI->draw();
    }

    void terminateGUI(GUI* pGUI)
    {
        if (pGUI != NULL)
        {
            delete pGUI;
            pGUI = NULL;
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "GUI was tried to terminate but is NULL pointer");
        }
    }

    void resizeGUI(GUI* pGUI, int width, int height)
    {
        pGUI->resize(width, height);
    }

    void loadStyleSheet(GUI* pGUI, std::string filepath)
    {
		pGUI->loadStyleSheet(filepath);
    }

	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, StylePropertyFloat stylePropertyType, std::string value)
	{
		pGUI->setStyleTreePropertyValue(styleClass, stylePropertyType, value);
	}

	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, StylePropertyVec4 stylePropertyType, std::string value)
	{
		pGUI->setStyleTreePropertyValue(styleClass, stylePropertyType, value);
	}

    void setGazeVisualizationDrawing(GUI* pGUI, bool draw)
    {
        pGUI->setGazeVisualizationDrawing(draw);
    }

    void toggleGazeVisualizationDrawing(GUI* pGUI)
    {
        pGUI->toggleGazeVisualizationDrawing();
    }

    void setDescriptionVisibility(GUI* pGUI, DescriptionVisibility visbility)
    {
        pGUI->setDescriptionVisibility(visbility);
    }

    void prefetchImage(GUI* pGUI, std::string filepath)
    {
        pGUI->prefetchImage(filepath);
    }

	std::u16string fetchLocalization(GUI const * pGUI, std::string key)
	{
		return pGUI->getContentFromLocalization(key);
	}

    unsigned int addDictionary(GUI* pGUI, std::string filepath)
    {
        return pGUI->addDictionary(filepath);
    }

    void moveLayoutToFront(GUI* pGUI, Layout* pLayout)
    {
        pGUI->moveLayoutToFront(pLayout);
    }

    void moveLayoutToBack(GUI* pGUI, Layout* pLayout)
    {
        pGUI->moveLayoutToBack(pLayout);
    }

    void setInputUsageOfLayout(Layout* pLayout, bool useInput)
    {
        pLayout->useInput(useInput);
    }

    void setVisibilityOfLayout(Layout* pLayout, bool visible, bool reset, bool fade)
    {
        pLayout->setVisibility(visible, fade);

        if (reset)
        {
            pLayout->resetElements();
        }
    }

    RelativePositionAndSize getRelativePositionAndSizeOfElement(
        Layout* pLayout,
        std::string id)
    {
        return pLayout->getRelativePositionAndSizeOfElement(id);
    }

    AbsolutePositionAndSize getAbsolutePositionAndSizeOfElement(
        Layout* pLayout,
        std::string id)
    {
        return pLayout->getAbsolutePositionAndSizeOfElement(id);
    }

	void setElementStylePropertyValue(Layout* pLayout, std::string id, StylePropertyFloat stylePropertyType, std::string value)
	{
		pLayout->setElementStylePropertyValue(id, stylePropertyType, value);
	}

	void setElementStylePropertyValue(Layout* pLayout, std::string id, StylePropertyVec4 stylePropertyType, std::string value)
	{
		pLayout->setElementStylePropertyValue(id, stylePropertyType, value);
	}

    void setElementActivity(Layout* pLayout, std::string id, bool active, bool fade)
    {
        pLayout->setElementActivity(id, active, fade);
    }

    void toggleElementActivity(Layout* pLayout, std::string id, bool fade)
    {
        pLayout->setElementActivity(id, !pLayout->isElementActive(id), fade);
    }

    bool isElementActive(Layout const * pLayout, std::string id)
    {
        return pLayout->isElementActive(id);
    }

    void setElementDimming(
        Layout* pLayout,
        std::string id,
        bool dimming)
    {
        pLayout->setElementDimming(id, dimming);
    }

    void setElementMarking(
        Layout* pLayout,
        std::string id,
        bool marking,
        int depth)
    {
        pLayout->setElementMarking(id, marking, depth);
    }

    bool isElementDimming(Layout const * pLayout, std::string id)
    {
        return pLayout->isElementDimming(id);
    }

    bool isElementMarking(Layout const * pLayout, std::string id)
    {
        return pLayout->isElementMarking(id);
    }

    void setElementHiding(Layout* pLayout, std::string id, bool hidden)
    {
        pLayout->setElementHiding(id, hidden);
    }

    bool checkForId(Layout const * pLayout, std::string id)
    {
        return pLayout->checkForId(id);
    }

	void flash(Layout* pLayout, std::string id)
	{
		pLayout->flash(id);
	}

    void highlightInteractiveElement(Layout* pLayout, std::string id, bool doHighlight)
    {
        pLayout->highlightInteractiveElement(id, doHighlight);
    }

    void toggleHighlightInteractiveElement(Layout* pLayout, std::string id)
    {
        pLayout->highlightInteractiveElement(id, pLayout->isInteractiveElementHighlighted(id));
    }

    bool isInteractiveElementHighlighted(Layout const * pLayout, std::string id)
    {
        return pLayout->isInteractiveElementHighlighted(id);
    }

    void setIconOfIconElement(Layout* pLayout, std::string id, std::string iconFilepath)
    {
        pLayout->setIconOfIconElement(id, iconFilepath);
    }

    void setIconOfIconElement(
        Layout* pLayout,
        std::string id,
        std::string name,
        int width,
        int height,
        ColorFormat format,
        unsigned char const * pIconData,
        bool flipY)
    {
        pLayout->setIconOfIconElement(id, name, width, height, format, pIconData, flipY);
    }

    void setImageOfPicture(
        Layout* pLayout,
        std::string id,
        std::string name,
        int width,
        int height,
        ColorFormat format,
        unsigned char const * pData,
        bool flipY)
    {
        pLayout->setImageOfPicture(id, name, width, height, format, pData, flipY);
    }

	void setImageOfPicture(
		Layout* pLayout,
		std::string id,
		std::string name)
	{
		pLayout->setImageOfPicture(id, name);
	}

	void fetchImage(
		Layout* pLayout,
		std::string name,
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pData,
		bool flipY)
	{
		pLayout->fetchImage(name, width, height, format, pData, flipY);
	}

    void interactWithInteractiveElement(Layout* pLayout, std::string id)
    {
        pLayout->interactWithInteractiveElement(id);
    }

    void selectInteractiveElement(Layout* pLayout, std::string id)
    {
        pLayout->selectInteractiveElement(id);
    }

    void deselectInteractiveElement(Layout* pLayout)
    {
        pLayout->deselectInteractiveElement();
    }

    void interactWithSelectedInteractiveElement(Layout* pLayout)
    {
        pLayout->interactWithSelectedInteractiveElement();
    }

    bool selectNextInteractiveElement(Layout* pLayout)
    {
        return pLayout->selectNextInteractiveElement();
    }

    void hitButton(Layout* pLayout, std::string id)
    {
        pLayout->hitButton(id);
    }

    void buttonDown(Layout* pLayout, std::string id, bool immediately)
    {
        pLayout->buttonDown(id, immediately);
    }

    void buttonUp(Layout* pLayout, std::string id, bool immediately)
    {
        pLayout->buttonUp(id, immediately);
    }

    bool isButtonSwitch(Layout const * pLayout, std::string id)
    {
        return pLayout->isButtonSwitch(id);
    }

    void penetrateSensor(Layout* pLayout, std::string id, float amount)
    {
        pLayout->penetrateSensor(id, amount);
    }

    void setContentOfTextBlock(Layout* pLayout, std::string id, std::u16string content)
    {
        pLayout->setContentOfTextBlock(id, content);
    }

    void setContentOfTextBlock(Layout* pLayout, std::string id, std::string content)
    {
        pLayout->setContentOfTextBlock(id, content);
    }

    void setKeyOfTextBlock(Layout* pLayout, std::string id, std::string key)
    {
        pLayout->setKeyOfTextBlock(id, key);
    }

    void setFastTypingOfKeyboard(Layout* pLayout, std::string id, bool useFastTyping)
    {
        pLayout->setFastTypingOfKeyboard(id, useFastTyping);
    }

    void setCaseOfKeyboard(Layout* pLayout, std::string id, KeyboardCase keyboardCase)
    {
        pLayout->setCaseOfKeyboard(id, keyboardCase);
    }

    unsigned int getCountOfKeymapsInKeyboard(Layout const * pLayout, std::string id)
    {
        return pLayout->getCountOfKeymapsInKeyboard(id);
    }

    void setKeymapOfKeyboard(Layout* pLayout, std::string id, unsigned int keymapIndex)
    {
        pLayout->setKeymapOfKeyboard(id, keymapIndex);
    }

	void classifyKey(Layout* pLayout, std::string id, bool accept)
	{
		pLayout->classifyKey(id, accept);
	}

    void suggestWords(Layout* pLayout, std::string id, std::u16string input, unsigned int dictionaryIndex)
    {
        std::u16string bestSuggestion;
        pLayout->suggestWords(id, input, dictionaryIndex, bestSuggestion);
    }

    void suggestWords(Layout* pLayout, std::string id, std::string input, unsigned int dictionaryIndex)
    {
        std::string bestSuggestion;
        pLayout->suggestWords(id, input, dictionaryIndex, bestSuggestion);
    }

    void suggestWords(Layout* pLayout, std::string id, std::u16string input, unsigned int dictionaryIndex, std::u16string& rBestSuggestion)
    {
        pLayout->suggestWords(id, input, dictionaryIndex, rBestSuggestion);
    }

    void suggestWords(Layout* pLayout, std::string id, std::string input, unsigned int dictionaryIndex, std::string& rBestSuggestion)
    {
        pLayout->suggestWords(id, input, dictionaryIndex, rBestSuggestion);
    }

    void clearSuggestions(Layout* pLayout, std::string id)
    {
        pLayout->clearSuggestions(id);
    }

    void setSpaceOfFlow(Layout* pLayout, std::string id, float space)
    {
        pLayout->setSpaceOfFlow(id, space);
    }

    void setProgress(Layout *pLayout, std::string id, float progress)
    {
        pLayout->setProgress(id, progress);
    }

	void moveCursorOverLettersInTextEdit(Layout* pLayout, std::string id, int letterCount)
	{
		pLayout->moveCursorOverLettersInTextEdit(id, letterCount);
	}

	void moveCursorOverWordsInTextEdit(Layout* pLayout, std::string id, int wordCount)
	{
		pLayout->moveCursorOverWordsInTextEdit(id, wordCount);
	}

    void moveCursorToStartInTextEdit(Layout* pLayout, std::string id)
    {
        pLayout->moveCursorToStartInTextEdit(id);
    }

    void moveCursorToEndInTextEdit(Layout* pLayout, std::string id)
    {
        pLayout->moveCursorToEndInTextEdit(id);
    }

	void addContentAtCursorInTextEdit(Layout* pLayout, std::string id, std::u16string content)
	{
		pLayout->addContentAtCursorInTextEdit(id, content);
	}

	void setContentOfTextEdit(Layout* pLayout, std::string id, std::u16string content)
	{
		pLayout->setContentOfTextEdit(id, content);
	}

	std::u16string getContentOfTextEdit(Layout const * pLayout, std::string id)
	{
		return pLayout->getContentOfTextEdit(id);
	}

	void deleteContentAtCursorInTextEdit(Layout* pLayout, std::string id, int letterCount)
	{
		pLayout->deleteContentAtCursorInTextEdit(id, letterCount);
	}

	std::u16string getActiveEntityContentInTextEdit(Layout const * pLayout, std::string id)
	{
		return pLayout->getActiveEntityContentInTextEdit(id);
	}

	void setActiveEntityContentInTextEdit(Layout* pLayout, std::string id, std::u16string content)
	{
		pLayout->setActiveEntityContentInTextEdit(id, content);
	}

    void addBrickToStack(
        Layout* pLayout,
        std::string id,
        std::string filepath)
    {
        // Delegate
        addBrickToStack(pLayout, id, filepath, std::map<std::string, std::string>());
    }

    void addBrickToStack(
        Layout* pLayout,
        std::string id,
        std::string filepath,
        std::map<std::string, std::string> idMapper)
    {
        pLayout->addBrickToStack(id, filepath, idMapper);
    }

    void registerButtonListener(Layout* pLayout, std::string id, std::weak_ptr<ButtonListener> wpListener)
    {
        pLayout->registerButtonListener(id, wpListener);
    }

    void registerSensorListener(Layout* pLayout, std::string id, std::weak_ptr<SensorListener> wpListener)
    {
        pLayout->registerSensorListener(id, wpListener);
    }

    void registerKeyboardListener(Layout* pLayout, std::string id, std::weak_ptr<KeyboardListener> wpListener)
    {
        pLayout->registerKeyboardListener(id, wpListener);
    }

    void registerWordSuggestListener(Layout* pLayout, std::string id, std::weak_ptr<WordSuggestListener> wpListener)
    {
        pLayout->registerWordSuggestListener(id, wpListener);
    }

    void replaceElementWithBrick(Layout* pLayout, std::string id, std::string filepath, bool fade)
    {
        // Delegate to other function with empty id mapper
        replaceElementWithBrick(pLayout, id, filepath, std::map<std::string, std::string>(), fade);
    }

    void replaceElementWithBrick(Layout* pLayout, std::string id, std::string filepath, std::map<std::string, std::string> idMapper, bool fade)
    {
        pLayout->replaceElementWithBrick(id, filepath, idMapper, fade);
    }

    unsigned int addFloatingFrameWithBrick(
        Layout* pLayout,
        std::string filepath,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY,
        bool visible,
        bool fade)
    {
        // Delegate to other function with empty id mapper
        return addFloatingFrameWithBrick(
            pLayout,
            filepath,
            relativePositionX,
            relativePositionY,
            relativeSizeX,
            relativeSizeY,
            std::map<std::string, std::string>(),
            visible,
            fade);
    }

    unsigned int addFloatingFrameWithBrick(
        Layout* pLayout,
        std::string filepath,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY,
        std::map<std::string, std::string> idMapper,
        bool visible,
        bool fade)
    {
        return pLayout->addFloatingFrameWithBrick(filepath, relativePositionX, relativePositionY, relativeSizeX, relativeSizeY, idMapper, visible, fade);
    }

    void setVisibilityOFloatingFrame(Layout* pLayout, unsigned int frameIndex, bool visible, bool reset, bool fade)
    {
        pLayout->setVisibiltyOfFloatingFrame(frameIndex, visible, fade);

        if (reset)
        {
            pLayout->resetFloatingFramesElements(frameIndex);
        }
    }

    void removeFloatingFrame(Layout* pLayout, unsigned int frameIndex, bool fade)
    {
        pLayout->removeFloatingFrame(frameIndex, fade);
    }

    void translateFloatingFrame(Layout* pLayout, unsigned int frameIndex, float translateX, float translateY)
    {
        pLayout->translateFloatingFrame(frameIndex, translateX, translateY);
    }

    void scaleFloatingFrame(Layout* pLayout, unsigned int frameIndex, float scaleX, float scaleY)
    {
        pLayout->scaleFloatingFrame(frameIndex, scaleX, scaleY);
    }

    void setPositionOfFloatingFrame(Layout* pLayout, unsigned int frameIndex, float relativePositionX, float relativePositionY)
    {
        pLayout->setPositionOfFloatingFrame(frameIndex, relativePositionX, relativePositionY);
    }

    void setSizeOfFloatingFrame(Layout* pLayout, unsigned int frameIndex, float relativeSizeX, float relativeSizeY)
    {
        pLayout->setSizeOfFloatingFrame(frameIndex, relativeSizeX, relativeSizeY);
    }

    void moveFloatingFrameToFront(Layout* pLayout, unsigned int frameIndex)
    {
        pLayout->moveFloatingFrameToFront(frameIndex);
    }

    void moveFloatingFrameToBack(Layout* pLayout, unsigned int frameIndex)
    {
        pLayout->moveFloatingFrameToBack(frameIndex);
    }

    RelativePositionAndSize getRelativePositionAndSizeOfFloatingFrame(
        Layout* pLayout,
        unsigned int frameIndex)
    {
        return pLayout->getRelativePositionAndSizeOfFloatingFrame(frameIndex);
    }

    AbsolutePositionAndSize getAbsolutePositionAndSizeOfFloatingFrame(
        Layout* pLayout,
        unsigned int frameIndex)
    {
        return pLayout->getAbsolutePositionAndSizeOfFloatingFrame(frameIndex);
    }

    void setErrorCallback(std::function<void(std::string)> callbackFunction)
    {
        OperationNotifier::setErrorCallback(callbackFunction);
    }

    void setWarningCallback(std::function<void(std::string)> callbackFunction)
    {
        OperationNotifier::setWarningCallback(callbackFunction);
    }

    void setInteractionCallback(
        std::function<
            void(
                std::string,
                std::string,
                std::string,
                std::string,
                std::string,
                std::string,
                std::string,
                std::string)> callbackFunction)
    {
        OperationNotifier::setInteractionCallback(callbackFunction);
    }

    void setResizeCallback(GUI* pGUI, std::function<void(int, int)> callbackFunction)
    {
        pGUI->setResizeCallback(callbackFunction);
    }

    std::string getLibraryVersion()
    {
        return VERSION_STRING;
    }

    void setRootFilepath(std::string rootFilepath)
    {
		PathBuilder::setRootFilepath(rootFilepath);
    }
}

namespace eyegui_helper
{
    bool convertUTF8ToUTF16(const std::string& rInput, std::u16string& rOutput)
    {
        return eyegui::convertUTF8ToUTF16(rInput, rOutput);
    }

    bool convertUTF16ToUTF8(const std::u16string& rInput, std::string& rOutput)
    {
        return eyegui::convertUTF16ToUTF8(rInput, rOutput);
    }

    bool loadImage(
        std::string filepath,
        std::vector<unsigned char>& rData,
        int& rWidth,
        int& rHeight,
        int& rChannelCount,
        bool flipY)
	{
        return eyegui::PixelTexture::loadImageFile(
            filepath,
            rData,
            rWidth,
            rHeight,
            rChannelCount,
            flipY);
	}
}

namespace eyegui_experimental
{
    void registerFutureKeyboardListener(
        eyegui::Layout* pLayout,
        std::string id,
        std::weak_ptr<FutureKeyboardListener> wpListener)
    {
        pLayout->registerFutureKeyboardListener(id, wpListener);
    }

	void setFutureKeyboardLineSuggestions(
		eyegui::Layout* pLayout,
		std::string id,
		std::u16string suggestionA,
		std::u16string suggestionB,
		std::u16string suggestionC)
	{
		pLayout->setFutureKeyboardLineSuggestions(id, suggestionA, suggestionB, suggestionC);
	}

    void setFutureKeySuggestion(
        eyegui::Layout* pLayout,
        std::string id,
        std::string keyId,
        std::u16string suggestion)
    {
        pLayout->setFutureKeySuggestion(id, keyId, suggestion);
    }

	std::u16string getFutureKeyboardContent(
		eyegui::Layout const * pLayout,
		std::string id)
	{
		return pLayout->getFutureKeyboardContent(id);
	}

	void nextFutureKeyboardSentence(
		eyegui::Layout* pLayout,
		std::string id,
		std::u16string sentence)
	{
		pLayout->nextFutureKeyboardSentence(id, sentence);
	}

	void clearFutureKeyboardPredisplay(
		eyegui::Layout* pLayout,
		std::string id)
	{
		pLayout->clearFutureKeyboardPredisplay(id);
	}
}
