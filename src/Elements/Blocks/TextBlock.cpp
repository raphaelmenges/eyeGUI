//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextBlock.h"

#include "Layout.h"
#include "src/Utilities/OperationNotifier.h"

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
        bool dimming,
        bool adaptiveScaling,
        bool consumeInput,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        float innerBorder,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        float textScale,
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
            dimming,
            adaptiveScaling,
            consumeInput,
            backgroundFilepath,
            backgroundAlignment,
            innerBorder)
    {
        mType = Type::TEXT_BLOCK;

        // Fill members
        mKey = key;

        // Content of text flow
        std::u16string textFlowContent;
        if (mKey != EMPTY_STRING_ATTRIBUTE)
        {
            std::u16string localization = mpLayout->getContentFromLocalization(mKey);
            if (localization == LOCALIZATION_NOT_FOUND)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No localization used or one found for following key: " + mKey + ". Element has following id: " + getId());

                textFlowContent = content;
            }
            else
            {
                textFlowContent = localization;
            }
        }
        else
        {
            textFlowContent = content;

        }

        // Create text flow
        mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, textScale, textFlowContent));
    }

    TextBlock::~TextBlock()
    {
        // Nothing to do
    }

    void TextBlock::setContent(std::u16string content)
    {
        // Check whether value from key is in use
        if (mKey != EMPTY_STRING_ATTRIBUTE && mpLayout->getContentFromLocalization(mKey) == LOCALIZATION_NOT_FOUND)
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

        // Drawing of text flow
        mupTextFlow->draw(getStyle()->fontColor, mAlpha);
    }

    void TextBlock::specialTransformAndSize()
    {
        // Super call
        Block::specialTransformAndSize();

        // Tell text flow about transformation
        mupTextFlow->transformAndSize(mInnerX, mInnerY, mInnerWidth, mInnerHeight);
    }
}
