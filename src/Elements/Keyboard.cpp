//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"

// TODO: Testing
#include <iostream>

namespace eyegui
{
    Keyboard::Keyboard(
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
        bool adaptiveScaling) : Element(
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
        mType = Type::KEYBOARD;

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);

        // Initialize keys
        newLine();

        // Add keys
        addKey(u'Q');
        addKey(u'W');
        addKey(u'E');
        addKey(u'R');
        addKey(u'T');
        addKey(u'Z');
        addKey(u'U');
        addKey(u'I');
        addKey(u'O');
        addKey(u'P');
        addKey(u'\u00dc');
        addKey(u'*');

        newLine();

        addKey(u'A');
        addKey(u'S');
        addKey(u'D');
        addKey(u'F');
        addKey(u'G');
        addKey(u'H');
        addKey(u'J');
        addKey(u'K');
        addKey(u'L');
        addKey(u'\u00d6');
        addKey(u'\u00c4');

        newLine();

        addKey(u'>');
        addKey(u'Y');
        addKey(u'X');
        addKey(u'C');
        addKey(u'V');
        addKey(u'B');
        addKey(u'N');
        addKey(u'M');
        addKey(u';');
        addKey(u':');
        addKey(u'-');
        addKey(u'_');
    }

    Keyboard::~Keyboard()
    {
        // Nothing to do
    }

    float Keyboard::specialUpdate(float tpf, Input* pInput)
    {
        // TODO: update keys (maybe do not save keycount but vector with matrices and...vector with letters...)
        return 0;
    }

    void Keyboard::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("backgroundColor", getStyle()->backgroundColor);
            mpBackground->getShader()->fillValue("alpha", mAlpha);
            mpBackground->getShader()->fillValue("activity", mActivity.getValue());
            mpBackground->getShader()->fillValue("dimColor", getStyle()->dimColor);
            mpBackground->getShader()->fillValue("dim", mDim.getValue());
            mpBackground->getShader()->fillValue("markColor", getStyle()->markColor);
            mpBackground->getShader()->fillValue("mark", mMark.getValue());
            mpBackground->draw();
        }

        // *** RENDER KEYS ***
        for(const auto& rLine : mKeys)
        {
            for(const auto& rKey : rLine)
            {
                rKey->draw(getStyle()->color, getStyle()->iconColor, mAlpha);
            }
        }
    }

    void Keyboard::specialTransformAndSize()
    {
        // Get line with maximum count
        int maxCountInLine = -1;
        for(const auto& rLine : mKeys)
        {
            int countInLine = rLine.size();
            if(countInLine > maxCountInLine)
            {
                maxCountInLine = countInLine;
            }
        }

        // Calculate size of keys
        int keySize;
        int maxHorizontalKeySize = mWidth / maxCountInLine;
        int maxVerticalKeySize = mHeight / mKeys.size();
        keySize = maxHorizontalKeySize > maxVerticalKeySize ? maxVerticalKeySize : maxHorizontalKeySize;
        int halfKeySize = keySize / 2;
        bool lastLineWasShifted = false;

        // Arrange keys
        for(int i = 0; i < mKeys.size(); i++) // Go over lines
        {
            // Count of keys in current row
            int keyCount = mKeys[i].size();

            // Decide, whether keys are shifted in that line
            int xOffset = 0;
            if((keyCount - maxCountInLine) % 2 != 0)
            {
                // X offset
                xOffset = halfKeySize;
            }
            for(int j = 0; j < keyCount; j++) // Go over keys
            {
                mKeys[i][j]->transformAndSize(
                    mX + halfKeySize + (j * keySize) + xOffset,
                    mY + halfKeySize + (i * keySize),
                    keySize);
            }
        }
    }

    void Keyboard::specialReset()
    {
        // Nothing to do
    }

    bool Keyboard::mayConsumeInput()
    {
        return true;
    }

    void Keyboard::addKey(char16_t character)
    {
        mKeys[mKeys.size()-1].push_back(mpAssetManager->createKey(mpLayout, character));
    }

    void Keyboard::newLine()
    {
        mKeys.push_back(std::vector<std::unique_ptr<Key> >());
    }
}
