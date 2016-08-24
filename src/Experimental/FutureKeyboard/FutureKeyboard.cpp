// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureKeyboard.h"

#include "src/Layout.h"
#include "src/Utilities/Helper.h"

namespace eyegui
{
	FutureKeyboard::FutureKeyboard(
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
		mType = Type::FUTURE_KEYBOARD;

        // Initialize members
        mMode = Mode::SUGGESTION_PER_KEY;
        mContent = u"";

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Initialize keys
        std::function<std::shared_ptr<FutureKey>(std::string, std::u16string, bool, bool)> createFutureKey =
            [&](std::string id, std::u16string letter, bool showSuggestion, bool ignoreCase)
            {
                auto spNewKey = std::shared_ptr<FutureKey>(
                    new FutureKey(
                        id,
                        mpLayout,
                        mpAssetManager,
                        letter,
                        0.8f,
                        showSuggestion,
                        KeyboardCase::UPPER,
                        ignoreCase)); // create new key
                mKeyList.push_back(spNewKey); // save smart pointer to list
                return spNewKey;
            };

        // First row
        mspQKey = createFutureKey("q", u"Q", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspWKey = createFutureKey("w", u"W", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspEKey = createFutureKey("e", u"E", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspRKey = createFutureKey("r", u"R", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspTKey = createFutureKey("t", u"T", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspYKey = createFutureKey("y", u"Y", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspUKey = createFutureKey("u", u"U", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspIKey = createFutureKey("i", u"I", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspOKey = createFutureKey("o", u"O", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspPKey = createFutureKey("p", u"P", mMode == Mode::SUGGESTION_PER_KEY, false);

        // Second row
        mspAKey = createFutureKey("a", u"A", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspSKey = createFutureKey("s", u"S", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspDKey = createFutureKey("d", u"D", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspFKey = createFutureKey("f", u"F", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspGKey = createFutureKey("g", u"G", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspHKey = createFutureKey("h", u"H", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspJKey = createFutureKey("j", u"J", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspKKey = createFutureKey("k", u"K", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspLKey = createFutureKey("l", u"L", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspEnterKey = createFutureKey("return", u"\u21AA", false, true);

        // Third row
        mspZKey = createFutureKey("z", u"Z", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspXKey = createFutureKey("x", u"X", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspCKey = createFutureKey("c", u"C", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspVKey = createFutureKey("v", u"V", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspBKey = createFutureKey("b", u"B", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspNKey = createFutureKey("n", u"N", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspMKey = createFutureKey("m", u"M", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspBackspaceKey = createFutureKey("backspace", u"\u21A4", false, true);

        // Forth row
        mspShiftKey = createFutureKey("shift", u"\u2191", false, true);
        mspRepeatKey = createFutureKey("repeat", u"\u21BA", false, true);
        mspCommaKey = createFutureKey("comma", u",", false, true);
        mspSpaceKey = createFutureKey("space", u"\u2423", false, true);
        mspDotKey = createFutureKey("dot", u".", false, true);
        mspQuestionKey = createFutureKey("question", u"?", false, true);
        mspExclamationKey = createFutureKey("exclamation", u"!", false, true);
        mspColonKey = createFutureKey("colon", u":", false, true);

        // Diplay
        mupDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);
        updateDisplay();

        // Pre display
        mupPreDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP, 1.f, u"Lorem ipsum.");
	}

	FutureKeyboard::~FutureKeyboard()
	{

	}

	float FutureKeyboard::specialUpdate(float tpf, Input* pInput)
	{
        // Prepare tasks
        enum class KeyTask { TOGGLE_CASE };
        std::vector<KeyTask> tasks;

        // Go over keys and update them
        for(auto& rspKey : mKeyList)
        {
            // Record whether hit
            FutureKey::HitType type = rspKey->update(tpf, pInput);

            // *** TASKS ***

            // Toggle case
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "shift")
            {
                tasks.push_back(KeyTask::TOGGLE_CASE);
            }

            // *** LETTERS ***
            if(type == FutureKey::HitType::LETTER
            && rspKey->getId() != "return"
            && rspKey->getId() != "backspace"
            && rspKey->getId() != "shift"
            && rspKey->getId() != "repeat"
            && rspKey->getId() != "space")
            {
                // Seems to be standard letter, just add it to content
                mContent.append(rspKey->getLetter());
                updateDisplay();
            }

            // *** SUGGESTIONS ***
            if(type == FutureKey::HitType::SUGGESTION)
            {
                mContent.append(rspKey->getSuggestion());
                updateDisplay();
            }

        }

        // Execute task after updating all keys
        for(auto task : tasks)
        {
            switch(task)
            {
                case KeyTask::TOGGLE_CASE:
                    for(auto& rspKey : mKeyList)
                    {
                        rspKey->toggleCase();
                    }
                    break;
            }
        }

		return 0.f;
	}

	void FutureKeyboard::specialDraw() const
	{
		// *** BACKGROUND ***
		if (getStyle()->backgroundColor.a > 0)
		{
			// Bind, fill and draw background
			mpBackground->bind();
			mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("color", glm::vec4(0.1f, 0.1f, 0.1f, 1));
			mpBackground->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
			mpBackground->draw();
		}

		// *** KEYS ***
        for(auto& rspKey : mKeyList)
        {
            rspKey->draw(getMultipliedDimmedAlpha());
        }

        // *** DISPLAY ***
        mupPreDisplay->draw(glm::vec4(0.3f,0.3f,0.3f,1), getMultipliedDimmedAlpha());;
        mupDisplay->draw(glm::vec4(1,1,1,1), getMultipliedDimmedAlpha());

	}

	void FutureKeyboard::specialTransformAndSize()
	{
        int xOffset = 0.01f * mWidth;
        int yOffset = 0.4f * mHeight;
        int keyWidth = 0.09f * mWidth;
        int keyHeight = 0.125f * mHeight;
        int keySpace = 0.01f * mWidth;

        // First row
        mspQKey->transformAndSize(xOffset + mX,                               yOffset + mY, keyWidth, keyHeight);
        mspWKey->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspEKey->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspRKey->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspTKey->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspYKey->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspUKey->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspIKey->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspOKey->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspPKey->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);

        // Second row
        mspAKey    ->transformAndSize(xOffset + mX,                               yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspSKey    ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspDKey    ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspFKey    ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspGKey    ->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspHKey    ->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspJKey    ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspKKey    ->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspLKey    ->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspEnterKey->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);

        // Third row
        mspZKey        ->transformAndSize(xOffset + mX,                               yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspXKey        ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCKey        ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspVKey        ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBKey        ->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspNKey        ->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspMKey        ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBackspaceKey->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);

        // Forth row
        mspShiftKey      ->transformAndSize(xOffset + mX,                               yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspRepeatKey     ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCommaKey      ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspSpaceKey      ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);
        mspDotKey        ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspQuestionKey   ->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspExclamationKey->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspColonKey      ->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);

        // Display
        mupDisplay->transformAndSize(mX, mY, mWidth, 0.2f * mHeight);

        // Pre display
        mupPreDisplay->transformAndSize(mX, mY, mWidth, 0.2f * mHeight);
	}

	void FutureKeyboard::specialReset()
	{

	}

	void FutureKeyboard::specialInteract()
	{

	}

	void FutureKeyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
	{

	}

    void FutureKeyboard::updateDisplay()
    {
        mupDisplay->setContent(mContent + u"_");
    }
}
