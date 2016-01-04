//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Block.h"

namespace eyegui
{
    Block::Block(
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
        float innerBorder) : Element(
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
            adaptiveScaling)
    {
        mType = Type::BLOCK;

        // Fill members
        mConsumeInput = consumeInput;
        mInnerBorder = innerBorder;

        // Fetch render item
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);
    }

    Block::~Block()
    {
        // Nothing to do so far
    }

    // Updating
    float Block::specialUpdate(float tpf, Input* pInput)
    {
        return 0;
    }

    void Block::specialDraw() const
    {
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind render item before setting values and drawing
            mpBackground->bind();

            // Fill matrix in shader
            mpBackground->getShader()->fillValue("matrix", mDrawMatrix);

            // Fill color to shader
            mpBackground->getShader()->fillValue(
                "backgroundColor",
                getStyle()->backgroundColor);

            // Fill alpha
            mpBackground->getShader()->fillValue("alpha", mAlpha);

            // Fill activity
            mpBackground->getShader()->fillValue("activity", mActivity.getValue());

            // Fill dimming
            mpBackground->getShader()->fillValue("dimColor", getStyle()->dimColor);
            mpBackground->getShader()->fillValue("dim", mDim.getValue());

			// Fill marking
			mpBackground->getShader()->fillValue("markColor", getStyle()->markColor);
			mpBackground->getShader()->fillValue("mark", mMark.getValue());

            // Draw render item
            mpBackground->draw();
        }
    }

    void Block::specialTransformAndSize()
    {
        // Use inner border
        int usedBorder;
        if (getOrientation() == Element::Orientation::HORIZONTAL)
        {
            usedBorder = (int)((float)mHeight * mInnerBorder);
        }
        else
        {
            usedBorder = (int)((float)mWidth * mInnerBorder);
        }
        mInnerX = mX + usedBorder / 2;
        mInnerY = mY + usedBorder / 2;
        mInnerWidth = mWidth - usedBorder;
        mInnerHeight = mHeight - usedBorder;
    }

    void Block::specialReset()
    {
        // Nothing to do
    }

    bool Block::mayConsumeInput()
    {
        return mConsumeInput;
    }
}
