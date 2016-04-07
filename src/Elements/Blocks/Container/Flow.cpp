//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Flow.h"

#include "src/Rendering/ScissorStack.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
    Flow::Flow(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
        bool consumeInput,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        float innerBorder,
        bool showBackground,
        float space) : Container(
            id,
            styleName,
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
            showBackground)
    {
        mType = Type::FLOW;

        // Members
        mSpace = space;
        mOffset.setValue(0);
    }

    Flow::~Flow()
    {
        // Do nothing
    }

    void Flow::attachInnerElement(std::unique_ptr<Element> upElement)
    {
        // Assume the best, an empty children vector
        mChildren.push_back(std::move(upElement));
    }

    float Flow::specialUpdate(float tpf, Input* pInput)
    {
        // Only give input to child when gaze upen element
        Input* childInput = NULL;
        if(penetratedByInput(pInput))
        {
            childInput = pInput;

            // Update offset
            int y = pInput->gazeY - mY;
            float oldValue = mOffset.getValue();
            if(y > (mHeight / 2))
            {
                mOffset.update(tpf);
            }
            else
            {
                mOffset.update(-tpf);
            }

            if(oldValue != mOffset.getValue())
            {
                // Call transform and resize on child aka inner element
                transformInnerElement();
            }
        }

        // Call super with it (which calls it on child)
        return Container::specialUpdate(tpf, childInput);
    }

    void Flow::specialTransformAndSize()
    {
        // Super call
        Container::specialTransformAndSize();

        // Inner element too
        transformInnerElement();
    }

    void Flow::drawOnTop() const
    {
        // Push scissor to render only in allowed area
        pushScissor(mX, mY, mWidth, mHeight);

        // Super call that draws children
        Container::drawOnTop();

        popScissor();
    }

    void Flow::transformInnerElement()
    {
        // Transform and size child
        int height = mHeight * mSpace; // TODO: make it somehow adaptive to content...

        if(mHeight < height)
        {
            // Scrolling necessary
            mChildren.at(0)->transformAndSize(mX, mY - (mOffset.getValue() * (height - mHeight)), mWidth, height);
        }
        else
        {
            // No scrolling necessary
            mChildren.at(0)->transformAndSize(mX, mY, mWidth, height);
        }
    }
}
