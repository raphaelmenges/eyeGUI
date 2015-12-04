//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "eyeGUI.h"

#include "GUI.h"
#include "Layout.h"
#include "OperationNotifier.h"
#include "PathBuilder.h"

// Version
static const std::string VERSION_STRING = "0.5";

namespace eyegui
{
    GUI* createGUI(
        int width,
        int height,
        std::string fontFilepath,
        CharacterSet characterSet,
        std::string localizationFilepath)
    {
        return (new GUI(width, height, fontFilepath, characterSet, localizationFilepath));
    }

    Layout* addLayout(GUI* pGUI, std::string filepath, bool visible)
    {
        return pGUI->addLayout(filepath, visible);
    }

    void removeLayout(GUI* pGUI, Layout const * pLayout)
    {
        pGUI->removeLayout(pLayout);
    }

    Input updateGUI(GUI* pGUI, float tpf, Input input)
    {
        return pGUI->update(tpf, input);
    }

    void drawGUI(GUI* pGUI)
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
            throwWarning(OperationNotifier::Operation::RUNTIME, "GUI was tried to terminate but is already terminated");
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

    void prefetchImage(GUI* pGUI, std::string filepath)
    {
        pGUI->prefetchImage(filepath);
    }

    void setValueOfConfigAttribute(
        GUI* pGUI,
        std::string attribute,
        std::string value)
    {
        pGUI->setValueOfConfigAttribute(attribute, value);
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

    void moveLayoutToFront(GUI* pGUI, Layout* pLayout)
    {
        pGUI->moveLayoutToFront(pLayout);
    }

    void moveLayoutToBack(GUI* pGUI, Layout* pLayout)
    {
        pGUI->moveLayoutToBack(pLayout);
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

    void setElementDimmable(
        Layout* pLayout,
        std::string id,
        bool dimmable)
    {
        pLayout->setElementDimmable(id, dimmable);
    }

    void setStyleOfElement(
        Layout* pLayout,
        std::string id,
        std::string style)
    {
        pLayout->setStyleOfElement(id, style);
    }

    bool isElementDimmable(Layout const * pLayout, std::string id)
    {
        return pLayout->isElementDimmable(id);
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
        float r,
        float g,
        float b,
        float a)
    {
        pLayout->setValueOfStyleAttribute(styleName, attribute, glm::vec4(r, g, b, a));
    }

    void setIconOfInteractiveElement(Layout* pLayout, std::string id, std::string iconFilepath)
    {
        pLayout->setIconOfInteractiveElement(id, iconFilepath);
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

    void registerButtonListener(Layout* pLayout, std::string id, std::weak_ptr<ButtonListener> wpListener)
    {
        pLayout->registerButtonListener(id, wpListener);
    }

    void registerSensorListener(Layout* pLayout, std::string id, std::weak_ptr<SensorListener> wpListener)
    {
        pLayout->registerSensorListener(id, wpListener);
    }

    void replaceElementWithBlock(Layout* pLayout, std::string id, bool consumeInput, bool fade)
    {
        pLayout->replaceElementWithBlock(id, consumeInput, fade);
    }

    void replaceElementWithPicture(Layout* pLayout, std::string id, std::string filepath, PictureAlignment alignment, bool fade)
    {
        pLayout->replaceElementWithPicture(id, filepath, alignment, fade);
    }

    void replaceElementWithBlank(Layout* pLayout, std::string id, bool fade)
    {
        pLayout->replaceElementWithBlank(id, fade);
    }

    void replaceElementWithCircleButton(Layout* pLayout, std::string id, std::string iconFilepath, bool isSwitch, bool fade)
    {
        pLayout->replaceElementWithCircleButton(id, iconFilepath, isSwitch, fade);
    }

    void replaceElementWithBoxButton(Layout* pLayout, std::string id, std::string iconFilepath, bool isSwitch, bool fade)
    {
        pLayout->replaceElementWithBoxButton(id, iconFilepath, isSwitch, fade);
    }

    void replaceElementWithSensor(Layout* pLayout, std::string id, std::string iconFilepath, bool fade)
    {
        pLayout->replaceElementWithSensor(id, iconFilepath, fade);
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
        std::string key,
        bool fade)
    {
        pLayout->replaceElementWithTextBlock(id, consumeInput, fontSize, alignment, verticalAlignment, content, innerBorder, key, fade);
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

    void setVisibilityOFloatingfFrame(Layout* pLayout, unsigned int frameIndex, bool visible, bool reset, bool fade)
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

    void setErrorCallback(void(*pCallbackFunction)(std::string))
    {
        OperationNotifier::setErrorCallback(pCallbackFunction);
    }

    void setWarningCallback(void(*pCallbackFunction)(std::string))
    {
        OperationNotifier::setWarningCallback(pCallbackFunction);
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
