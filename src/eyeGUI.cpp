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

// Version
static const std::string VERSION_STRING = "0.9";

namespace eyegui
{
    GUI* GUIBuilder::construct() const
    {
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

    void loadConfig(GUI* pGUI, std::string filepath)
    {
        pGUI->loadConfig(filepath);
    }

    void setGazeVisualizationDrawing(GUI* pGUI, bool draw)
    {
        pGUI->setGazeVisualizationDrawing(draw);
    }

    void toggleGazeVisualizationDrawing(GUI* pGUI)
    {
        pGUI->toggleGazeVisualizationDrawing();
    }

    void setShowDescriptions(GUI* pGUI, bool showDescriptions)
    {
        pGUI->setShowDescriptions(showDescriptions);
    }

    void prefetchImage(GUI* pGUI, std::string filepath)
    {
        pGUI->prefetchImage(filepath);
    }

    unsigned int addDictionary(GUI* pGUI, std::string filepath)
    {
        return pGUI->addDictionary(filepath);
    }

    void setValueOfConfigAttribute(
        GUI* pGUI,
        std::string attribute,
        std::string value)
    {
        pGUI->setValueOfConfigAttribute(attribute, value);
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

    void setStyleOfElement(
        Layout* pLayout,
        std::string id,
        std::string style)
    {
        pLayout->setStyleOfElement(id, style);
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

    void setValueOfStyleAttribute(
        Layout* pLayout,
        std::string styleName,
        std::string attribute,
        std::string value)
    {
        pLayout->setValueOfStyleAttribute(styleName, attribute, value);
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

    void replaceElementWithBlock(
        Layout* pLayout,
        std::string id,
        bool consumeInput,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        bool fade)
    {
        pLayout->replaceElementWithBlock(id, consumeInput, backgroundFilepath, backgroundAlignment, fade);
    }

    void replaceElementWithPicture(Layout* pLayout, std::string id, std::string filepath, ImageAlignment alignment, bool fade)
    {
        pLayout->replaceElementWithPicture(id, filepath, alignment, fade);
    }

    void replaceElementWithBlank(Layout* pLayout, std::string id, bool fade)
    {
        pLayout->replaceElementWithBlank(id, fade);
    }

    void replaceElementWithCircleButton(
        Layout* pLayout,
        std::string id,
        std::string iconFilepath,
        std::u16string desc,
        std::string descKey,
        bool isSwitch,
        bool fade)
    {
        pLayout->replaceElementWithCircleButton(id, iconFilepath, desc, descKey, isSwitch, fade);
    }

    void replaceElementWithBoxButton(
        Layout* pLayout,
        std::string id,
        std::string
        iconFilepath,
        std::u16string desc,
        std::string descKey,
        bool isSwitch,
        bool fade)
    {
        pLayout->replaceElementWithBoxButton(id, iconFilepath, desc, descKey, isSwitch, fade);
    }

    void replaceElementWithSensor(
        Layout* pLayout,
        std::string id,
        std::string iconFilepath,
        std::u16string desc,
        std::string descKey,
        bool fade)
    {
        pLayout->replaceElementWithSensor(id, iconFilepath, desc, descKey, fade);
    }

    void replaceElementWithTextBlock(
        Layout* pLayout,
        std::string id,
        bool consumeInput,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content,
        float innerBorder,
        float textScale,
        std::string key,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        bool fade)
    {
        pLayout->replaceElementWithTextBlock(
            id,
            consumeInput,
            backgroundFilepath,
            backgroundAlignment,
            fontSize,
            alignment,
            verticalAlignment,
            textScale,
            content,
            innerBorder,
            key,
            fade);
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
        // Set root filepath as friend of path builder
        PathBuilder::rootFilepath = rootFilepath;
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
}