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
        std::function<std::unique_ptr<FutureKey>(std::u16string)> createSuggestionKey =
            [&](std::u16string letter)
            {
                return std::move(std::unique_ptr<FutureKey>(new FutureKey(mpLayout, mpAssetManager, letter)));
            };

        mupQKey = createSuggestionKey(u"Q");
        mupWKey = createSuggestionKey(u"W");
        mupEKey = createSuggestionKey(u"E");
        mupRKey = createSuggestionKey(u"R");
        mupTKey = createSuggestionKey(u"T");
        mupYKey = createSuggestionKey(u"Y");
	}

	FutureKeyboard::~FutureKeyboard()
	{

	}

	float FutureKeyboard::specialUpdate(float tpf, Input* pInput)
	{
        mupQKey->update(tpf, pInput);
        mupWKey->update(tpf, pInput);
        mupEKey->update(tpf, pInput);
        mupRKey->update(tpf, pInput);
        mupTKey->update(tpf, pInput);
        mupYKey->update(tpf, pInput);
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
        mupQKey->draw(getMultipliedDimmedAlpha());
        mupWKey->draw(getMultipliedDimmedAlpha());
        mupEKey->draw(getMultipliedDimmedAlpha());
        mupRKey->draw(getMultipliedDimmedAlpha());
        mupTKey->draw(getMultipliedDimmedAlpha());
        mupYKey->draw(getMultipliedDimmedAlpha());
	}

	void FutureKeyboard::specialTransformAndSize()
	{
        int keyWidth = 0.1f * mWidth;
        int keyHeight = 0.2f * mHeight;
        int keySpace = 0.02f * mWidth;
        mupQKey->transformAndSize(mX, mY, keyWidth, keyHeight);
        mupWKey->transformAndSize(mX + (1 * (keyWidth + keySpace)), mY, keyWidth, keyHeight);
        mupEKey->transformAndSize(mX + (2 * (keyWidth + keySpace)), mY, keyWidth, keyHeight);
        mupRKey->transformAndSize(mX + (3 * (keyWidth + keySpace)), mY, keyWidth, keyHeight);
        mupTKey->transformAndSize(mX + (4 * (keyWidth + keySpace)), mY, keyWidth, keyHeight);
        mupYKey->transformAndSize(mX + (5 * (keyWidth + keySpace)), mY, keyWidth, keyHeight);
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
