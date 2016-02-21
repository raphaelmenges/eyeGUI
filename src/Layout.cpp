//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Layout.h"

#include "GUI.h"
#include "Helper.h"
#include "Elements/ElementCasting.h"
#include "Defines.h"
#include "OperationNotifier.h"
#include "externals/utfcpp/source/utf8.h"

#include <algorithm>

namespace eyegui
{
    Layout::Layout(GUI const * pGUI, AssetManager* pAssetManager, std::string stylesheetFilepath)
    {
        // Initialize members
        mpGUI = pGUI;
        mpAssetManager = pAssetManager;
        mupIds = NULL;
        mAlpha.setValue(1);
        mVisible = true;
        mResizeNecessary = true;
        mUseInput = true;
        mpSelectedInteractiveElement = NULL;
        mupMainFrame = std::unique_ptr<Frame>(new Frame(this, 0, 0, 1, 1));
        mupNotificationQueue = std::unique_ptr<NotificationQueue>(new NotificationQueue(this));

        // Parse style file
        mupStyles = stylesheet_parser::parse(stylesheetFilepath);
    }

    Layout::~Layout()
    {
        // Nothing to do
    }

    void Layout::update(float tpf, Input* pInput)
    {
        // *** DELETION OF REMOVED FLOATING FRAMES ***
        for (int i : mDyingFloatingFramesIndices)
        {
            mFloatingFramesOrderingIndices.erase(
                std::remove(
                    mFloatingFramesOrderingIndices.begin(), mFloatingFramesOrderingIndices.end(), i), mFloatingFramesOrderingIndices.end());
            mFloatingFrames[i].reset(NULL);
        }
        mDyingFloatingFramesIndices.clear();

        // *** RESIZING ***
        internalResizing();

        // *** NOTIFICATIONS ***
        mupNotificationQueue->process();

        // *** OWN UPDATE ***

        // Update alpha
        mAlpha.update(tpf / getConfig()->animationDuration, !mVisible);

        // *** UPDATE FRAMES ***

        // Update root only if own alpha greater zero
        if (mAlpha.getValue() > 0)
        {
            // Do not use input if still fading
            if (!mUseInput || mAlpha.getValue() < 1)
            {
                pInput = NULL;
            }

            // Update floating frames
            for (int i = (int)(mFloatingFramesOrderingIndices.size()) - 1; i >= 0; i--)
            {
                // Update last added first
                int frameIndex = mFloatingFramesOrderingIndices[i];
                Frame* pFrame = mFloatingFrames[frameIndex].get();
                if (pFrame != NULL)
                {
                    if (pFrame->isRemoved())
                    {
                        // Do fading of removed frame
                        float fadingAlpha = pFrame->getRemovedFadingAlpha() - (tpf / getConfig()->animationDuration);
                        fadingAlpha = clamp(fadingAlpha, 0, 1);
                        pFrame->setRemovedFadingAlpha(fadingAlpha);

                        // Update
                        pFrame->update(tpf, mAlpha.getValue(), NULL);

                        // Delete frame in next update
                        if (fadingAlpha <= 0)
                        {
                            mDyingFloatingFramesIndices.push_back(frameIndex);
                        }
                    }
                    else
                    {
                        // Standard update
                        pFrame->update(tpf, mAlpha.getValue(), pInput);
                    }
                }
            }

            // Update main frame
            mupMainFrame->update(tpf, mAlpha.getValue(), pInput);
        }
    }

    void Layout::draw() const
    {
        // Use alpha because while fading it should still draw
        if (mAlpha.getValue() > 0)
        {
            // Draw main frame
            mupMainFrame->draw();

            // Draw floating frames
            for (int i = 0; i < mFloatingFramesOrderingIndices.size(); i++)
            {
                Frame* pFrame = mFloatingFrames[mFloatingFramesOrderingIndices[i]].get();
                if (pFrame != NULL)
                {
                    pFrame->draw();
                }
            }
        }
    }

    void Layout::makeResizeNecessary()
    {
        mResizeNecessary = true;
    }

    void Layout::attachElementToMainFrameAsRoot(
        std::unique_ptr<Element> upElement,
        std::unique_ptr<std::map<std::string, Element*> > upIds)
    {
        // Set root element of main frame
        mupMainFrame->attachRoot(std::move(upElement));

        // Keep track of ids
        mupIds = std::move(upIds);
    }

