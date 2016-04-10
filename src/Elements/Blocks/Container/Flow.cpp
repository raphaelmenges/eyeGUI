//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Flow.h"

#include "src/Rendering/ScissorStack.h"
#include "src/Layout.h"

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
		FlowDirection direction,
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
		mDirection = direction;
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

	void Flow::setSpace(float space)
	{
		mSpace = space;
		transformInnerElement();
	}

    float Flow::specialUpdate(float tpf, Input* pInput)
    {
        // Only give input to child when gaze upen element
        Input* childInput = NULL;
        if(penetratedByInput(pInput))
        {
            childInput = pInput;

            // Update offset
			float offsetSpeed = 0;
            float oldValue = mOffset.getValue();
			if (mDirection == FlowDirection::VERTICAL)
			{
				// Vertical direction
				int y = pInput->gazeY - mY;
				offsetSpeed = ((float)(4 * (y - (mHeight / 2))) / (float)mHeight);
			}
			else
			{
				// Horizontal direction
				int x = pInput->gazeX - mX;
				offsetSpeed = ((float)(4 * (x - (mWidth / 2))) / (float)mWidth);
			}
            offsetSpeed *= 1.0f / mSpace; // Normalization
            mOffset.update(offsetSpeed * tpf * mpLayout->getConfig()->flowSpeedMultiplier);

            // Only transform inner element if necessary
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

    void Flow::drawChildren() const
    {
        // Push scissor to render only in allowed area
        pushScissor(mInnerX, mInnerY, mInnerWidth, mInnerHeight);

        // Super call that draws children
        Container::drawChildren();

        popScissor();
    }

    void Flow::transformInnerElement()
    {
		if (mDirection == FlowDirection::VERTICAL)
		{
			// Transform and size child
			int height = (int)((float)mInnerHeight * mSpace);

			// Evaluate used size
			int usedWidth, usedHeight;
			mChildren[0]->evaluateSize(
				mInnerWidth,
				height,
				usedWidth,
				usedHeight);

			// Delta
			int deltaX = (mWidth - usedWidth) / 2;
			int deltaY = (height - usedHeight) / 2;

			// Transform the one and only child
			if (mHeight < height)
			{
				// Scrolling necessary
				mChildren.at(0)->transformAndSize(
					mX + deltaX,
					mY + deltaY - (int)(mOffset.getValue() * (float)std::max(0, (usedHeight - mInnerHeight))),
					usedWidth,
					usedHeight);
			}
			else
			{
				// No scrolling necessary
				mChildren.at(0)->transformAndSize(
					mX + deltaX,
					mY + deltaY,
					usedWidth,
					usedHeight);
			}
		}
		else
		{
			// Transform and size child
			int width = (int)((float)mInnerWidth * mSpace);

			// Evaluate used size
			int usedWidth, usedHeight;
			mChildren[0]->evaluateSize(
				width,
				mInnerHeight,
				usedWidth,
				usedHeight);

			// Delta
			int deltaX = (width - usedWidth) / 2;
			int deltaY = (mHeight - usedHeight) / 2;

			// Transform the one and only child
			if (mWidth < width)
			{
				// Scrolling necessary
				mChildren.at(0)->transformAndSize(
					mX + deltaX - (int)(mOffset.getValue() * (float)std::max(0, (usedWidth - mInnerWidth))),
					mY + deltaY,
					usedWidth,
					usedHeight);
			}
			else
			{
				// No scrolling necessary
				mChildren.at(0)->transformAndSize(
					mX + deltaX,
					mY + deltaY,
					usedWidth,
					usedHeight);
			}
		}
    }
}
