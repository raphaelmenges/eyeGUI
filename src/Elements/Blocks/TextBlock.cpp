//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextBlock.h"

#include "Layout.h"
#include "OperationNotifier.h"

namespace eyegui
{
    TextBlock::TextBlock(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
		bool dimmable,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content,
        std::string key,
        float innerBorder) : Block(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
			dimmable,
			innerBorder)
    {
        mType = Type::TEXT_BLOCK;

        // Fill members
        mKey = key;

        // Create text flow
        if (mKey != EMPTY_STRING_ATTRIBUTE)
        {
            std::u16string localization = mpLayout->getContentFromLocalization(mKey);
            if (localization == LOCALIZATION_NOT_FOUND)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No localization used or one found for following key: " + mKey + ". Element has following id: " + getId());

                mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, content));
            }
            else
            {
                mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, localization));
            }
        }
        else
        {
            mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, content));
        }
    }

    TextBlock::~TextBlock()
    {
        // Nothing to do
    }

    void TextBlock::specialDraw() const
    {
        // Super call
        Block::specialDraw();

        // Draw text (emulation of shader bevavior for color mixing)
		glm::vec4 color = getStyle()->fontColor;
		color.a *= mAlpha;
		color *= (1.0f - mDimming.getValue()) + (mDimming.getValue() * getStyle()->dimColor);
        mupTextFlow->draw(1.0f, color);
    }

    void TextBlock::specialTransformAndSize()
    {
        // Super call
        Block::specialTransformAndSize();

        // Tell text flow about transformation
        mupTextFlow->transformAndSize(mInnerX, mInnerY, mInnerWidth, mInnerHeight);
    }
}