    NotificationQueue* Layout::getNotificationQueue() const
    {
        return mupNotificationQueue.get();
    }

    Config const * Layout::getConfig() const
    {
        return mpGUI->getConfig();
    }

    Frame* Layout::getMainFrame()
    {
        return mupMainFrame.get();
    }

    uint Layout::getLayoutWidth() const
    {
        return mpGUI->getWindowWidth();
    }

    uint Layout::getLayoutHeight() const
    {
        return mpGUI->getWindowHeight();
    }

    float Layout::getAccPeriodicTime() const
    {
        return mpGUI->getAccPeriodicTime();
    }

    Style const * Layout::getStyleFromStylesheet(std::string styleName) const
    {
        // Search for style
        auto it = mupStyles->find(styleName);

        if (it != mupStyles->end())
        {
            return &(it->second);
        }
        else
        {
            return NULL;
        }
    }

    std::u16string Layout::getContentFromLocalization(std::string key) const
    {
        return mpGUI->getContentFromLocalization(key);
    }

    std::set<std::string> Layout::getNamesOfAvailableStyles() const
    {
        std::set<std::string> names;
        for (const std::pair<std::string, Style>& pair : *(mupStyles.get()))
        {
            names.insert(pair.first);
        }

        return names;
    }

    void Layout::setVisibility(bool visible, bool fade)
    {
        mVisible = visible;

        if (!fade)
        {
            if (mVisible)
            {
                mAlpha.setValue(1);
            }
            else
            {
                mAlpha.setValue(0);
            }
        }
    }

    void Layout::useInput(bool useInput)
    {
        mUseInput = useInput;
    }

    RelativePositionAndSize Layout::getRelativePositionAndSizeOfElement(std::string id) const
    {
        RelativePositionAndSize result;
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            result.x = pElement->getRelativePositionOnLayoutX();
            result.y = pElement->getRelativePositionOnLayoutY();
            result.width = pElement->getRelativeSizeOnLayoutX();
            result.height = pElement->getRelativeSizeOnLayoutY();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
        return result;
    }

    AbsolutePositionAndSize Layout::getAbsolutePositionAndSizeOfElement(std::string id) const
    {
        AbsolutePositionAndSize result;
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            result.x = pElement->getX();
            result.y = pElement->getY();
            result.width = pElement->getWidth();
            result.height = pElement->getHeight();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
        return result;
    }

    void Layout::setElementActivity(std::string id, bool active, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            pElement->setActivity(active, fade);

            if (!active && mpSelectedInteractiveElement == pElement)
            {
                // If selected element goes inactive, deselect it
                deselectInteractiveElement();
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    bool Layout::isElementActive(std::string id) const
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            return pElement->isActive();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
        return false;
    }

    void Layout::setElementDimming(std::string id, bool dimming)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            pElement->setDimming(dimming);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::setElementMarking(std::string id, bool marking, int depth)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            pElement->setMarking(marking, depth);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::setElementHiding(std::string id, bool hidden)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            pElement->setHiding(hidden);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::setStyleOfElement(std::string id, std::string style)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            pElement->setStyle(style);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    bool Layout::isElementDimming(std::string id) const
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            return pElement->isDimming();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
        return false;
    }

