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

        // Fetch render item for keys
        mpKey = mpAssetManager->fetchRenderItem(
            shaders::Type::KEY,
            meshes::Type::QUAD);

        // TODO: Testing
        mKeyCount = 35;
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

        // *** KEYS *** // TODO: Crazy algorithm, not good
        mpKey->bind();
        mpKey->getShader()->fillValue("alpha", mAlpha);

        // Determine key size, count of rows and count of keys in even rows
        unsigned int size;
        unsigned int rowCount;
        unsigned int countPerRow;

        fitKeys(size, rowCount, countPerRow);

        // Draw keys
        float sizeWithKeyDistance = size * (1.f + KEY_DISTANCE);
        unsigned int keysInRow = 0;
        unsigned int row = 0;
        unsigned int keysNotInLastRow = 0;
        for (unsigned int j = 0; j < mKeyCount; j++)
        {
            // Determine current row
            if(keysInRow >= countPerRow - (row % 2))
            {
                keysInRow = 0;
                row++;
            }

            // Determine xOffset of key
            unsigned int xOffset = 0;
            if(row != rowCount - 1)
            {
                // Not the last row
                xOffset = (mWidth - (countPerRow - (row % 2)) * sizeWithKeyDistance) / 2; // Move it to center
                keysNotInLastRow++;
            }
            else
            {
                // Last row
                xOffset = (mWidth - (mKeyCount - keysNotInLastRow) * sizeWithKeyDistance) / 2; // Move it to center

            }
            xOffset += (size * KEY_DISTANCE) / 2;
            xOffset += keysInRow * sizeWithKeyDistance; // Offset by keys in rows

            // Determine yOffset of key
            unsigned int yOffset = 0;
            yOffset = row*size + (mHeight - size * rowCount) / 2;

            // Fill draw matrix
            glm::mat4 matrix = calculateDrawMatrix((int)(mX + xOffset), (int)(mY + yOffset), (int)size, (int)size);
            mpKey->getShader()->fillValue("matrix", matrix);

            // Draw the key
            mpKey->draw();

            // Increment key count of this row
            keysInRow++;
        }
    }

    void Keyboard::specialTransformAndSize()
    {
        // Nothing to do
    }

    void Keyboard::specialReset()
    {
        // Nothing to do
    }

    bool Keyboard::mayConsumeInput()
    {
        return false;
    }

    void Keyboard::fitKeys(unsigned int& rSize, unsigned int& rRowCount, unsigned int& rCountPerRow) const
    {
        // TODO: not optimal nor stable

        // Try to fit keys in available space
        float i = 0;
        bool sizeNotFound = true;

        while(sizeNotFound)
        {
            // Increment i (used to divide height and create size)
            i += 0.5f;

            // New size for keys
            rSize = mHeight / i;
            float sizeWithKeyDistance = rSize * (1.f + KEY_DISTANCE);

            // Check, how many fit in one row
            unsigned int currentCountPerRow = mWidth / sizeWithKeyDistance;

            // Calculate, how much height it would need
            unsigned int currentRowCount = 0;
            unsigned int currentKeyCount = 0;
            bool rowWithOneLess = false;
            while(currentKeyCount < mKeyCount)
            {
                // Add possible keys (for which would be space)
                if(rowWithOneLess && currentCountPerRow >= 1)
                {
                    currentKeyCount += currentCountPerRow-1;
                }
                else
                {
                    currentKeyCount += currentCountPerRow;
                }

                // Alternating count in rows
                rowWithOneLess = !rowWithOneLess;

                // Next row
                currentRowCount++;
            }

            // Check, whether height is enough for all keys
            if((currentRowCount * sizeWithKeyDistance) <= mHeight)
            {
                sizeNotFound = false;
                rRowCount = currentRowCount;
                rCountPerRow = currentCountPerRow;
            }
        }
    }
}
