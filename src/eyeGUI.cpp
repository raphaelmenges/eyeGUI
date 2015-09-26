//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "eyeGUI.h"

#include "GUI.h"
#include "Layout.h"
#include "OperationNotifier.h"

// Version
static const std::string VERSION_STRING = "0.1";

namespace eyegui
{
    GUI* createGUI(int width, int height)
    {
        return (new GUI(width, height));
    }

    Layout* addLayout(GUI* pGUI, std::string filepath, bool visible)
    {
        return pGUI->addLayout(filepath, visible);
    }

    void renderGUI(GUI* pGUI, float tpf)
    {
        pGUI->render(tpf);
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

    void setMouseCursor(GUI* pGUI, int x, int y)
    {
        pGUI->setMouseCursor(x, y);
    }

    void prefetchImage(GUI* pGUI, std::string filepath)
    {
        pGUI->prefetchImage(filepath);
    }

    void setInputUsageOfLayout(Layout* pLayout, bool useInput)
    {
        pLayout->useInput(useInput);
    }

    void setVisibilityOfLayout(Layout* pLayout, bool visible, bool reset, bool setImmediately)
    {
        pLayout->setVisibility(visible, setImmediately);

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

    void setElementActivity(Layout* pLayout, std::string id, bool active, bool setImmediately)
    {
        pLayout->setElementActivity(id, active, setImmediately);
    }

    void toggleElementActivity(Layout* pLayout, std::string id, bool setImmediately)
    {
        pLayout->setElementActivity(id, !pLayout->isElementActive(id), setImmediately);
    }

    bool isElementActive(Layout const * pLayout, std::string id)
    {
        return pLayout->isElementActive(id);
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

    void buttonDown(Layout* pLayout, std::string id)
    {
        pLayout->buttonDown(id);
    }

    void buttonUp(Layout* pLayout, std::string id)
    {
        pLayout->buttonUp(id);
    }

    bool isButtonSwitch(Layout const * pLayout, std::string id)
    {
        return pLayout->isButtonSwitch(id);
    }

    void penetrateSensor(Layout* pLayout, std::string id, float amount)
    {
        pLayout->penetrateSensor(id, amount);
    }

    void registerButtonListener(Layout* pLayout, std::string id, std::weak_ptr<ButtonListener> wpListener)
    {
        pLayout->registerButtonListener(id, wpListener);
    }

    void registerSensorListener(Layout* pLayout, std::string id, std::weak_ptr<SensorListener> wpListener)
    {
        pLayout->registerSensorListener(id, wpListener);
    }

    void replaceElementWithBlock(Layout* pLayout, std::string id, bool fade)
    {
        pLayout->replaceElementWithBlock(id, fade);
    }

    void replaceElementWithPicture(Layout* pLayout, std::string id, bool fade, std::string filepath, PictureAlignment alignment)
    {
        pLayout->replaceElementWithPicture(id, fade, filepath, alignment);
    }

    void replaceElementWithBlank(Layout* pLayout, std::string id, bool fade)
    {
        pLayout->replaceElementWithBlank(id, fade);
    }

    void replaceElementWithCircleButton(Layout* pLayout, std::string id, bool fade, std::string iconFilepath, bool isSwitch)
    {
        pLayout->replaceElementWithCircleButton(id, fade, iconFilepath, isSwitch);
    }

    void replaceElementWitBoxButton(Layout* pLayout, std::string id, bool fade, std::string iconFilepath, bool isSwitch)
    {
        pLayout->replaceElementWitBoxButton(id, fade, iconFilepath, isSwitch);
    }

    void replaceElementWitSensor(Layout* pLayout, std::string id, bool fade, std::string iconFilepath)
    {
        pLayout->replaceElementWitSensor(id, fade, iconFilepath);
    }

    void replaceElementWithBrick(Layout* pLayout, std::string id, bool fade, std::string filepath)
    {
        pLayout->replaceElementWithBrick(id, fade, filepath);
    }

    unsigned int addFloatingFrameWithBrick(
        Layout* pLayout,
        std::string filepath,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY,
        bool fade,
        bool visible)
    {
        // TODO: visibilty / fading
        return pLayout->addFloatingFrameWithBrick(filepath, relativePositionX, relativePositionY, relativeSizeX, relativeSizeY, fade, visible);
    }

    void setVisibilityOFloatingfFrame(Layout* pLayout, unsigned int frameIndex, bool visible, bool reset, bool setImmediately)
    {
        pLayout->setVisibiltyOfFloatingFrame(frameIndex, visible, setImmediately);

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
}
