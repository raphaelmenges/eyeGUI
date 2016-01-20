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
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFocusPosition = glm::vec2(0,0);
        mNewFocus = true;

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
        // User's gaze
        glm::vec2 gazePoint = glm::vec2(pInput->gazeX, pInput->gazeY);

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Keyboard threshold
        mThreshold.update(0.5f * tpf, !penetrated);

        // Determine focused key
        if(!penetrated)
        {
            // Gaze now more in element
            mNewFocus = true;
        }
        else
        {
            // Go over keys and search nearest
            float minDistance = 1000000;
            int newFocusedKeyRow = -1;
            int newFocusedKeyColumn = -1;
            for(int i = 0; i < mKeys.size(); i++)
            {
                for(int j = 0; j < mKeys[i].size(); j++)
                {
                    // Use position in keys to get position after last update
                    float currentDistance = glm::abs(glm::distance(gazePoint, mKeys[i][j]->getPosition()));
                    if(currentDistance < minDistance)
                    {
                        minDistance = currentDistance;
                        newFocusedKeyRow = i;
                        newFocusedKeyColumn = j;
                    }
                }
            }

            // Set focus if necessary
            if(newFocusedKeyRow != mFocusedKeyRow || newFocusedKeyColumn != mFocusedKeyColumn)
            {
                // Unset old focus
                if(mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
                {
                    mKeys[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(false);
                }

                // Set new focus
                mFocusedKeyRow = newFocusedKeyRow;
                mFocusedKeyColumn = newFocusedKeyColumn;
                mKeys[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(true);
            }
        }

        // Update focus position
        if(mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
        {
            if(mNewFocus)
            {
                // Set focus position directly
                mFocusPosition = mInitialKeyPositions[mFocusedKeyRow][mFocusedKeyColumn];
                mNewFocus = false;
            }
            else
            {
                // Move focus position to focused key
                mFocusPosition += tpf * (mInitialKeyPositions[mFocusedKeyRow][mFocusedKeyColumn] - mFocusPosition);
            }
        }

        // Update keys (they have to be transformed and sized each update)
        for(int i = 0; i < mKeys.size(); i++)
        {
            for(int j = 0; j < mKeys[i].size(); j++)
            {
                // Get delta between focused node and this node
                glm::vec2 positionDelta = mInitialKeyPositions[i][j] - mFocusPosition;

                // Make delta depenend on itself
                float positionDeltaWeight = glm::length(positionDelta) / (5 * mInitialKeySize); // Key size used for normalization
                positionDeltaWeight = 1.f - clamp(positionDeltaWeight, 0, 1);

                // Only near keys have to be moved
                positionDelta *= positionDeltaWeight;

                // Calculate delta of size
                float sizeDelta = mInitialKeySize - glm::length(mKeys[i][j]->getPosition() - gazePoint);
                sizeDelta = std::max(-mInitialKeySize / 2, sizeDelta);

                // Weight with threshold
                positionDelta *= mThreshold.getValue();
                sizeDelta *= mThreshold.getValue();

                // Transform and size
                mKeys[i][j]->transformAndSize(
                    mInitialKeyPositions[i][j].x + positionDelta.x,
                    mInitialKeyPositions[i][j].y + positionDelta.y,
                    mInitialKeySize + sizeDelta);

                // Updating
                mKeys[i][j]->update(tpf);

                // TODO: react when size is over certain threshold?
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
                rKey->draw(mX, mY, mWidth, mHeight, getStyle()->color, getStyle()->iconColor, mAlpha);
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
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFocusPosition = glm::vec2(0,0);
        mNewFocus = true;

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
