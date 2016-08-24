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

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Initialize keys
        std::function<std::shared_ptr<FutureKey>(std::string, std::u16string)> createSuggestionKey =
            [&](std::string id, std::u16string letter)
            {
                auto spNewKey = std::shared_ptr<FutureKey>(new FutureKey(id, mpLayout, mpAssetManager, letter, 0.8f)); // create new key
                mKeyList.push_back(spNewKey); // save smart pointer to list
                return spNewKey;
            };

        // First row
        mspQKey = createSuggestionKey("q", u"Q");
        mspWKey = createSuggestionKey("w", u"W");
        mspEKey = createSuggestionKey("e", u"E");
        mspRKey = createSuggestionKey("r", u"R");
        mspTKey = createSuggestionKey("t", u"T");
        mspYKey = createSuggestionKey("y", u"Y");
        mspUKey = createSuggestionKey("u", u"U");
        mspIKey = createSuggestionKey("i", u"I");
        mspOKey = createSuggestionKey("o", u"O");
        mspPKey = createSuggestionKey("p", u"P");

        // Second row
        mspAKey = createSuggestionKey("a", u"A");
        mspSKey = createSuggestionKey("s", u"S");
        mspDKey = createSuggestionKey("d", u"D");
        mspFKey = createSuggestionKey("f", u"F");
        mspGKey = createSuggestionKey("g", u"G");
        mspHKey = createSuggestionKey("h", u"H");
        mspJKey = createSuggestionKey("j", u"J");
        mspKKey = createSuggestionKey("k", u"K");
        mspLKey = createSuggestionKey("l", u"L");
        mspEnterKey = createSuggestionKey("Enter", u"\u23CE");

        // Third row
        mspZKey = createSuggestionKey("z", u"Z");
        mspXKey = createSuggestionKey("x", u"X");
        mspCKey = createSuggestionKey("c", u"C");
        mspVKey = createSuggestionKey("v", u"V");
        mspBKey = createSuggestionKey("b", u"B");
        mspNKey = createSuggestionKey("n", u"N");
        mspMKey = createSuggestionKey("m", u"M");
        mspBackspaceKey = createSuggestionKey("backspace", u"\u232B");

        // Forth row
        mspShiftKey = createSuggestionKey("shift", u"\u21E7");
        mspRepeatKey = createSuggestionKey("repeat", u"\u25CE");
        mspCommaKey = createSuggestionKey("comma", u",");
        mspSpaceKey = createSuggestionKey("space", u"\u2423");
        mspDotKey = createSuggestionKey("dot", u".");
        mspQuestionKey = createSuggestionKey("question", u"?");
        mspExclamationKey = createSuggestionKey("exclamation", u"!");
        mspColonKey = createSuggestionKey("colon", u":");
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

            // Toggle case
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "shift")
            {
                tasks.push_back(KeyTask::TOGGLE_CASE);
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
}
