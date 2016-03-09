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
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
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

        // Fetch background image if one is wanted
        if(backgroundFilepath != EMPTY_STRING_ATTRIBUTE)
        {
            mupImage = mpAssetManager->createImage(mpLayout, backgroundFilepath, backgroundAlignment);
        }
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
        // Draw simple background color
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind render item before setting values and drawing
            mpBackground->bind();

            // Fill matrix in shader
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);

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

        // Draw background image if available
        if(mupImage != NULL)
        {
            mupImage->draw(
                mAlpha,
                mActivity.getValue(),
                getStyle()->dimColor,
                mDim.getValue(),
                getStyle()->markColor,
                mMark.getValue());
        }
    }

    void Block::specialTransformAndSize()
    {
        // Background image placing
        if(mupImage != NULL)
        {
            // First, evaluate size of image (not in own evaluation method, because
            // this element is not dependend on image size.
            int imageWidth, imageHeight;
            mupImage->evaluateSize(mWidth, mHeight, imageWidth, imageHeight);

            // Center image in element
            int imageDeltaX = (mWidth - imageWidth) / 2;
            int imageDeltaY = (mHeight - imageHeight) / 2;
            mupImage->transformAndSize(mX + imageDeltaX, mY + imageDeltaY, imageWidth, imageHeight);
        }

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
