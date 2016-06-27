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
            RenderingMask::BOX,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::KEYBOARD;

        // Initialize members
        mThreshold.setValue(0);
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFocusPosition = glm::vec2(0,0);
        mGazePosition = glm::vec2(0,0);
        mKeyWasPressed = false;
        mCurrentKeymapIndex = 0;
        mBigCharactersActive = false;
        mLastFastKeyRow = -1;
        mLastFastKeyColumn = -1;
        mUseFastTyping = false;
        mFastBuffer = u"";

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::COLOR,
            meshes::Type::QUAD);

        // Create keymaps (mKeymaps)
        initKeymaps(mpLayout->getCharacterSet());
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
    }

    uint Keyboard::getCountOfKeymaps() const
    {
        return (uint)(mKeymaps.size());
    }

    void Keyboard::setKeymap(uint keymapIndex)
    {
        // Set keymap (must be bigger than zero because unsigned int used)
        if(keymapIndex >= getCountOfKeymaps())
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Keymap id does not exist in keyboard: " + getId());
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

    float Keyboard::specialUpdate(float tpf, Input* pInput)
    {
        // *** SET UP PARAMETERS ***

        // Radius stuff is given in key radii (since normalized with that value)

        // Some parameters which may or may not be defineable in config file
        float GAZE_DELTA_WEIGHT_RADIUS = 1.0f; // Radius in which gaze has to be so that threshold is increased
        float THRESHOLD_INCREASE_DURATION = 0.25f; // Duration of threshold to become one (depending very much on other parameters)

        // Not that important parameters
        float GAZE_FILTER_RADIUS = 5.f; // Radius in which the gaze is filtered. Outside of that radius, gaze data is took raw
        float GAZE_DIRECT_USAGE_MULTIPLIER = 10.f; // Multiplier for usage of raw gaze when outside filter area (take look at GAZE_FILTER_RADIUS)
        float PRESSED_KEY_SCALING_MULTIPLIER = 5.f; // Just animation scale of pressed key which is moving and fading towards user
        float THRESHOLD_DECREASE_AFTER_PRESS_DURATION = 0.2f; // Decrease of threshold after pressing
        float THRESHOLD_DECREASE_DURATION = 1.f; // General decrease duration of threshold if no gaze is upon element
        float FOCUS_RADIUS = 3.0f; // Radius of gaze affected keys (in the center bigger, else smaller)
        float KEY_POSITION_DELTA_MULTIPLIER = 0.25f; // Delta position multiplier of keys in focus
        float KEY_SIZE_DELTA_MULTIPLIER = 0.5f; // Delta size multiplier of keys in focus
        float MINIMAL_KEY_SIZE = 0.75f; // Cap minimal key size for those in focus area

        // Some adjustments for fast typing
        if (mUseFastTyping)
        {
            GAZE_DELTA_WEIGHT_RADIUS *= 0.3f;
            THRESHOLD_INCREASE_DURATION *= 1.5f;
        }

        // Use speed multiplier
        THRESHOLD_INCREASE_DURATION /= mpLayout->getConfig()->keyboardSpeedMultiplier;

        // *** SETUP ***

        // Call super
        InteractiveElement::specialUpdate(tpf, pInput);

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Get stuff from currently active keymap
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

        // *** UPDATE ANIMATED PRESSED KEYES ***
        std::vector<int> dyingPressedKeys;
        for (uint i = 0; i < mPressedKeys.size(); i++)
        {
            // Update alpha and size
            mPressedKeys[i].first -= tpf / INTERACTION_FADING_DURATION;
            Key* pKey = mPressedKeys[i].second.get();
            pKey->transformAndSize((int)pKey->getPosition().x, (int)pKey->getPosition().y, (int)(pKey->getSize() + PRESSED_KEY_SCALING_MULTIPLIER * tpf * initialKeySize));

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
        float gazeFilterRadius = GAZE_FILTER_RADIUS * initialKeySize;
        float rawGazeFilter = std::min(1.f, glm::abs(glm::length(rawGazeDelta)) / gazeFilterRadius); // 0 when filtering and 1 when direkt usage of gaze
        float gazeFilter = rawGazeFilter + (1.f - rawGazeFilter) * std::min(1.f, GAZE_DIRECT_USAGE_MULTIPLIER * tpf);
        mGazePosition += gazeFilter * rawGazeDelta;

        // Use gaze delta as weight for threshold (is one if low delta in gaze)
        float gazeDelta = glm::abs(glm::length(rawGazeDelta)); // In pixels!
        float gazeDeltaWeight = 1.f - clamp(gazeDelta / (GAZE_DELTA_WEIGHT_RADIUS * initialKeySize), 0, 1); // Key size used for normalization

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
            for(uint i = 0; i < pKeys->size(); i++)
            {
                for(uint j = 0; j < (*pKeys)[i].size(); j++)
                {
                    // Use position in keys to get position after last update
                    float currentDistance = glm::abs(glm::distance(mGazePosition, (*pKeys)[i][j]->getPosition()));
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
                    (*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(false);
                    (*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->setPicked(false);
                }

                // Set new focus
                mFocusedKeyRow = newFocusedKeyRow;
                mFocusedKeyColumn = newFocusedKeyColumn;
                (*pKeys)[mFocusedKeyRow][mFocusedKeyColumn]->setFocus(true);
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

        // *** UPDATE POSITION OF FOCUS ***

        // Update focus position
        if(mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
        {
            mFocusPosition = (*pInitialKeyPositions)[mFocusedKeyRow][mFocusedKeyColumn];
        }

        // *** UPDATE KEY POSITIONS ***

        // Update keys (they have to be transformed and sized each update)
        for(uint i = 0; i < pKeys->size(); i++)
        {
            for(uint j = 0; j < (*pKeys)[i].size(); j++)
            {
                // Get delta between position of initial key position and gaze position
                glm::vec2 positionDelta = (*pInitialKeyPositions)[i][j] - mGazePosition;

                // Radius of focus
                float focusWeight = 1.f - glm::length(positionDelta) / (FOCUS_RADIUS * initialKeySize); // Key size used for normalization
                focusWeight = clamp(focusWeight, 0, 1);

                // Only near keys have to be moved
                positionDelta *= focusWeight;
                positionDelta *= KEY_POSITION_DELTA_MULTIPLIER;

                // Calculate delta of size
                float sizeDelta = initialKeySize - glm::length((*pKeys)[i][j]->getPosition() - mGazePosition);
                sizeDelta = (1.f - focusWeight) + sizeDelta * focusWeight;
                sizeDelta = std::max(-(1.f - MINIMAL_KEY_SIZE) * initialKeySize, sizeDelta); // Subtracted from initial size
                sizeDelta *= KEY_SIZE_DELTA_MULTIPLIER;

                // Weight with threshold
                positionDelta *= mThreshold.getValue();
                sizeDelta *= mThreshold.getValue();

                // Calc stuff for key
                int keyPositionX = (int)((*pInitialKeyPositions)[i][j].x + positionDelta.x);
                int keyPositionY = (int)((*pInitialKeyPositions)[i][j].y + positionDelta.y);
                int keySize = (int)(initialKeySize + sizeDelta);

                // Transform and size
                (*pKeys)[i][j]->transformAndSize(keyPositionX, keyPositionY, keySize);

                // Updating
                (*pKeys)[i][j]->update(tpf);

                // Check for "key pressed"
                if(!mKeyWasPressed && mThreshold.getValue() >= 1.f && (*pKeys)[i][j]->isFocused())
                {
                    // Check, whether gaze is really on focused key
                    /*
                    if(
                        glm::abs(
                            glm::distance(
                                glm::vec2(keyPositionX, keyPositionY),
                                mGazePosition))
                        < keySize / 2)
                    */
                    {
                        // Press key
                        pressKey(pKeys, i, j);
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
            mpBackground->getShader()->fillValue("color", getStyle()->backgroundColor);
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
                    getStyle()->color,
                    getStyle()->pickColor,
                    getStyle()->iconColor,
                    getStyle()->thresholdColor,
                    mThreshold.getValue() * rupKey->getFocusValue(),
                    getMultipliedDimmedAlpha());
            }
        }

        // Draw animation of pressed keys
        for (const auto& rPressedKey : mPressedKeys)
        {
            rPressedKey.second->draw(
                getStyle()->color,
                getStyle()->pickColor,
                getStyle()->iconColor,
                getStyle()->thresholdColor,
                0,
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

        mFocusPosition = glm::vec2(0,0);
        mGazePosition = glm::vec2(0,0);
        mPressedKeys.clear();

        resetKeymapsAndState();

        // Following should be probably not reset
        // mCurrentKeymapIndex = 0;
        // mBigCharactersActive = false;
    }

    void Keyboard::specialInteract()
    {
        // Either currently focused or the one with highest combination of focus and threshold...first idea used here
        if (mFocusedKeyRow >= 0 && mFocusedKeyColumn >= 0)
        {
            if (mBigCharactersActive)
            {
                pressKey(&mKeymaps[mCurrentKeymapIndex].bigKeys, mFocusedKeyRow, mFocusedKeyColumn);
            }
            else
            {
                pressKey(&mKeymaps[mCurrentKeymapIndex].smallKeys, mFocusedKeyRow, mFocusedKeyColumn);
            }
        }
    }

    void Keyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
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

    void Keyboard::initKeymaps(CharacterSet set)
    {
        switch (set)
        {
        case CharacterSet::US_ENGLISH:

            // Primary keymap
            mKeymaps.push_back(Keymap());
            addKeys(
                mKeymaps.back().smallKeys,
                mKeymaps.back().bigKeys,
                mKeymaps.back().initialKeyPositions,
                std::vector<std::vector<CPair> >
            {
                { CPair(u'1'), CPair(u'2'), CPair(u'3'), CPair(u'4'), CPair(u'5'), CPair(u'6'), CPair(u'7'), CPair(u'8'), CPair(u'9'), CPair(u'0'), CPair(u'/') },
                { CPair(u'q', u'Q'), CPair(u'w', u'W'), CPair(u'e', u'E'), CPair(u'r', u'R'), CPair(u't', u'T'), CPair(u'y', u'Y'), CPair(u'u', u'U'), CPair(u'i', u'I'), CPair(u'o', u'O'), CPair(u'p', u'P'), CPair(u'+'), CPair(u'-') },
                { CPair(u'a', u'A'), CPair(u's', u'S'), CPair(u'd', u'D'), CPair(u'f', u'F'), CPair(u'g', u'G'), CPair(u'h', u'H'), CPair(u'j', u'J'), CPair(u'k', u'K'), CPair(u'l', u'L'), CPair(u'('), CPair(u')') },
                { CPair(u'@'), CPair(u'z', u'Z'), CPair(u'x', u'X'), CPair(u'c', u'C'), CPair(u'v', u'V'), CPair(u'b', u'B'), CPair(u'n', u'N'), CPair(u'm', u'M'), CPair(u'.'), CPair(u':'), CPair(u'?'), CPair(u'!') }
            });

            // Secondary keymap
            mKeymaps.push_back(Keymap());
            addKeys(
                mKeymaps.back().smallKeys,
                mKeymaps.back().bigKeys,
                mKeymaps.back().initialKeyPositions,
                std::vector<std::vector<CPair> >
            {
                { CPair(u'\u00A7'), CPair(u'$'), CPair(u'\u20AC'), CPair(u'#')},
                { CPair(u'['), CPair(u']'), CPair(u'{'), CPair(u'}'), CPair(u'%') },
                { CPair(u'<'), CPair(u'>'), CPair(u'('), CPair(u')')},
            });

            break;
        case CharacterSet::GERMANY_GERMAN:

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
                { CPair(u'@'), CPair(u'y', u'Y'), CPair(u'x', u'X'), CPair(u'c', u'C'), CPair(u'v', u'V'), CPair(u'b', u'B'), CPair(u'n', u'N'), CPair(u'm', u'M'), CPair(u'.'), CPair(u':'), CPair(u'!'), CPair(u'?') }
            });

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
                { CPair(u'<'), CPair(u'>'), CPair(u'('), CPair(u')')},
            });

            break;
        default:
            throwError(OperationNotifier::Operation::BUG, "Tried to initialize keyboard of unkown character set");
        }

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
        mThreshold.setValue(0);
        mKeyWasPressed = false;
        mFocusedKeyRow = -1;
        mFocusedKeyColumn = -1;
        mFastBuffer = u"";
        mLastFastKeyRow = -1;
        mLastFastKeyColumn = -1;

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

    void Keyboard::pressKey(SubKeymap* pKeys, int i, int j)
    {
        // Get value of pressed key
        std::u16string pressedValue = (*pKeys)[i][j]->getValue();

        if (mUseFastTyping)
        {
            // If fast typing is used, initial last pressed key value with buffer
            mLastPressedKeyValue = mFastBuffer;

            // Check whether one has to add pressed key as well, because only new picked keys were added so far
            if ((int)i != mLastFastKeyRow && (int)j != mLastFastKeyColumn)
            {
                mLastPressedKeyValue.append(pressedValue);
            }
        }
        else
        {
            // Save value in member to have it when notification queue calls the pipe method
            mLastPressedKeyValue = pressedValue;
        }

        // Reset fast buffer
        mFastBuffer = u"";
        mLastFastKeyRow = -1;
        mLastFastKeyColumn = -1;

        // Just a reminder for the threshold to decrease
        mKeyWasPressed = true;

        // Inform listener after updating
        mpNotificationQueue->enqueue(getId(), NotificationType::KEYBOARD_KEY_PRESSED);

        // Add pressed key for nice animation
        std::unique_ptr<Key> upPressedKey = std::unique_ptr<Key>(new CharacterKey(*(CharacterKey*)((*pKeys)[i][j].get())));
        upPressedKey->transformAndSize();
        mPressedKeys.push_back(PressedKey(1.f, std::move(upPressedKey)));
    }
}
