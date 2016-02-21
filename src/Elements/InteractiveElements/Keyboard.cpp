//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"

#include "NotificationQueue.h"
#include "OperationNotifier.h"
#include "externals/utfcpp/source/utf8.h"

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
        bool adaptiveScaling) : InteractiveElement(
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
		mKeyWasPressed = false;

		// TODO: Fast typing (make it changeable via interface)
		mUseFastTyping = false;
		mFastBuffer = u"";

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);

        // Initialize keys
        newLine();

        // Add keys
		addKey(u'1');
		addKey(u'2');
		addKey(u'3');
		addKey(u'4');
		addKey(u'5');
		addKey(u'6');
		addKey(u'7');
		addKey(u'8');
		addKey(u'9');
		addKey(u'0');
		addKey(u'/');

		newLine();

		addKey(u'q');
		addKey(u'w');
		addKey(u'e');
		addKey(u'r');
		addKey(u't');
		addKey(u'y');
		addKey(u'u');
		addKey(u'i');
		addKey(u'o');
		addKey(u'p');
		addKey(u'+');
		addKey(u'-');

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
		addKey(u'(');
		addKey(u')');

		newLine();

		addKey(u'#');
		addKey(u'z');
		addKey(u'x');
		addKey(u'c');
		addKey(u'v');
		addKey(u'b');
		addKey(u'n');
		addKey(u'm');
		addKey(u'.');
		addKey(u':');
		addKey(u'<');
		addKey(u'>');

		// QWERTZ layout, does not work within CEF3 because of german umlaute for the moment
		/*addKey(u'1');
		addKey(u'2');
		addKey(u'3');
		addKey(u'4');
		addKey(u'5');
		addKey(u'6');
		addKey(u'7');
		addKey(u'8');
		addKey(u'9');
		addKey(u'0');
		addKey(u'#');

		newLine();

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
        addKey(u'#'); */

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
		// Some parameters which may or may not be defineable in config file
		float PRESSED_KEY_FADING_DURATION = 0.5f;
		float PRESSED_KEY_SCALING_MULTIPLIER = 5.f;
		float GAZE_FILTER_RADIUS = 4.f;
		float GAZE_DIRECT_USAGE_MULTIPLIER = 5.f;
		float GAZE_DELTA_WEIGHT_RADIUS = 2.f;
		float THRESHOLD_DECREASE_AFTER_PRESS_DURATION = 0.2f;
		float THRESHOLD_INCREASE_DURATION = 0.5f;
		float THRESHOLD_DECREASE_DURATION = 1.f;
		float FOCUS_RADIUS = 3.0f;
		float KEY_POSITION_DELTA_MULTIPLIER = 0.25f;
		float KEY_SIZE_DELTA_MULTIPLIER = 0.5f;
		float MINIMAL_KEY_SIZE = 0.75f;

		// Call super
		InteractiveElement::specialUpdate(tpf, pInput);

		// Check for penetration by input
		bool penetrated = penetratedByInput(pInput);

		// *** UPDATE ANIMATED PRESSED KEYES ***
		std::vector<int> dyingPressedKeys;
		for (int i = 0; i < mPressedKeys.size(); i++)
		{
			// Update alpha and size
			mPressedKeys[i].first -= tpf / PRESSED_KEY_FADING_DURATION;
			Key* pKey = mPressedKeys[i].second.get();
			pKey->transformAndSize((int)pKey->getPosition().x, (int)pKey->getPosition().y, (int)(pKey->getSize() + PRESSED_KEY_SCALING_MULTIPLIER * tpf * mInitialKeySize));

			// Check, whether still visible
			if (mPressedKeys[i].first <= 0)
			{
				dyingPressedKeys.push_back(i);
			}
		}

		// Delete dying pressed keys (go backwards through it)
		for (int i = (int)dyingPressedKeys.size()-1; i >= 0; i--)
		{
			mPressedKeys.erase(mPressedKeys.begin() + i);
		}

        // *** FILTER USER'S GAZE ***
		glm::vec2 rawGazeDelta = glm::vec2(0,0);
		if (pInput != NULL)
		{
			rawGazeDelta = glm::vec2(pInput->gazeX, pInput->gazeY) - mGazePosition;
		}

		// Filter only, when delta is small
		float gazeFilterRadius = GAZE_FILTER_RADIUS * mInitialKeySize;
		float rawGazeFilter = std::min(1.f, glm::abs(glm::length(rawGazeDelta)) / gazeFilterRadius); // 0 when filtering and 1 when direkt usage of gaze
		float gazeFilter = rawGazeFilter + (1.f - rawGazeFilter) * std::min(1.f, GAZE_DIRECT_USAGE_MULTIPLIER * tpf);
		mGazePosition += gazeFilter * rawGazeDelta;

		// Use gaze delta as weight for threshold (is one if low delta in gaze)
		float gazeDelta = glm::abs(glm::length(rawGazeDelta)); // In pixels!
		float gazeDeltaWeight = 1.f - clamp(gazeDelta / (GAZE_DELTA_WEIGHT_RADIUS * mInitialKeySize), 0, 1); // Key size used for normalization

		// *** CHECK FOR PENETRATION ***

		// Threshold (used to determine pressing a key)
		if (mKeyWasPressed)
		{
			mThreshold.update(-tpf / THRESHOLD_DECREASE_AFTER_PRESS_DURATION);
			if (mThreshold.getValue() <= 0)
			{
				mKeyWasPressed = false;
			}
		}
		else if (penetrated)
		{
			mThreshold.update((tpf * (gazeDeltaWeight - 0.8f)) / THRESHOLD_INCREASE_DURATION);
		}
		else
		{
			mThreshold.update(-tpf / THRESHOLD_DECREASE_DURATION);
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
					mKeys[mFocusedKeyRow][mFocusedKeyColumn]->setSelect(false);
                }

                // Set new focus
                mFocusedKeyRow = newFocusedKeyRow;
                mFocusedKeyColumn = newFocusedKeyColumn;
                mKeys[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(true);
            }

			// Selection of focused key
			if (mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
			{
				Key* pFocusedKey = mKeys[mFocusedKeyRow][mFocusedKeyColumn].get();
				if (!pFocusedKey->isSelected() && pFocusedKey->getFocusValue() >= 1.f)
				{
					pFocusedKey->setSelect(true);

					// Add selected key's value to buffer
					mFastBuffer += pFocusedKey->getValue();
				}
			}
        }

        // *** UPDATE POSITION OF FOCUS ***

        // Update focus position
        if(mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
        {
            mFocusPosition = mInitialKeyPositions[mFocusedKeyRow][mFocusedKeyColumn];
        }

        // *** UPDATE KEY POSITIONS ***

        // Update keys (they have to be transformed and sized each update)
        for(int i = 0; i < mKeys.size(); i++)
        {
            for(int j = 0; j < mKeys[i].size(); j++)
            {
                // Get delta between position of initial key position and gaze position
                glm::vec2 positionDelta = mInitialKeyPositions[i][j] - mGazePosition;

                // Radius of focus
                float focusWeight = 1.f - glm::length(positionDelta) / (FOCUS_RADIUS * mInitialKeySize); // Key size used for normalization
                focusWeight = clamp(focusWeight, 0, 1);

                // Only near keys have to be moved
                positionDelta *= focusWeight;
                positionDelta *= KEY_POSITION_DELTA_MULTIPLIER;

                // Calculate delta of size
                float sizeDelta = mInitialKeySize - glm::length(mKeys[i][j]->getPosition() - mGazePosition);
                sizeDelta = (1.f - focusWeight) + sizeDelta * focusWeight;
                sizeDelta = std::max(-(1.f - MINIMAL_KEY_SIZE) * mInitialKeySize, sizeDelta); // Subtracted from initial size
                sizeDelta *= KEY_SIZE_DELTA_MULTIPLIER;

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
                if(!mKeyWasPressed && mThreshold.getValue() >= 1.f && mKeys[i][j]->isFocused())
                {
                    // Check, whether gaze is really on focused key
                    if(
                        glm::abs(
                            glm::distance(
                                glm::vec2(keyPositionX, keyPositionY),
                                mGazePosition))
                        < keySize / 2)
					{
						if (mUseFastTyping)
						{
							// If fast typing is used, initial last pressed key value with buffer
							mLastPressedKeyValue = mFastBuffer;
						}
						else
						{
							// Save value in member to have it when notification queue calls the pipe method
							mLastPressedKeyValue = mKeys[i][j]->getValue();
						}

						// Reset fast buffer
						mFastBuffer = u"";

						// Just a reminder for the threshold to decrease
						mKeyWasPressed = true;

                        // Inform listener after updating
                        mpNotificationQueue->enqueue(getId(), NotificationType::KEYBOARD_KEY_PRESSED);

						// Add pressed key for nice animation
						std::unique_ptr<Key> upPressedKey = std::unique_ptr<Key>(new CharacterKey(*(CharacterKey*)(mKeys[i][j].get())));
						upPressedKey->transformAndSize();
						mPressedKeys.push_back(PressedKey(1.f, std::move(upPressedKey)));
                    }
                }
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
            for(const auto& rupKey : rLine)
            {
                rupKey->draw(mX, mY, mWidth, mHeight, getStyle()->color, getStyle()->selectionColor, getStyle()->iconColor, mAlpha);
            }
        }

		// Render animation of pressed keys
		for (const auto& rPressedKey : mPressedKeys)
		{
			rPressedKey.second->draw(mX, mY, mWidth, mHeight, getStyle()->color, getStyle()->selectionColor, getStyle()->iconColor, mAlpha * rPressedKey.first);
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

			// Go over keys to save initial position
            for(int j = 0; j < keyCount; j++)
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
		// Call super
		InteractiveElement::specialReset();

        mThreshold.setValue(0);
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFocusPosition = glm::vec2(0,0);
        mGazePosition = glm::vec2(0,0);
		mKeyWasPressed = false;
		mPressedKeys.clear();
		mFastBuffer = u"";

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
		// Call super
		InteractiveElement::mayConsumeInput();

        return true;
    }

	void Keyboard::specialInteract()
	{
		// TODO (what should happen at simple interaction?)
	}

    void Keyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
		// Has to be declared here, otherwise Visual Studio does not compile...
		std::string lastPressedKeyValue8;

        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::KEYBOARD_KEY_PRESSED:

			// Notify listener method with UTF-16 string
			notifyListener(&KeyboardListener::keyPressed, pLayout, getId(), mLastPressedKeyValue);

			// Convert last pressed value to UTF-8 string
			utf8::utf16to8(mLastPressedKeyValue.begin(), mLastPressedKeyValue.end(), back_inserter(lastPressedKeyValue8));
  
			// Notify listener method with UTF-8 string
			notifyListener(&KeyboardListener::keyPressed, pLayout, getId(), lastPressedKeyValue8);
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
