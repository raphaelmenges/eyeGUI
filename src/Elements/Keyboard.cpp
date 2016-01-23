//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"

#include "NotificationQueue.h"
#include "OperationNotifier.h"

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
        bool adaptiveScaling) : NotifierElement(
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
        mGazePosition = glm::vec2(0,0);
        mNewFocus = true;

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);

        // Initialize keys
        newLine();

        // Add keys
        addKey(u'q');
        addKey(u'w');
        addKey(u'e');
        addKey(u'r');
        addKey(u't');
        addKey(u'z');
        addKey(u'u');
        addKey(u'i');
        addKey(u'o');
        addKey(u'p');
        addKey(u'\u00fc');
        addKey(u'+');

        newLine();

        addKey(u'a');
        addKey(u's');
        addKey(u'd');
        addKey(u'f');
        addKey(u'g');
        addKey(u'h');
        addKey(u'j');
        addKey(u'k');
        addKey(u'l');
        addKey(u'\u00f6');
        addKey(u'\u00e4');

        newLine();

        addKey(u'<');
        addKey(u'y');
        addKey(u'x');
        addKey(u'c');
        addKey(u'v');
        addKey(u'b');
        addKey(u'n');
        addKey(u'm');
        addKey(u'.');
        addKey(u'-');
        addKey(u'/');
        addKey(u'#');

        /*addKey(u'Q');
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
        addKey(u'#'); */
    }

    Keyboard::~Keyboard()
    {
        // Nothing to do
    }

    float Keyboard::specialUpdate(float tpf, Input* pInput)
    {
        // *** FILTER USER'S GAZE ***
        glm::vec2 newGazePosition = glm::vec2(pInput->gazeX, pInput->gazeY);
        if(mNewFocus)
        {
            mGazePosition = newGazePosition;
        }
        else
        {
            mGazePosition += std::min(1.f, 5.f * tpf) * (newGazePosition - mGazePosition);
        }

        // Use gaze delta as weight (is one if low delta in gaze)
        float gazeDelta = glm::abs(glm::distance(newGazePosition, mGazePosition)); // In pixels!
        float gazeDeltaWeight = 1.f - clamp(gazeDelta / (2 * mInitialKeySize), 0, 1); // Key size used for normalization

        // *** CHECK FOR PENETRATION ***

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Threshold (used to determine key selection)
        if(penetrated)
        {
            mThreshold.update(tpf * 4.f * (gazeDeltaWeight - 0.8f));
        }
        else
        {
            mThreshold.update(-2.0f * tpf);
        }

        // *** DETERMINE FOCUSED KEY ***
        if(penetrated)
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
                    float currentDistance = glm::abs(glm::distance(mGazePosition, mKeys[i][j]->getPosition()));
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

        // *** UPDATE POSITION OF FOCUS ***

        // Update focus position
        if(mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
        {
            mFocusPosition = mInitialKeyPositions[mFocusedKeyRow][mFocusedKeyColumn];
        }

        // *** UPDATE KEY POSITIONS ***
        bool keyPressed = false;

        // Update keys (they have to be transformed and sized each update)
        for(int i = 0; i < mKeys.size(); i++)
        {
            for(int j = 0; j < mKeys[i].size(); j++)
            {
                // Get delta between position of initial key position and gaze position
                glm::vec2 positionDelta = mInitialKeyPositions[i][j] - mGazePosition;

                // Radius of focus
                float focusWeight = glm::length(positionDelta) / (5 * mInitialKeySize); // Key size used for normalization
                focusWeight = clamp(focusWeight, 0, 1);

                // Only near keys have to be moved
                positionDelta *= 1.f - focusWeight;
                positionDelta *= 0.25f;

                // Calculate delta of size
                float sizeDelta = mInitialKeySize - glm::length(mKeys[i][j]->getPosition() - mGazePosition);
                sizeDelta = focusWeight + sizeDelta * (1.f - focusWeight);
                sizeDelta = std::max(-0.3f * mInitialKeySize, sizeDelta); // Subtracted from initial size
                sizeDelta *= 0.75f;

                // Weight with threshold
                positionDelta *= mThreshold.getValue();
                sizeDelta *= mThreshold.getValue();

                // Calc stuff for key
                int keyPositionX = (int)(mInitialKeyPositions[i][j].x + positionDelta.x);
                int keyPositionY = (int)(mInitialKeyPositions[i][j].y + positionDelta.y);
                int keySize = (int)(mInitialKeySize + sizeDelta);

                // Transform and size
                mKeys[i][j]->transformAndSize(keyPositionX, keyPositionY, keySize);

                // Updating
                mKeys[i][j]->update(tpf);

                // Check for "key pressed"
                if(mThreshold.getValue() >= 1.f && mKeys[i][j]->isFocused())
                {
                    // Check, whether gaze is really on focused key
                    if(
                        glm::abs(
                            glm::distance(
                                glm::vec2(keyPositionX, keyPositionY),
                                mGazePosition))
                        < keySize / 2)
                    {
                        // Save value in member to have it when notification queue calls the pipe method
                        mLastPressedKeyValue = mKeys[i][j]->getValue();
                        keyPressed = true;

                        // Inform listener after updating
                        mpNotificationQueue->enqueue(getId(), NotificationType::KEYBOARD_KEY_PRESSED);
                    }
                }
            }
        }

        if(keyPressed)
        {
            mThreshold.setValue(0);
        }

        // Check, whether next update will be a new focus
        if(penetrated)
        {
            mNewFocus = false;
        }
        else
        {
            mNewFocus = true;
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
            int countInLine = (int)rLine.size();
            if(countInLine > maxCountInLine)
            {
                maxCountInLine = countInLine;
            }
        }

        // Calculate size of keys
        int keySize;
        int maxHorizontalKeySize = (mWidth / maxCountInLine);
        maxHorizontalKeySize -= (int)(maxHorizontalKeySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE); // Substract distance
        int maxVerticalKeySize = (int)(mHeight / mKeys.size());
        keySize = maxHorizontalKeySize > maxVerticalKeySize ? maxVerticalKeySize : maxHorizontalKeySize;
        int halfKeySize = keySize / 2;

        // Calculate offset to center
        int xCenterOffset, yCenterOffset;
        xCenterOffset = (int)((mWidth - (maxCountInLine * (keySize + (keySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE)))) / 2.f);
        yCenterOffset = (int)((mHeight - (mKeys.size() * keySize)) / 2.f);

        // Save initial key positions
        for(int i = 0; i < mKeys.size(); i++) // Go over lines
        {
            // Count of keys in current row
            int keyCount = (int)mKeys[i].size();

            // Decide, whether keys are shifted in that line
            int xOffset = (int)(halfKeySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE);
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

                xOffset += (int)(keySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE);
            }
        }

        mInitialKeySize = (float)keySize;
    }

    void Keyboard::specialReset()
    {
        mThreshold.setValue(0);
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFocusPosition = glm::vec2(0,0);
        mGazePosition = glm::vec2(0,0);
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

    void Keyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::KEYBOARD_KEY_PRESSED:
            notifyListener(&KeyboardListener::keyPressed, pLayout, getId(), mLastPressedKeyValue);
            break;
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Keyboard got notification which is not thought for it.");
            break;
        }
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