    bool Layout::isElementMarking(std::string id) const
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            return pElement->isMarking();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
        return false;
    }

    void Layout::highlightInteractiveElement(std::string id, bool doHighlight)
    {
        InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
        if (pInteractiveElement != NULL)
        {
            pInteractiveElement->highlight(doHighlight);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
        }
    }

    void Layout::setIconOfIconInteractiveElement(std::string id, std::string iconFilepath)
    {
        IconInteractiveElement* pIconInteractiveElement = toIconInteractiveElement(fetchElement(id));
        if (pIconInteractiveElement != NULL)
        {
			pIconInteractiveElement->setIcon(iconFilepath);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
        }
    }

    void Layout::interactWithInteractiveElement(std::string id)
    {
        InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
        if (pInteractiveElement != NULL)
        {
            pInteractiveElement->interact();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
        }
    }

    void Layout::hitButton(std::string id)
    {
        Button* pButton = toButton(fetchElement(id));
        if (pButton != NULL)
        {
            return pButton->hit();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
        }
    }

    void Layout::buttonDown(std::string id, bool immediately)
    {
        Button* pButton = toButton(fetchElement(id));
        if (pButton != NULL)
        {
            return pButton->down(immediately);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
        }
    }

    void Layout::buttonUp(std::string id, bool immediately)
    {
        Button* pButton = toButton(fetchElement(id));
        if (pButton != NULL)
        {
            return pButton->up(immediately);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
        }
    }

    bool Layout::isButtonSwitch(std::string id) const
    {
        Button* pButton = toButton(fetchElement(id));
        if (pButton != NULL)
        {
            return pButton->isSwitch();
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
        }
        return false;
    }

    void Layout::penetrateSensor(std::string id, float amount)
    {
        Sensor* pSensor = toSensor(fetchElement(id));
        if (pSensor != NULL)
        {
            pSensor->penetrate(amount);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find sensor with id: " + id);
        }
    }

    void Layout::setContentOfTextBlock(std::string id, std::u16string content)
    {
        TextBlock* pTextBlock = toTextBlock(fetchElement(id));
        if (pTextBlock != NULL)
        {
            pTextBlock->setContent(content);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find text block with id: " + id);
        }
    }

    void Layout::setContentOfTextBlock(std::string id, std::string content)
    {
        // Check for valid UTF-8
        if(utf8::is_valid(content.begin(), content.end()))
        {
            // Convert to 16 bit string
            std::u16string content16;
            utf8::utf8to16(content.begin(), content.end(), back_inserter(content16));

            // Pipe it to method for 16 bit strings
            setContentOfTextBlock(id, content16);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Invalid characters found in content. Please check unicode encoding of your source code. Following string was received: " + content);
        }
    }

    void Layout::setKeyOfTextBlock(std::string id, std::string key)
    {
        TextBlock* pTextBlock = toTextBlock(fetchElement(id));
        if (pTextBlock != NULL)
        {
            pTextBlock->setKey(key);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find text block with id: " + id);
        }
    }

    void Layout::registerButtonListener(std::string id, std::weak_ptr<ButtonListener> wpListener)
    {
        Button* pButton = toButton(fetchElement(id));
        if (pButton != NULL)
        {
            pButton->registerListener(wpListener);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find button with id: " + id);
        }
    }

    void Layout::registerSensorListener(std::string id, std::weak_ptr<SensorListener> wpListener)
    {
        Sensor* pSensor = toSensor(fetchElement(id));
        if (pSensor != NULL)
        {
            pSensor->registerListener(wpListener);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find sensor with id: " + id);
        }
    }

    void Layout::registerKeyboardListener(std::string id, std::weak_ptr<KeyboardListener> wpListener)
    {
        Keyboard* pKeyboard = toKeyboard(fetchElement(id));
        if (pKeyboard != NULL)
        {
            pKeyboard->registerListener(wpListener);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find keyboard with id: " + id);
        }
    }

    void Layout::selectInteractiveElement(std::string id)
    {
        InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
        if (pInteractiveElement != NULL)
        {
            selectInteractiveElement(pInteractiveElement);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find interactive element with id: " + id);
        }
    }

    bool Layout::selectInteractiveElement(InteractiveElement* pInteractiveElement)
    {
        // Deselect currently selected interactive element
        deselectInteractiveElement();

        // Only select next, if visible
        if (mAlpha.getValue() >= 1 && pInteractiveElement->getAlpha() > 0)
        {
            mpSelectedInteractiveElement = pInteractiveElement;
            mpSelectedInteractiveElement->select(true);
            return true;
        }
        else
        {
            return false;
        }
    }

    void Layout::deselectInteractiveElement()
    {
        if (mpSelectedInteractiveElement != NULL)
        {
            mpSelectedInteractiveElement->select(false);
            mpSelectedInteractiveElement = NULL;
        }
    }

    void Layout::interactWithSelectedInteractiveElement()
    {
        if (mpSelectedInteractiveElement != NULL && mAlpha.getValue() >= 1)
        {
            mpSelectedInteractiveElement->interact();
        }
    }

    bool Layout::selectNextInteractiveElement()
    {
        // Take a guess
        bool reachedEndOfLayout = true;

        // Pointer to next interactive element
        InteractiveElement* pNext = NULL;

        if (mpSelectedInteractiveElement == NULL)
        {
            // Nothing selected at the moment, so start at main frame
            pNext = mupMainFrame->getFirstInteractiveElement();
        }
        else
        {
            // Try to get next interactive element from that frame
            pNext = mpSelectedInteractiveElement->nextInteractiveElement();

            // No further interactive element found in current frame, try other
            if (pNext == NULL && mFloatingFramesOrderingIndices.size() > 0)
            {
                // Get the current frame
                Frame* pFrame = mpSelectedInteractiveElement->getFrame();

                // Find start frame in vector of ordered frames indices
                int indexOfStartFrameIndex = -1;
                if (pFrame == mupMainFrame.get())
                {
                    indexOfStartFrameIndex = 0;
                }
                else
                {
                    // Go over ordered frame indices and search for current frame
                    for (int i = 0; i < (int)(mFloatingFramesOrderingIndices.size()) - 1; i++)
                    {
                        Frame* pThatFrame = mFloatingFrames[mFloatingFramesOrderingIndices[i]].get();
                        if (pThatFrame == pFrame)
                        {
                            indexOfStartFrameIndex = i + 1;
                            break;
                        }
                    }
                }

                // Go over all remaining frame indices until interactive element found or null
                if (indexOfStartFrameIndex >= 0)
                {
                    for (int i = indexOfStartFrameIndex; i < mFloatingFramesOrderingIndices.size(); i++)
                    {
                        Frame* pNextFrame = mFloatingFrames[mFloatingFramesOrderingIndices[i]].get();
                        if (!pNextFrame->isRemoved())
                        {
                            pNext = pNextFrame->getFirstInteractiveElement();
                            if (pNext != NULL)
                            {
                                // Found interactive element!
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Only do something, if something was found
        if (pNext != NULL)
        {
            if (selectInteractiveElement(pNext))
            {
                reachedEndOfLayout = false;
            }
            else
            {
                // Some problem occurent selecting the next element
                deselectInteractiveElement();
            }
        }
        else
        {
            // No next element found
            deselectInteractiveElement();
        }

        return reachedEndOfLayout;
    }

    void Layout::resetElements()
    {
        mupMainFrame->resetElements();

        for (auto& upFrame : mFloatingFrames)
        {
            upFrame->resetElements();
        }
    }

    bool Layout::checkForId(std::string id) const
    {
        Element const* pElement = fetchElement(id);
        if (pElement == NULL)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool Layout::isInteractiveElementHighlighted(std::string id) const
    {
        InteractiveElement* pInteractiveElement = toInteractiveElement(fetchElement(id));
        if (pInteractiveElement != NULL)
        {
            return pInteractiveElement->isHighlighted();
        }
        else
        {

        }
        return false;
    }

    void Layout::setValueOfStyleAttribute(std::string styleName, std::string attribute, glm::vec4 value)
    {
        // Check, whether style exists
        auto it = mupStyles->find(styleName);

        if (it != mupStyles->end())
        {
            stylesheet_parser::fillValue(it->second, attribute, value);
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find style with name: " + styleName);
        }
    }

    void Layout::replaceElementWithBlock(
        std::string id,
        bool consumeInput,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new block
            std::unique_ptr<Block> upBlock = std::unique_ptr<Block>(new Block(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling(),
                consumeInput,
                backgroundFilepath,
                backgroundAlignment));
                // innerBorder is 0 by default and not necessary for block

            Element* pBlock = upBlock.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upBlock), fade))
            {
                insertId(pBlock);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithPicture(std::string id, std::string filepath, ImageAlignment alignment, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new picture
            std::unique_ptr<Picture> upPicture = std::unique_ptr<Picture>(new Picture(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling(),
                filepath,
                alignment));

            Element* pPicture = upPicture.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upPicture), fade))
            {
                insertId(pPicture);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithBlank(std::string id, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new blank
            std::unique_ptr<Blank> upBlank = std::unique_ptr<Blank>(new Blank(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling()));

            Element* pBlank = upBlank.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upBlank), fade))
            {
                insertId(pBlank);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithCircleButton(std::string id, std::string iconFilepath, bool isSwitch, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new picture
            std::unique_ptr<CircleButton> upCircleButton = std::unique_ptr<CircleButton>(new CircleButton(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling(),
                iconFilepath,
                isSwitch));

            Element* pCircleButton = upCircleButton.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upCircleButton), fade))
            {
                insertId(pCircleButton);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithBoxButton(std::string id, std::string iconFilepath, bool isSwitch, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new picture
            std::unique_ptr<BoxButton> upBoxButton = std::unique_ptr<BoxButton>(new BoxButton(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling(),
                iconFilepath,
                isSwitch));

            Element* pBoxButton = upBoxButton.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upBoxButton), fade))
            {
                insertId(pBoxButton);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithSensor(std::string id, std::string iconFilepath, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new picture
            std::unique_ptr<Sensor> upSensor = std::unique_ptr<Sensor>(new Sensor(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling(),
                iconFilepath));

            Element* pSensor = upSensor.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upSensor), fade))
            {
                insertId(pSensor);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithTextBlock(
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
        bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            // Create new text block
            std::unique_ptr<TextBlock> upTextBlock = std::unique_ptr<TextBlock>(new TextBlock(
                pElement->getId(),
                pElement->getStyleName(),
                pElement->getParent(),
                pElement->getLayout(),
                pElement->getFrame(),
                pElement->getAssetManager(),
                pElement->getNotificationQueue(),
                pElement->getRelativeScale(),
                pElement->getBorder(),
                pElement->isDimming(),
                pElement->getAdaptiveScaling(),
                consumeInput,
                backgroundFilepath,
                backgroundAlignment,
                innerBorder,
                fontSize,
                alignment,
                verticalAlignment,
				textScale,
                content,
                key));

            Element* pTextBlock = upTextBlock.get();

            // Replace target with it
            if (replaceElement(pElement, std::move(upTextBlock), fade))
            {
                insertId(pTextBlock);
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    void Layout::replaceElementWithBrick(std::string id, std::string filepath, std::map<std::string, std::string> idMapper, bool fade)
    {
        Element* pElement = fetchElement(id);
        if (pElement != NULL)
        {
            std::unique_ptr<elementsAndIds> upPair = std::move(
                brick_parser::parse(
                    pElement->getLayout(),
                    pElement->getFrame(),
                    pElement->getAssetManager(),
                    pElement->getNotificationQueue(),
                    pElement->getParent(),
                    filepath,
                    idMapper));
            if (replaceElement(pElement, std::move(upPair->first), fade))
            {
                insertIds(std::move(upPair->second));
            }
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find element with id: " + id);
        }
    }

    uint Layout::addFloatingFrameWithBrick(
        std::string filepath,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY,
        std::map<std::string, std::string> idMapper,
        bool visible,
        bool fade)
    {
        // Push back new frame
        auto upFrame = std::unique_ptr<Frame>(
            new Frame(
                this,
                relativePositionX,
                relativePositionY,
                relativeSizeX,
                relativeSizeY));
        Frame* pFrame = upFrame.get();

        // Go through floating frames and search for free place
        int freeIndex = -1;
        for (int i = 0; i < mFloatingFrames.size(); i++)
        {
            Frame* pFrame = mFloatingFrames[i].get();
            if (pFrame == NULL)
            {
                freeIndex = i;
                break;
            }
        }

        // Insert new floating frame
        int frameIndex = -1;
        if (freeIndex >= 0)
        {
            mFloatingFrames[freeIndex] = std::move(upFrame);
            frameIndex = freeIndex;
        }
        else
        {
            mFloatingFrames.push_back(std::move(upFrame));
            frameIndex = (int)(mFloatingFrames.size()) - 1;
        }

        // Push back index for updating and drawing
        mFloatingFramesOrderingIndices.push_back(frameIndex);

        // Create brick
        std::unique_ptr<elementsAndIds> upPair = std::move(
            brick_parser::parse(
                this,
                pFrame,
                mpAssetManager,
                mupNotificationQueue.get(),
                NULL,
                filepath,
                idMapper));

        // Attach elements to root of frame
        pFrame->attachRoot(std::move(upPair->first));

        // Set visibility
        pFrame->setVisibility(visible, !fade);

        // Insert ids
        insertIds(std::move(upPair->second));

        // Return index
        return frameIndex;
    }

    void Layout::setVisibiltyOfFloatingFrame(uint frameIndex, bool visible, bool fade)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            pFrame->setVisibility(visible, fade);
        }
    }

    void Layout::resetFloatingFramesElements(uint frameIndex)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            pFrame->resetElements();
        }
    }

    void Layout::removeFloatingFrame(uint frameIndex, bool fade)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            // Remove ids
            for (std::string id : pFrame->getAllElementsIds())
            {
                mupIds->erase(id);
            }

            // Reminder for removed frame
            pFrame->setRemoved();

            // Deselected element if it is in frame
            std::set<Element*> children = pFrame->getAllElements();
            for (Element* pElement : children)
            {
                if (mpSelectedInteractiveElement == pElement)
                {
                    deselectInteractiveElement();
                    break;
                }
            }

            // Delete frame in next update before drawing if no fading wished
            if (!fade)
            {
                mDyingFloatingFramesIndices.push_back(frameIndex);
            }
        }
    }

    void Layout::translateFloatingFrame(uint frameIndex, float translateX, float translateY)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            pFrame->translate(translateX, translateY);
        }
    }

    void Layout::scaleFloatingFrame(uint frameIndex, float scaleX, float scaleY)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            pFrame->scale(scaleX, scaleY);
        }
    }

    void Layout::setPositionOfFloatingFrame(uint frameIndex, float relativePositionX, float relativePositionY)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            pFrame->setPosition(relativePositionX, relativePositionY);
        }
    }

    void Layout::setSizeOfFloatingFrame(uint frameIndex, float relativeSizeX, float relativeSizeY)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            pFrame->setSize(relativeSizeX, relativeSizeY);
        }
    }

    void Layout::moveFloatingFrameToFront(uint frameIndex)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            // Search for it in sorted vector
            int index = -1;
            for (int i = 0; i < mFloatingFramesOrderingIndices.size(); i++)
            {
                if (mFloatingFrames[mFloatingFramesOrderingIndices[i]].get() == pFrame)
                {
                    index = i;
                    break;
                }
            }

            if (index < 0)
            {
                throwError(OperationNotifier::Operation::BUG, "Floating frame is owned by layout but not in ordered frame index vector");
            }

            moveFloatingFrame(index, (int)(mFloatingFramesOrderingIndices.size()) - 1);
        }
    }

    void Layout::moveFloatingFrameToBack(uint frameIndex)
    {
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            // Search for it in sorted vector
            int index = -1;
            for (int i = 0; i < mFloatingFramesOrderingIndices.size(); i++)
            {
                if (mFloatingFrames[mFloatingFramesOrderingIndices[i]].get() == pFrame)
                {
                    index = i;
                    break;
                }
            }

            if (index < 0)
            {
                throwError(OperationNotifier::Operation::BUG, "Floating frame is owned by layout but not in ordered frame index vector");
            }

            moveFloatingFrame(index, 0);
        }
    }

    RelativePositionAndSize Layout::getRelativePositionAndSizeOfFloatingFrame(unsigned int frameIndex) const
    {
        RelativePositionAndSize result;
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            result.x = pFrame->getRelativePositionOnLayoutX();
            result.y = pFrame->getRelativePositionOnLayoutY();
            result.width = pFrame->getRelativeSizeOnLayoutX();
            result.height = pFrame->getRelativeSizeOnLayoutY();
        }
        return result;
    }

    AbsolutePositionAndSize Layout::getAbsolutePositionAndSizeOfFloatingFrame(unsigned int frameIndex) const
    {
        AbsolutePositionAndSize result;
        Frame* pFrame = fetchFloatingFrame(frameIndex);
        if (pFrame != NULL)
        {
            result.x = pFrame->getX();
            result.y = pFrame->getY();
            result.width = pFrame->getWidth();
            result.height = pFrame->getHeight();
        }
        return result;
    }

    void Layout::internalResizing()
    {
        if (mResizeNecessary && mAlpha.getValue() > 0)
        {
            // Resize main frame
            mupMainFrame->makeResizeNecessary();

            // Resize floating frames
            for (auto& upFrame : mFloatingFrames)
            {
                Frame* pFrame = upFrame.get();
                if (pFrame != NULL)
                {
                    pFrame->makeResizeNecessary();
                }
            }
            mResizeNecessary = false;
        }
    }

    Element* Layout::fetchElement(std::string id) const
    {
        auto it = mupIds->find(id);

        if (it != mupIds->end())
        {
            return it->second;
        }
        else
        {
            return NULL;
        }
    }

    bool Layout::replaceElement(Element* pTarget, std::unique_ptr<Element> upElement, bool fade)
    {
        // Pointer to replacement
        Element* pElement = upElement.get();

        // Do the replacement
        std::unique_ptr<Element> upTarget;
        if (pTarget->getParent() != NULL)
        {
            // Fetch returned pointer
            upTarget = std::move(pTarget->getParent()->replaceAttachedElement(pTarget, std::move(upElement)));
        }
        else
        {
            // No parent? So must be root of a frame
            upTarget = std::move(pTarget->getFrame()->replaceRoot(std::move(upElement)));
        }

        // Check, whether element to be replaced was found and not yet replaced
        if (upTarget != NULL)
        {
            // Set activity
            pElement->setActivity(pTarget->isActive(), true);

            // Give new element replaced one for fading
            pElement->commitReplacedElement(std::move(upTarget), fade);

            // Some reminder to handle selection later
            bool shouldSelected = false;

            // Selection
            if (mpSelectedInteractiveElement == pTarget)
            {
                shouldSelected = true;
            }

            // Get pointers to children to determine whether child is selected
            if (!shouldSelected)
            {
                std::set<Element*> children = pTarget->getAllChildren();
                for (Element* pChild : children)
                {
                    if (mpSelectedInteractiveElement == pChild)
                    {
                        shouldSelected = true;
                        break;
                    }
                }
            }

            // Do selection
            if (shouldSelected)
            {
                InteractiveElement* pInteractiveElement = toInteractiveElement(pElement);
                if (pInteractiveElement != NULL)
                {
                    selectInteractiveElement(pInteractiveElement);
                }
                else
                {
                    // Just deselect
                    deselectInteractiveElement();
                }
            }

            // Remove front elements in frame
            pTarget->getFrame()->removeFrontElementsOfElement(pTarget);

            // Remove target and all children from id map
            if (pTarget->getId() != EMPTY_STRING_ATTRIBUTE)
            {
                mupIds->erase(pTarget->getId());
            }
            std::set<std::string> childrenIds = pTarget->getAllChildrensIds();
            for (std::string id : childrenIds)
            {
                mupIds->erase(id);
            }

            // Do resizing of whole frame
            pTarget->getFrame()->makeResizeNecessary();

            // Success
            return true;
        }

        return false;
    }

    void Layout::insertId(Element* pElement)
    {
        int idCount = (int)mupIds->size();

        if (pElement->getId() != EMPTY_STRING_ATTRIBUTE)
        {
            (*(mupIds.get()))[pElement->getId()] = pElement;
        }

        if (idCount == (int)mupIds->size())
        {
            throwError(OperationNotifier::Operation::RUNTIME, "Following id is not unique: " + pElement->getId());
        }
    }

    void Layout::insertIds(std::unique_ptr<idMap> upIdMap)
    {
        // Faster code, but not so good for debugging later applications
        /*int idCount = (int)(mupIds->size() + upIdMap->size());
        mupIds->insert(upIdMap->begin(), upIdMap->end());

        if (mupIds->size() != idCount)
        {
            throwError(OperationNotifier::Operation::RUNTIME, "Ids are no more unique");
        }*/

        // Loop over map und add every id
        for(const auto& rIdElement : *(upIdMap.get()))
        {
            int oldIdCount = (int)(mupIds->size());
            (*(mupIds.get()))[rIdElement.second->getId()] = rIdElement.second;

            // Check, whether id was really unique
            if(oldIdCount == (int)(mupIds->size()))
            {
                throwError(OperationNotifier::Operation::RUNTIME, "Following id is not unique: " + rIdElement.second->getId());
            }
        }
    }

    Frame* Layout::fetchFloatingFrame(uint frameIndex) const
    {
        Frame* pFrame = NULL;
        if (frameIndex < mFloatingFrames.size())
        {
            pFrame = mFloatingFrames[frameIndex].get();

            // Check whether frame was removed
            if (pFrame != NULL)
            {
                if (pFrame->isRemoved())
                {
                    pFrame = NULL;
                }
            }
        }

        if (pFrame == NULL)
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find floating frame with index: " + frameIndex);
        }

        return pFrame;
    }

    void Layout::moveFloatingFrame(int oldIndex, int newIndex)
    {
        // Move index of floating frame
        int movedFrameIndex = mFloatingFramesOrderingIndices[oldIndex];
        mFloatingFramesOrderingIndices.erase(mFloatingFramesOrderingIndices.begin() + oldIndex);

        if (newIndex >= mFloatingFramesOrderingIndices.size())
        {
            mFloatingFramesOrderingIndices.push_back(movedFrameIndex);
        }
        else
        {
            mFloatingFramesOrderingIndices.insert(mFloatingFramesOrderingIndices.begin() + newIndex, movedFrameIndex);
        }
    }
}
