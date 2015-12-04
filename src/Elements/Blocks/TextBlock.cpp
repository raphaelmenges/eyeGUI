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
        bool adaptiveScaling,
        bool consumeInput,
        float innerBorder,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content,
        std::string key) : Block(
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
            adaptiveScaling,
            consumeInput,
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

    void TextBlock::setContent(std::u16string content)
    {
        // TODO: somehow strange, maybe save the usage of the key in a bool
        if (mKey != EMPTY_STRING_ATTRIBUTE && mpLayout->getContentFromLocalization(mKey) != LOCALIZATION_NOT_FOUND)
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Content of TextBlock could not be set because value from key is in use");
        }
        else
        {
            mupTextFlow->setContent(content);
        }
    }

    void TextBlock::setKey(std::string key)
    {
        if (key == EMPTY_STRING_ATTRIBUTE)
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Tried to use empty key for localization");
        }
        else
        {
            std::u16string localization = mpLayout->getContentFromLocalization(key);
            if (localization == LOCALIZATION_NOT_FOUND)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No localization found for following key: " + key + ". Element has following id: " + getId());
            }
            else
            {
                mKey = key;
                mupTextFlow->setContent(localization);
            }
        }
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
