//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Picture.h"

namespace eyegui
{
    Picture::Picture(
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
        std::string filepath,
        ImageAlignment alignment) : Element(
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
        mType = Type::PICTURE;

        // Fill members
        mupImage = std::move(mpAssetManager->createImage(mpLayout, filepath, alignment));

        // Aspect ratio of border should be preserved if necessary (only when pictue is neither zoomed nor stretched)
        if (alignment == ImageAlignment::ORIGINAL)
        {
            mBorderAspectRatio = (float)(mupImage->getTextureWidth()) / (float)(mupImage->getTextureHeight());
        }

    }

    Picture::~Picture()
    {
        // Nothing to do
    }

    void Picture::evaluateSize(
        int availableWidth,
        int availableHeight,
        int& rWidth,
        int& rHeight) const
    {
        mupImage->evaluateSize(availableWidth, availableHeight, rWidth, rHeight);
    }

    float Picture::specialUpdate(float tpf, Input* pInput)
    {
        return 0;
    }

    void Picture::specialDraw() const
    {
        // Draw image owned by this
        mupImage->draw(
            mAlpha,
            mActivity.getValue(),
            getStyle()->dimColor,
            mDim.getValue(),
            getStyle()->markColor,
            mMark.getValue());
    }

    void Picture::specialTransformAndSize()
    {
        mupImage->transformAndSize(mX, mY, mWidth, mHeight);
    }

    void Picture::specialReset()
    {
        // Nothing to do
    }

    bool Picture::mayConsumeInput()
    {
        return true;
    }
}
