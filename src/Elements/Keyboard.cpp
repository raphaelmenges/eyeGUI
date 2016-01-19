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

        // Initialize members
        mInitialKeySize = 0;
        mThreshold.setValue(0);
        mpFocusedKey = NULL;
        mFocusedKeyRow = 0;
        mFocusedKeyColumn = 0;
        mGrowOffset = glm::vec2(0,0);

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
        addKey(u'_');
        addKey(u'#');
    }

    Keyboard::~Keyboard()
    {
        // Nothing to do
    }

    float Keyboard::specialUpdate(float tpf, Input* pInput)
    {
        // TODO REDO whole method!!!

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Keyboard threshold
        mThreshold.update(tpf, !penetrated);

        // Determine focused key
        if(penetrated)
        {
            // User's gaze
            glm::vec2 point = glm::vec2(pInput->gazeX, pInput->gazeY);

            // Go over keys and search nearest
            float distance = 1000000;
            Key* pNewFocusedKey = NULL;
            int newFocusedKeyRow = 0;
            int newFocusedKeyColumn = 0;
            for(int i = 0; i < mKeys.size(); i++)
            {
                for(int j = 0; j < mKeys[i].size(); j++)
                {
                    // Use position in keys to get position after last update
                    float currentDistance = glm::abs(glm::distance(point, mKeys[i][j]->getPosition()));
                    if(currentDistance < distance)
                    {
                        distance = currentDistance;
                        pNewFocusedKey = mKeys[i][j].get();
                        newFocusedKeyRow = i;
                        newFocusedKeyColumn = j;
                    }
                }
            }

            // Set focus if necessary
            if(pNewFocusedKey != mpFocusedKey)
            {
                if(mpFocusedKey != NULL)
                {
                    mpFocusedKey->setFocus(false);
                }
                mpFocusedKey = pNewFocusedKey;
                mpFocusedKey->setFocus(true);
                mFocusedKeyRow = newFocusedKeyRow;
                mFocusedKeyColumn = newFocusedKeyColumn;
            }
        }

        // Update keys (they have to be transformed and sized each update)
        for(int i = 0; i < mKeys.size(); i++)
        {
            for(int j = 0; j < mKeys[i].size(); j++)
            {
                // TODO: do the magic here
                if(mpFocusedKey != NULL)
                {
                   glm::vec2 newGrowOffset = mInitialKeyPositions[i][j] - mInitialKeyPositions[mFocusedKeyRow][mFocusedKeyColumn];
                   mGrowOffset +=  0.01f * tpf * (newGrowOffset - mGrowOffset);
                }

                // Transform and size
                mKeys[i][j]->transformAndSize(
                    mInitialKeyPositions[i][j].x + mGrowOffset.x * mThreshold.getValue(),
                    mInitialKeyPositions[i][j].y + mGrowOffset.y * mThreshold.getValue(),
                    mInitialKeySize);

                // Updating
                mKeys[i][j]->update(tpf);
            }
        }

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
        int maxHorizontalKeySize = (mWidth / maxCountInLine);
        maxHorizontalKeySize -= maxHorizontalKeySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE; // Substract distance
        int maxVerticalKeySize = mHeight / mKeys.size();
        keySize = maxHorizontalKeySize > maxVerticalKeySize ? maxVerticalKeySize : maxHorizontalKeySize;
        int halfKeySize = keySize / 2;

        // Calculate offset to center
        int xCenterOffset, yCenterOffset;
        xCenterOffset = (mWidth - (maxCountInLine * (keySize + (keySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE)))) / 2;
        yCenterOffset = (mHeight - (mKeys.size() * keySize)) / 2;

        // Save initial key positions
        for(int i = 0; i < mKeys.size(); i++) // Go over lines
        {
            // Count of keys in current row
            int keyCount = mKeys[i].size();

            // Decide, whether keys are shifted in that line
            int xOffset = halfKeySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE;
            if((keyCount - maxCountInLine) % 2 != 0)
            {
                // Add shift
                xOffset += halfKeySize;
            }
            for(int j = 0; j < keyCount; j++) // Go over keys
            {
                mInitialKeyPositions[i][j] = glm::vec2(
                    mX + xCenterOffset + halfKeySize + (j * keySize) + xOffset,
                    mY + yCenterOffset + halfKeySize + (i * keySize));

                xOffset += keySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE;
            }
        }

        mInitialKeySize = keySize;
    }

    void Keyboard::specialReset()
    {
        mThreshold.setValue(0);
        mpFocusedKey = NULL;
        mFocusedKeyRow = 0;
        mFocusedKeyColumn = 0;
        mGrowOffset = glm::vec2(0,0);

        // Reset keys
        for(const auto& rLine : mKeys)
        {
            for(const auto& rKey : rLine)
            {
                rKey->reset();
            }
        }
    }

    bool Keyboard::mayConsumeInput()
    {
        return true;
    }

    void Keyboard::addKey(char16_t character)
    {
        mKeys[mKeys.size()-1].push_back(mpAssetManager->createKey(mpLayout, character));
        mInitialKeyPositions[mInitialKeyPositions.size()-1].push_back(glm::vec2());
    }

    void Keyboard::newLine()
    {
        mKeys.push_back(std::vector<std::unique_ptr<Key> >());
        mInitialKeyPositions.push_back(std::vector<glm::vec2>());
    }
}
