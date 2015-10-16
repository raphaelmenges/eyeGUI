//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextBlock.h"

namespace eyegui
{
    TextBlock::TextBlock(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout* pLayout,
        Frame * pFrame,
        AssetManager* pAssetManager,
        float relativeScale,
        float border,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content,
        float innerBorder) : Block(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            relativeScale,
            border)
    {
        mType = Type::TEXT_BLOCK;

        // Fill members
        mInnerBorder = innerBorder;
        mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, content));
    }

    TextBlock::~TextBlock()
    {
        // Nothing to do
    }

    void TextBlock::specialDraw() const
    {
        // Super call
        Block::specialDraw();

        // Draw text
        mupTextFlow->draw(getStyle()->fontColor, mAlpha);
    }

    void TextBlock::specialTransformAndSize()
    {
        // Super call
        Block::specialTransformAndSize();

        // Use inner border (Copy code from Stack :-/) -> TODO: move to block?
        int usedBorder;
        int innerX, innerY, innerWidth, innerHeight;
        if (getOrientation() == Element::Orientation::HORIZONTAL)
        {
            usedBorder = (int)((float)mHeight * mInnerBorder);
        }
        else
        {
            usedBorder = (int)((float)mWidth * mInnerBorder);
        }
        innerX = mX + usedBorder / 2;
        innerY = mY + usedBorder / 2;
        innerWidth = mWidth - usedBorder;
        innerHeight = mHeight - usedBorder;

        // Tell text flow about transformation
        mupTextFlow->transformAndSize(innerX, innerY, innerWidth, innerHeight);
    }
}
