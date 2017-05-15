//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"

#include "src/NotificationQueue.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Layout.h"
#include "src/Rendering/ScissorStack.h"

#include <limits>

namespace eyegui
{
    Keyboard::Keyboard(
        std::string id,
        std::vector<std::string> styles,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
		bool instantPress) : InteractiveElement(
            id,
            styles,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            RenderingMask::BOX,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::KEYBOARD;
		mForcedKeyboardLayout = false;

		// Fill members
		mInstantPress = instantPress;

        // Initialize members
        mCurrentKeymapIndex = 0;
        mBigCharactersActive = false;
        mUseFastTyping = false;

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::COLOR,
            meshes::Type::QUAD);

        // Create keymaps (mKeymaps)
		fillKeymaps(mpLayout->getKeyboardLayout());
    }

	Keyboard::Keyboard(
		std::string id,
		std::vector<std::string> styles,
		Element* pParent,
		Layout const * pLayout,
		Frame* pFrame,
		AssetManager* pAssetManager,
		NotificationQueue* pNotificationQueue,
		float relativeScale,
		float border,
		bool dimming,
		bool adaptiveScaling,
		bool instantPress,
		KeyboardLayout keyboardLayout) : InteractiveElement(
			id,
			styles,
			pParent,
			pLayout,
			pFrame,
			pAssetManager,
			pNotificationQueue,
			RenderingMask::BOX,
			relativeScale,
			border,
			dimming,
			adaptiveScaling)
	{
		mType = Type::KEYBOARD;
		mForcedKeyboardLayout = true;

		// Fill members
		mInstantPress = instantPress;

		// Initialize members
		mCurrentKeymapIndex = 0;
		mBigCharactersActive = false;
		mUseFastTyping = false;

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Create keymaps (mKeymaps)
		fillKeymaps(keyboardLayout);
	}

    Keyboard::~Keyboard()
    {
        // Nothing to do
    }

    void Keyboard::setFastTyping(bool useFastTyping)
    {
        // First check, whether set is necessary
        if(useFastTyping != mUseFastTyping)
        {
            mUseFastTyping = useFastTyping;
            mFastBuffer = u"";
            mLastFastKeyRow = -1;
            mLastFastKeyColumn = -1;

            // Unset picked if possible
            if(mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
            {
                // Just do it for both
                mKeymaps[mCurrentKeymapIndex].smallKeys[mFocusedKeyRow][mFocusedKeyColumn]->setPicked(false);
                mKeymaps[mCurrentKeymapIndex].bigKeys[mFocusedKeyRow][mFocusedKeyColumn]->setPicked(false);
            }
        }
    }

    void Keyboard::setCase(KeyboardCase keyboardCase)
    {
        // Set case
        switch (keyboardCase)
        {
        case KeyboardCase::LOWER:
            mBigCharactersActive = false;
        break;
        case KeyboardCase::UPPER:
            mBigCharactersActive = true;
        break;
        }

        // Reset state
        resetKeymapsAndState();
    }

    uint Keyboard::getCountOfKeymaps() const
    {
        return (uint)(mKeymaps.size());
    }

    void Keyboard::setKeymap(uint keymapIndex)
    {
        // Set keymap (must be positive because unsigned int is used)
        if(keymapIndex >= getCountOfKeymaps())
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Keymap index does not exist in keyboard: " + getId());
        }
        else
        {
            mCurrentKeymapIndex = keymapIndex;
        }

        // Reset state
        resetKeymapsAndState();
    }

    InteractiveElement* Keyboard::internalNextInteractiveElement(Element const * pChildCaller)
    {
        // Not implemented for keyboard
        // If selection visualization and interact is implemented, just delete
        // this method because InteractiveElement implements it correctly, already.
        return NULL;
    }

	void Keyboard::classifyKey(bool accept)
	{
		if (mSelectionClassification == Classification::PENDING)
		{
			if (accept)
			{
				mSelectionClassification = Classification::ACCEPT;
			}
			else
			{
				mSelectionClassification = Classification::REJECT;
			}
		}
	}

	void Keyboard::updateKeyboardLayout()
	{
		// Use layout to fill keymaps
		fillKeymaps(mpLayout->getKeyboardLayout());

		// Tranform and size necessary to calc intial positions of keys
		specialTransformAndSize();
	}

	float Keyboard::specialUpdate(float tpf, Input* pInput)
	{
		// *** SET UP PARAMETERS ***

		// Radius stuff is given in key radii (since normalized with that value)

		// Parameters of gaze smoothing
		float GAZE_FILTER_DURATION = 0.25f; // Time for smoothed gaze to interpolate towards current raw gaze value. Applied when zoom gets higher

		// Parameters of zooming
		float ZOOM_INCREASE_DURATION = 0.75f; // Duration of zoom to become full. Not to take as "real" seconds since depending on other values as well
		float ZOOM_DECREASE_AFTER_PRESS_DURATION = 0.4f; // Decrease duration of zoom after pressing
		float ZOOM_DECREASE_DURATION = 1.f; // General decrease duration of zoom if no gaze is upon element

		// Parameters of visualization
		float FOCUS_RADIUS = 3.5f; // Radius of gaze affected keys (in the center bigger, else smaller). Normalized by standard key size
		float MINIMAL_KEY_SIZE = 0.25f; // Cap minimal key size for those in outer focus area
		float PRESSED_KEY_SCALING_MULTIPLIER = 2.5f; // Just animation scale of pressed key which is moving and fading towards user

		// Use speed multiplier
		ZOOM_INCREASE_DURATION /= getStyleValue(StylePropertyFloat::KeyboardZoomSpeedMultiplier);

		// *** SETUP ***

		// Call super
		float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

		// Check for penetration by input
		bool penetrated = penetratedByInput(pInput);

		// Get pointers from currently active keymap
		SubKeymap* pKeys = NULL;
		PositionMap* pInitialKeyPositions = &(mKeymaps[mCurrentKeymapIndex].initialKeyPositions);
		float initialKeySize = mKeymaps[mCurrentKeymapIndex].initialKeySize;
		if (mBigCharactersActive)
		{
			pKeys = &(mKeymaps[mCurrentKeymapIndex].bigKeys);
		}
		else
		{
			pKeys = &(mKeymaps[mCurrentKeymapIndex].smallKeys);
		}

		// If something is selected, wait for reaction from application
		if (mSelectionClassification != Classification::NO_SELECTION)
		{
			// Wait for application to classify selection
			if (mInstantPress || mSelectionClassification == Classification::ACCEPT) // accept
			{
				// Press the key
				pressKey(mpSelectedKey);

				// Reset selection
				mpSelectedKey->unselect();
				mSelectionClassification = Classification::NO_SELECTION;
				mpSelectedKey = nullptr;
			}
			else if (mSelectionClassification == Classification::REJECT) // reject
			{
				// Reset selection
				mpSelectedKey->unselect();
				mSelectionClassification = Classification::NO_SELECTION;
				mpSelectedKey = nullptr;

			} // else: pending
		}
		else
		{
			// *** UPDATE ANIMATED PRESSED KEYES ***
			std::vector<int> dyingPressedKeys;
			for (uint i = 0; i < mPressedKeys.size(); i++)
			{
				// Update alpha and size
				mPressedKeys[i].first -= tpf / INTERACTION_FADING_DURATION;
				Key* pKey = mPressedKeys[i].second.get();
				pKey->transformAndSize(
					(int)pKey->getPosition().x,
					(int)pKey->getPosition().y,
					(int)
					((float)initialKeySize // using initial size as base
						+ (float)initialKeySize * PRESSED_KEY_SCALING_MULTIPLIER
						* (1.f - mPressedKeys[i].first)));

				// Check, whether still visible
				if (mPressedKeys[i].first <= 0)
				{
					dyingPressedKeys.push_back(i);
				}
			}

			// Delete dying pressed keys (go backwards through it)
			for (int i = (int)dyingPressedKeys.size() - 1; i >= 0; i--)
			{
				mPressedKeys.erase(mPressedKeys.begin() + dyingPressedKeys[i]);
			}

			// *** FILTER USER'S GAZE ***
			glm::vec2 rawGazeDelta = glm::vec2(0, 0);
			if (pInput != NULL)
			{
				rawGazeDelta = glm::vec2(pInput->gazeX, pInput->gazeY) - mFilteredGazePosition;
			}

			// Filter only, when zoom is higher
			float sqrtZoom = glm::sin(mZoom.getValue() * 3.14f * 0.5f); // make starting and ending of function smoother
			float gazeFilter = (1.f - sqrtZoom) + sqrtZoom * (tpf / GAZE_FILTER_DURATION);
			mFilteredGazePosition += gazeFilter * rawGazeDelta; // update filtered gaze position

			// *** UPDATE ZOOM ***
			if (mKeyboardRecovers)
			{
				mZoom.update(-tpf / ZOOM_DECREASE_AFTER_PRESS_DURATION);
				if (mZoom.getValue() <= 0)
				{
					mKeyboardRecovers = false;
				}
			}
			else if (penetrated)
			{
				mZoom.update(tpf / ZOOM_INCREASE_DURATION);
			}
			else
			{
				mZoom.update(-tpf / ZOOM_DECREASE_DURATION);
			}

			// *** DETERMINE FOCUSED KEY ***
			if (penetrated)
			{
				// Go over keys and search nearest
				float minDistance = std::numeric_limits<float>::max();
				int newFocusedKeyRow = -1;
				int newFocusedKeyColumn = -1;
				for (uint i = 0; i < pKeys->size(); i++)
				{
					for (uint j = 0; j < (*pKeys)[i].size(); j++)
					{
						// Use position in keys to get position after last update
						float currentDistance = glm::abs(glm::distance(mFilteredGazePosition, (*pKeys)[i][j]->getPosition()));
						if (currentDistance < minDistance)
						{
							minDistance = currentDistance;
							newFocusedKeyRow = i;
							newFocusedKeyColumn = j;
						}
					}
				}

				// Set focus if necessary
				if (newFocusedKeyRow != mFocusedKeyRow || newFocusedKeyColumn != mFocusedKeyColumn)
				{
					// Unset old focus
					if (mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
					{
						(*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(false);
						(*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->setPicked(false);

						// Notify interaction
						notifyInteraction("LEAVE_KEY", convertUTF16ToUTF8((*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->getValue()));
					}

					// Set new focus
					mFocusedKeyRow = newFocusedKeyRow;
					mFocusedKeyColumn = newFocusedKeyColumn;
					(*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(true);

					// Notify interaction
					notifyInteraction("ENTER_KEY", convertUTF16ToUTF8((*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->getValue()));
				}

				// Pick focused key when using fast typing
				if (mUseFastTyping && mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
				{
					Key* pFocusedKey = (*pKeys)[mFocusedKeyRow][mFocusedKeyColumn].get();
					if (!pFocusedKey->isPicked() && pFocusedKey->getFocusValue() >= 1.f)
					{
						pFocusedKey->setPicked(true);

						// Add picked key's value to buffer (only adds new picked keys)
						mFastBuffer += pFocusedKey->getValue();
						mLastFastKeyRow = mFocusedKeyRow;
						mLastFastKeyColumn = mFocusedKeyColumn;
					}
				}
			}

			// *** UPDATE KEY POSITIONS ***

			// Update keys (they have to be transformed and sized each update)
			for (uint i = 0; i < pKeys->size(); i++)
			{
				for (uint j = 0; j < (*pKeys)[i].size(); j++)
				{
					// Clamp position to borders of element
					glm::vec2 clampledGazePosition(
						glm::clamp(mFilteredGazePosition.x, (float)mX, (float)(mX + mWidth)),
						glm::clamp(mFilteredGazePosition.y, (float)mY, (float)(mY + mHeight)));

					// Get delta between position of initial key position and gaze position
					glm::vec2 positionDelta = (*pInitialKeyPositions)[i][j] - clampledGazePosition;

					// Radius of focus
					float focusWeight = 1.f - (glm::length(positionDelta) / (FOCUS_RADIUS * initialKeySize)); // key size used for normalization
					focusWeight = clamp(focusWeight, 0, 1); // between zero and one

					// Only near keys have to be moved
					positionDelta *= 0.25f; // scale it
					positionDelta *= focusWeight; // only use it within focus

					// Calculate delta of size (use already calculated position delta as basis)
					float sizeDelta = glm::length(positionDelta); // length of position delta
					sizeDelta *= 2.f; // scale it
					sizeDelta = -sizeDelta; // make outer keys smaller, not bigger
					sizeDelta += 0.5f * initialKeySize * glm::sin(0.5f * 3.14f * focusWeight); // make inner keys bigger

					// Weight with zoom
					positionDelta *= mZoom.getValue();
					sizeDelta *= mZoom.getValue();

					// Calc stuff for key
					int keyPositionX = (int)((*pInitialKeyPositions)[i][j].x + positionDelta.x);
					int keyPositionY = (int)((*pInitialKeyPositions)[i][j].y + positionDelta.y);
					int keySize = (int)(glm::max(MINIMAL_KEY_SIZE, initialKeySize + sizeDelta));

					// Transform and size
					(*pKeys)[i][j]->transformAndSize(keyPositionX, keyPositionY, keySize);

					// Actual updating
					bool selected = (*pKeys)[i][j]->update(tpf, !mKeyboardRecovers && penetrated, getStyleValue(StylePropertyFloat::KeyboardKeySelectionDuration)); // do not calculate penetration for each key but use one from elements

					// Check for "key selected"
					if (selected) // && (*pKeys)[i][j]->isFocused())
					{
						// Save selection
						mSelectionClassification = Classification::PENDING;
						mpSelectedKey = (*pKeys)[i][j].get();

						// Just a reminder for the zoom to decrease
						mKeyboardRecovers = true;

						// Inform listener after updating
						mpNotificationQueue->enqueue(getId(), NotificationType::KEYBOARD_KEY_SELECTED);

						// Interaction notification
						notifyInteraction("KEY_SELECTED");
					}
				}
			}
		} // nothing selected

        return adaptiveScale;
    }

    void Keyboard::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyleValue(StylePropertyVec4::BackgroundColor).a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("color", getStyleValue(StylePropertyVec4::BackgroundColor));
            mpBackground->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
            mpBackground->draw();
        }

        // *** RENDER KEYS ***

        // Push scissor to prohibit keys to draw outside of element
        pushScissor(mX, mY, mWidth, mHeight);

        // Determine, which sub keymap to draw
        SubKeymap const * pSubKeymap = NULL;
        if (mBigCharactersActive)
        {
            pSubKeymap = &(mKeymaps[mCurrentKeymapIndex].bigKeys);
        }
        else
        {
            pSubKeymap = &(mKeymaps[mCurrentKeymapIndex].smallKeys);
        }

        // Draw the sub keymap
        for(const auto& rLine : *pSubKeymap)
        {
            for(const auto& rupKey : rLine)
            {
                rupKey->draw(
					getStyleValue(StylePropertyVec4::Color),
					getStyleValue(StylePropertyVec4::PickColor),
					getStyleValue(StylePropertyVec4::IconColor),
					getStyleValue(StylePropertyVec4::ThresholdColor),
                    // mThreshold.getValue() * rupKey->getFocusValue(),
					getStyleValue(StylePropertyVec4::SelectionColor),
                    getMultipliedDimmedAlpha());
            }
        }

        // Draw animation of pressed keys
        for (const auto& rPressedKey : mPressedKeys)
        {
            rPressedKey.second->draw(
                getStyleValue(StylePropertyVec4::Color),
                getStyleValue(StylePropertyVec4::PickColor),
                getStyleValue(StylePropertyVec4::IconColor),
                getStyleValue(StylePropertyVec4::ThresholdColor),
				getStyleValue(StylePropertyVec4::SelectionColor),
                getMultipliedDimmedAlpha() * rPressedKey.first);
        }

        popScissor();

        // Render superclass
        InteractiveElement::specialDraw();
    }

    void Keyboard::specialTransformAndSize()
    {
        // Sets initial key size and position on all keymaps and their submaps
        // Has the assumption, that in small and big sub keymap the amount of keys in each row are equal
        // Therefore, everywhere small keymap is used
        for (Keymap& rKeymap : mKeymaps)
        {
            // Get line with maximum count
            int maxCountInLine = -1;
            for (const auto& rLine : rKeymap.smallKeys)
            {
                int countInLine = (int)rLine.size();
                if (countInLine > maxCountInLine)
                {
                    maxCountInLine = countInLine;
                }
            }

            // Calculate size of keys
            int keySize;
            int maxHorizontalKeySize = (mWidth / maxCountInLine);
            maxHorizontalKeySize -= (int)(maxHorizontalKeySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE); // Substract distance
            int maxVerticalKeySize = (int)(mHeight / rKeymap.smallKeys.size());
            keySize = maxHorizontalKeySize > maxVerticalKeySize ? maxVerticalKeySize : maxHorizontalKeySize;
            int halfKeySize = keySize / 2;

            // Set size of keys in keymap
            rKeymap.initialKeySize = (float)keySize;

            // Calculate offset to center
            int xCenterOffset, yCenterOffset;
            xCenterOffset = (int)((mWidth - (maxCountInLine * (keySize + (keySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE)))) / 2.f);
            yCenterOffset = (int)((mHeight - (rKeymap.smallKeys.size() * keySize)) / 2.f);

            // Save initial key positions
            for (uint i = 0; i < rKeymap.smallKeys.size(); i++) // Go over lines
            {
                // Count of keys in current row
                int keyCount = (int)rKeymap.smallKeys[i].size();

                // Decide, whether keys are shifted in that line
                int xOffset = (int)(halfKeySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE);
                if ((keyCount - maxCountInLine) % 2 != 0)
                {
                    // Add shift
                    xOffset += halfKeySize;
                }

                // Go over keys to save initial position
                for (int j = 0; j < keyCount; j++)
                {
                    rKeymap.initialKeyPositions[i][j] = glm::vec2(
                        mX + xCenterOffset + halfKeySize + (j * keySize) + xOffset,
                        mY + yCenterOffset + halfKeySize + (i * keySize));

                    xOffset += (int)(keySize * KEYBOARD_HORIZONTAL_KEY_DISTANCE);

                    // Set it for keymaps, may be overridden by update later that frame
                    glm::ivec2 pos = (glm::ivec2)(rKeymap.initialKeyPositions[i][j]);
                    rKeymap.smallKeys[i][j]->transformAndSize(pos.x, pos.y, (int)(rKeymap.initialKeySize));
                    rKeymap.bigKeys[i][j]->transformAndSize(pos.x, pos.y, (int)(rKeymap.initialKeySize));
                }
            }
        }
    }

    void Keyboard::specialReset()
    {
        // Call super
        InteractiveElement::specialReset();

        mFilteredGazePosition = glm::vec2(0,0);
        mPressedKeys.clear();

        resetKeymapsAndState();

        // Following should probably be not reset
        // mCurrentKeymapIndex = 0;
        // mBigCharactersActive = false;
    }

    void Keyboard::specialInteract()
    {
        // TODO: Interact with selected one, only.
        if (mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
        {
            if (mBigCharactersActive)
            {
                pressKey(mKeymaps[mCurrentKeymapIndex].bigKeys[mFocusedKeyRow][mFocusedKeyColumn].get());
            }
            else
            {
                pressKey(mKeymaps[mCurrentKeymapIndex].smallKeys[mFocusedKeyRow][mFocusedKeyColumn].get());
            }

			// Reset selection
			mKeyboardRecovers = true;
			if (mSelectionClassification != Classification::NO_SELECTION)
			{
				mpSelectedKey->unselect();
				mSelectionClassification = Classification::NO_SELECTION;
				mpSelectedKey = nullptr;
			}
        }
    }

    void Keyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
		case NotificationType::KEYBOARD_KEY_SELECTED:
		{
			// Check, whether there is still a selected key
			if (mpSelectedKey != nullptr)
			{
				// Get value of selected key
				std::u16string selectedKeyValue16 = mpSelectedKey->getValue();

				// Notify listener about selection with UTF-16 string
				notifyListener(&KeyboardListener::keySelected, pLayout, getId(), selectedKeyValue16);

				// Convert selected key's value to UTF-8 string
				std::string selectedKeyValue8;
				convertUTF16ToUTF8(selectedKeyValue16, selectedKeyValue8);

				// Notify listener about selection with UTF-8 string
				notifyListener(&KeyboardListener::keySelected, pLayout, getId(), selectedKeyValue8);
			}
			break;
		}
        case NotificationType::KEYBOARD_KEY_PRESSED:
        {
            // Notify listener method with UTF-16 string
            notifyListener(&KeyboardListener::keyPressed, pLayout, getId(), mLastPressedKeyValue);

            // Convert last pressed value to UTF-8 string
            std::string lastPressedKeyValue8;
            convertUTF16ToUTF8(mLastPressedKeyValue, lastPressedKeyValue8);

            // Notify listener method with UTF-8 string
            notifyListener(&KeyboardListener::keyPressed, pLayout, getId(), lastPressedKeyValue8);
            break;
        }
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Keyboard got notification which is not thought for it.");
            break;
        }
    }

    void Keyboard::fillKeymaps(KeyboardLayout layout)
    {
		// Before starting, clear current maps
		mKeymaps.clear();

		// Depending on layout, fill primary keymap
        switch (layout)
        {
        case KeyboardLayout::US_ENGLISH:

            // Primary keymap
            mKeymaps.push_back(Keymap());
            addKeys(
                mKeymaps.back().smallKeys,
                mKeymaps.back().bigKeys,
                mKeymaps.back().initialKeyPositions,
                std::vector<std::vector<CPair> >
            {
				{ CPair(u'1'), CPair(u'2'), CPair(u'3'), CPair(u'4'), CPair(u'5'), CPair(u'6'), CPair(u'7'), CPair(u'8'), CPair(u'9'), CPair(u'0'), CPair(u'#') },
				{ CPair(u'q', u'Q'), CPair(u'w', u'W'), CPair(u'e', u'E'), CPair(u'r', u'R'), CPair(u't', u'T'), CPair(u'y', u'Y'), CPair(u'u', u'U'), CPair(u'i', u'I'), CPair(u'o', u'O'), CPair(u'p', u'P'), CPair(u'-', u'_'), CPair(u'$') },
				{ CPair(u'a', u'A'), CPair(u's', u'S'), CPair(u'd', u'D'), CPair(u'f', u'F'), CPair(u'g', u'G'), CPair(u'h', u'H'), CPair(u'j', u'J'), CPair(u'k', u'K'), CPair(u'l', u'L'), CPair(u'/'), CPair(u'\'') },
				{ CPair(u'@'), CPair(u'z', u'Z'), CPair(u'x', u'X'), CPair(u'c', u'C'), CPair(u'v', u'V'), CPair(u'b', u'B'), CPair(u'n', u'N'), CPair(u'm', u'M'), CPair(u',', u';'), CPair(u'.', u':'), CPair(u'?'), CPair(u'!'), }
            });

            break;
        case KeyboardLayout::GERMANY_GERMAN:

            // Primary keymap
            mKeymaps.push_back(Keymap());
            addKeys(
                mKeymaps.back().smallKeys,
                mKeymaps.back().bigKeys,
                mKeymaps.back().initialKeyPositions,
                std::vector<std::vector<CPair> >
            {
                { CPair(u'1'), CPair(u'2'), CPair(u'3'), CPair(u'4'), CPair(u'5'), CPair(u'6'), CPair(u'7'), CPair(u'8'), CPair(u'9'), CPair(u'0'), CPair(u'\u00df') },
                { CPair(u'q', u'Q'), CPair(u'w', u'W'), CPair(u'e', u'E'), CPair(u'r', u'R'), CPair(u't', u'T'), CPair(u'z', u'Z'), CPair(u'u', u'U'), CPair(u'i', u'I'), CPair(u'o', u'O'), CPair(u'p', u'P'), CPair(u'\u00fc', u'\u00dc'), CPair(u'\u20AC') },
                { CPair(u'a', u'A'), CPair(u's', u'S'), CPair(u'd', u'D'), CPair(u'f', u'F'), CPair(u'g', u'G'), CPair(u'h', u'H'), CPair(u'j', u'J'), CPair(u'k', u'K'), CPair(u'l', u'L'), CPair(u'\u00f6', u'\u00d6'), CPair(u'\u00e4', u'\u00c4') },
                { CPair(u'@'), CPair(u'y', u'Y'), CPair(u'x', u'X'), CPair(u'c', u'C'), CPair(u'v', u'V'), CPair(u'b', u'B'), CPair(u'n', u'N'), CPair(u'm', u'M'), CPair(u',', u';'), CPair(u'.', u':'), CPair(u'?'), CPair(u'!') }
            });

            break;
		case KeyboardLayout::ISRAEL_HEBREW:

			// Primary keymap
			mKeymaps.push_back(Keymap());
			addKeys(
				mKeymaps.back().smallKeys,
				mKeymaps.back().bigKeys,
				mKeymaps.back().initialKeyPositions,
				std::vector<std::vector<CPair> >
			{
				{ CPair(u'1'), CPair(u'2'), CPair(u'3'), CPair(u'4'), CPair(u'5'), CPair(u'6'), CPair(u'7'), CPair(u'8'), CPair(u'9'), CPair(u'0'), CPair(u'#') },
				{ CPair(u'/', u'Q'), CPair(u'\u05F3', u'W'), CPair(u'\u05E7', u'E'), CPair(u'\u05E8', u'R'), CPair(u'\u05D0', u'T'), CPair(u'\u05D8', u'Y'), CPair(u'\u05D5', u'U'), CPair(u'\u05DF', u'I'), CPair(u'\u05DD', u'O'), CPair(u'\u05E4', u'P'), CPair(u'-', u'_'), CPair(u'\u20AA') },
				{ CPair(u'\u05E9', u'A'), CPair(u'\u05D3', u'S'), CPair(u'\u05D2', u'D'), CPair(u'\u05DB', u'F'), CPair(u'\u05E2', u'G'), CPair(u'\u05D9', u'H'), CPair(u'\u05D7', u'J'), CPair(u'\u05DC', u'K'), CPair(u'\u05DA', u'L'), CPair(u'\u05E3'), CPair(u'/') },
				{ CPair(u'@'), CPair(u'\u05D6', u'Z'), CPair(u'\u05E1', u'X'), CPair(u'\u05D1', u'C'), CPair(u'\u05D4', u'V'), CPair(u'\u05E0', u'B'), CPair(u'\u05DE', u'N'), CPair(u'\u05E6', u'M'), CPair(u'\u05EA'), CPair(u'\u05E5'), CPair(u',', u';'), CPair(u'.', u':') }
			});

			break;
		case KeyboardLayout::GREECE_GREEK:

			// Primary keymap
			mKeymaps.push_back(Keymap());
			addKeys(
				mKeymaps.back().smallKeys,
				mKeymaps.back().bigKeys,
				mKeymaps.back().initialKeyPositions,
				std::vector<std::vector<CPair> >
			{
				{ CPair(u'T'), CPair(u'O'), CPair(u'D'), CPair(u'O') },
			});

			break;
        default:
            throwError(OperationNotifier::Operation::BUG, "Tried to initialize keyboard of unknown layout");
        }

		// Secondary keymap
		mKeymaps.push_back(Keymap());
		addKeys(
			mKeymaps.back().smallKeys,
			mKeymaps.back().bigKeys,
			mKeymaps.back().initialKeyPositions,
			std::vector<std::vector<CPair> >
		{
			{ CPair(u'+'), CPair(u'-'), CPair(u'$'), CPair(u'#')},
			{ CPair(u'['), CPair(u']'), CPair(u'{'), CPair(u'}'), CPair(u'%') },
			{ CPair(u'<'), CPair(u'>'), CPair(u'('), CPair(u')') },
		});

        // Transform and size has to be called to set initial key size (depending on size of element)
    }

    void Keyboard::addKeys(SubKeymap& rSmallKeys, SubKeymap& rBigKeys, PositionMap& rInitialPositions, const std::vector<std::vector<CPair> >& input)
    {
        // Go over lines of input
        for (const auto& rLine : input)
        {
            newLine(rSmallKeys, rBigKeys, rInitialPositions);

            // Go over keys inside line
            for (const auto& rKey : rLine)
            {
                addKey(rSmallKeys, rBigKeys, rInitialPositions, rKey.small, rKey.big);
            }
        }
    }

    void Keyboard::addKey(SubKeymap& rSmallKeys, SubKeymap& rBigKeys, PositionMap& rInitialPositions, char16_t smallCharacter, char16_t bigCharacter) const
    {
        rSmallKeys[rSmallKeys.size() - 1].push_back(std::move(mpAssetManager->createKey(mpLayout, smallCharacter)));
        rBigKeys[rBigKeys.size() - 1].push_back(std::move(mpAssetManager->createKey(mpLayout, bigCharacter)));
        rInitialPositions[rInitialPositions.size() - 1].push_back(glm::vec2());
    }

    void Keyboard::newLine(SubKeymap& rSmallKeys, SubKeymap& rBigKeys, PositionMap& rInitialPositions) const
    {
        rSmallKeys.push_back(std::vector<std::unique_ptr<Key> >());
        rBigKeys.push_back(std::vector<std::unique_ptr<Key> >());
        rInitialPositions.push_back(std::vector<glm::vec2>());
    }

    void Keyboard::resetKeymapsAndState()
    {
        mZoom.setValue(0);
		mKeyboardRecovers = false;
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFastBuffer = u"";
        mLastFastKeyRow = -1;
        mLastFastKeyColumn = -1;
		mpSelectedKey = nullptr;
		mSelectionClassification = Classification::NO_SELECTION;

        // Reset keys on all keymaps
        for (const auto& rKeymap : mKeymaps)
        {
            // Go over sub keymap with small keys
            for (const auto& rLine : rKeymap.smallKeys)
            {
                for (const auto& rKey : rLine)
                {
                    rKey->reset();
                }
            }

            // Go over sub keymap with big keys
            for (const auto& rLine : rKeymap.bigKeys)
            {
                for (const auto& rKey : rLine)
                {
                    rKey->reset();
                }
            }
        }
    }

    void Keyboard::pressKey(Key* pKey)
    {
        // Get value of pressed key
        std::u16string pressedValue = pKey->getValue();

        if (mUseFastTyping)
        {
            // If fast typing is used, init last pressed key value with buffer
            mLastPressedKeyValue = mFastBuffer;

            // Check whether one has to add pressed key as well, because only picked keys were added so far
            if (mFocusedKeyRow != mLastFastKeyRow && mFocusedKeyColumn != mLastFastKeyColumn)
            {
                mLastPressedKeyValue.append(pressedValue);
            }
        }
        else // no fast typing
        {
            // Save value in member to have it when notification queue calls the pipe method
            mLastPressedKeyValue = pressedValue;
        }

        // Reset fast buffer
        mFastBuffer = u"";
        mLastFastKeyRow = -1;
        mLastFastKeyColumn = -1;

        // Inform listener after updating
        mpNotificationQueue->enqueue(getId(), NotificationType::KEYBOARD_KEY_PRESSED);

        // Save pressed key for nice animation
        std::unique_ptr<Key> upPressedKey = std::unique_ptr<Key>(new CharacterKey(*(CharacterKey*)(pKey)));
        upPressedKey->transformAndSize();
        mPressedKeys.push_back(PressedKey(1.f, std::move(upPressedKey)));

        // Interaction notification
        notifyInteraction("KEY_PRESS", convertUTF16ToUTF8(mLastPressedKeyValue));
    }
}
