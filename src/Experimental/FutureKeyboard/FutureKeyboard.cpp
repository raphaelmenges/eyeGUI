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
        std::function<std::shared_ptr<FutureKey>(std::u16string)> createSuggestionKey =
            [&](std::u16string letter)
            {
                auto spNewKey = std::shared_ptr<FutureKey>(new FutureKey(mpLayout, mpAssetManager, letter)); // create new key
                mKeyList.push_back(spNewKey); // save smart pointer to list
                return spNewKey;
            };

        // First row
        mspQKey = createSuggestionKey(u"Q");
        mspWKey = createSuggestionKey(u"W");
        mspEKey = createSuggestionKey(u"E");
        mspRKey = createSuggestionKey(u"R");
        mspTKey = createSuggestionKey(u"T");
        mspYKey = createSuggestionKey(u"Y");
        mspUKey = createSuggestionKey(u"U");
        mspIKey = createSuggestionKey(u"I");
        mspOKey = createSuggestionKey(u"O");
        mspPKey = createSuggestionKey(u"P");

        // Second row
        mspAKey = createSuggestionKey(u"A");
        mspSKey = createSuggestionKey(u"S");
        mspDKey = createSuggestionKey(u"D");
        mspFKey = createSuggestionKey(u"F");
        mspGKey = createSuggestionKey(u"G");
        mspHKey = createSuggestionKey(u"H");
        mspJKey = createSuggestionKey(u"J");
        mspKKey = createSuggestionKey(u"K");
        mspLKey = createSuggestionKey(u"L");
        mspEnterKey = createSuggestionKey(u"Enter");

        // Third row
        mspZKey = createSuggestionKey(u"Z");
        mspXKey = createSuggestionKey(u"X");
        mspCKey = createSuggestionKey(u"C");
        mspVKey = createSuggestionKey(u"V");
        mspBKey = createSuggestionKey(u"B");
        mspNKey = createSuggestionKey(u"N");
        mspMKey = createSuggestionKey(u"M");
        mspBackspaceKey = createSuggestionKey(u"Back");

        // Forth row
        mspShiftKey = createSuggestionKey(u"Shift");
        mspRepeatKey = createSuggestionKey(u"Repeat");
        mspCommaKey = createSuggestionKey(u",");
        mspSpaceKey = createSuggestionKey(u"Space");
        mspDotKey = createSuggestionKey(u".");
        mspQuestionKey = createSuggestionKey(u"?");
        mspExclamationKey = createSuggestionKey(u"!");
        mspColonKey = createSuggestionKey(u":");
	}

	FutureKeyboard::~FutureKeyboard()
	{

	}

	float FutureKeyboard::specialUpdate(float tpf, Input* pInput)
	{
        for(auto& rspKey : mKeyList)
        {
            rspKey->update(tpf, pInput);
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
			mpBackground->getShader()->fillValue("color", glm::vec4(0,1,0,1)); // mpBackground->getShader()->fillValue("color", getStyle()->backgroundColor);
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
        int yOffset = 0.01f * mHeight;
        int keyWidth = 0.09f * mWidth;
        int keyHeight = 0.15f * mHeight;
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
