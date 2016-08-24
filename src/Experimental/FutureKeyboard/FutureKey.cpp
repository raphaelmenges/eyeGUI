//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureKey.h"

#include "src/Rendering/AssetManager.h"
#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Rendering/ScissorStack.h"

namespace eyegui
{
    FutureKey::FutureKey(
        std::string id,
        Layout const * pLayout,
        AssetManager* pAssetManager,
        std::u16string letter,
        float letterScale,
        KeyboardCase keyCase)
	{
		// Initialize members
		mX = 0;
		mY = 0;
		mWidth = 0;
		mHeight = 0;
        mFirstThreshold.setValue(0);
        mSecondThreshold.setValue(0);
        mDoingSecondThreshold = false;
        mLetterFading.setValue(0);

		// Save members
        mId = id;
		mpLayout = pLayout;
		mpAssetManager = pAssetManager;
        mLetter = letter;
        mLetterScale = letterScale;
        mKeyCase = keyCase;

		// Fetch render items
        mpKeyItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
        mpSuggestionBackgroundItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
		mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_THRESHOLD, meshes::Type::QUAD);

        // Simple text for letter
        switch(mKeyCase)
        {
        case KeyboardCase::LOWER:
            toLower(mLetter);
            mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, mLetter);
            break;
        case KeyboardCase::UPPER:
            toUpper(mLetter);
            mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, mLetter);
            break;
        }

        // Simple text for suggestion
        mupSuggestion = mpAssetManager->createTextSimple(FontSize::SMALL, 1.f, u"suggestion");
	}

	FutureKey::~FutureKey()
	{

	}

	void FutureKey::transformAndSize(int x, int y, int width, int height)
	{
        // Key
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;

        // Letter
        transformLetter();

        // Suggestion
        transformSuggestion();
	}

    FutureKey::HitType FutureKey::update(float tpf, Input const * pInput)
	{
        // Return value
        HitType value = HitType::NONE;

        // Decide penetration
        bool penetrated = false;
        if(pInput != NULL)
        {
            penetrated = (pInput->gazeX >= mX
                && pInput->gazeX < mX + mWidth
                && pInput->gazeY >= mY
                && pInput->gazeY < mY + mHeight);
        }

        // Update threshold
        if(!mDoingSecondThreshold)
        {
            // First threshold (TODO: adjust threshold time)
            mFirstThreshold.update(tpf, !penetrated);

            // Fading of letter when second threshold is active should be decreased since second threshold is not ongoing
            mLetterFading.update(2.f * -tpf);
        }
        else
        {
            // Second threshold. But wait for fading of letter! (TODO: adjust threshold time)
            mSecondThreshold.update(tpf, !penetrated || (mLetterFading.getValue() < 1.f));

            // Fading of letter when second threshold is ongoing
            mLetterFading.update(2.f * tpf);
        }

        // Decide whether some threshold is reached
        if(!mDoingSecondThreshold)
        {
            // First threshold
            if(mFirstThreshold.getValue() >= 1.f)
            {
                mFirstThreshold.setValue(0.f);
                mDoingSecondThreshold = true;
                value = HitType::LETTER;
            }
        }
        else
        {
            // Second threshold
            if(mSecondThreshold.getValue() >= 1.f)
            {
                mSecondThreshold.setValue(0.f);
                mDoingSecondThreshold = false;
                value = HitType::SUGGESTION;
            }
        }

        // Just update suggestion transformation each frame
        transformSuggestion();

        // Return value
        return value;
	}

	void FutureKey::draw(float alpha) const
	{
		// Push scissor to limit rendering to current key (especially threshold)
		pushScissor(mX, mY, mWidth, mHeight);

        // *** DRAW KEY ***
        mpKeyItem->bind();
        mpKeyItem->getShader()->fillValue(
        "matrix",
        calculateDrawMatrix(
            mpLayout->getLayoutWidth(),
            mpLayout->getLayoutHeight(),
            mX,
            mY,
            mWidth,
            mHeight));
        mpKeyItem->getShader()->fillValue("color", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
        mpKeyItem->getShader()->fillValue("alpha", alpha);
        mpKeyItem->draw();

        // *** DRAW LETTER ***
        mupLetter->draw(glm::vec4(1,1,1,1), alpha * (1.f - (LETTER_FADING_MULTIPLIER * mLetterFading.getValue())), false, 0, 0);

        // *** DRAW SUGGESTION BACKGROUND ***
        float suggestionHeight = glm::mix(SUGGESTION_HEIGHT, 1.f, mSecondThreshold.getValue());
        mpSuggestionBackgroundItem->bind();
        mpSuggestionBackgroundItem->getShader()->fillValue(
        "matrix",
        calculateDrawMatrix(
            mpLayout->getLayoutWidth(),
            mpLayout->getLayoutHeight(),
            mX,
            mY + ((1.f - suggestionHeight) * mHeight),
            mWidth,
            (suggestionHeight * mHeight) + 1));
        mpSuggestionBackgroundItem->getShader()->fillValue("color", glm::vec4(0.3f, 0.3f, 0.3f, 1.f));
        mpSuggestionBackgroundItem->getShader()->fillValue("alpha", alpha);
        mpSuggestionBackgroundItem->draw();

        // *** DRAW SUGGESTION ***
        mupSuggestion->draw(glm::vec4(0.8f, 0.8f, 0.8f, 1.f), alpha, false, 0, 0);

        // *** DRAW THRESHOLD ***

		// Calculate draw matrix for threshold
        int thresholdSize = glm::sqrt((mWidth * mWidth) + (mHeight * mHeight));
        int thresholdX = mX - (int)((thresholdSize - mWidth) / 2.f);
        int thresholdY = mY - (int)((thresholdSize - mHeight) / 2.f);
		glm::mat4 thresholdDrawMatrix = calculateDrawMatrix(
			mpLayout->getLayoutWidth(),
			mpLayout->getLayoutHeight(),
			thresholdX,
			thresholdY,
            thresholdSize,
            thresholdSize);

		// Draw threshold
		mpThresholdItem->bind();
		mpThresholdItem->getShader()->fillValue("matrix", thresholdDrawMatrix);
        mpThresholdItem->getShader()->fillValue("thresholdColor", glm::vec4(0.f, 1.f, 1.f, 1.f));
        mpThresholdItem->getShader()->fillValue("threshold", mFirstThreshold.getValue());
        mpThresholdItem->getShader()->fillValue("alpha", 0.5f * alpha);
		mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
		mpThresholdItem->draw();

		// Pop scissor again
		popScissor();
	}

	void FutureKey::reset()
	{
        mFirstThreshold.setValue(0);
        mSecondThreshold.setValue(0);
        mDoingSecondThreshold = false;
        mLetterFading.setValue(0);
	}

    void FutureKey::setCase(KeyboardCase keyCase)
    {
        mKeyCase = keyCase;

        // Load correct letter
        switch(mKeyCase)
        {
        case KeyboardCase::LOWER:
            toLower(mLetter);
            mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, mLetter);
            break;
        case KeyboardCase::UPPER:
            toUpper(mLetter);
            mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, mLetter);
            break;
        }

        // Call transform of letter
        transformLetter();
    }

    void FutureKey::transformLetter()
    {
        mupLetter->transform(); // has to be called first to calculate width and height
        int letterX = mX + ((mWidth - mupLetter->getWidth()) / 2);
        int letterY = mY + (LETTER_Y_OFFSET_MULTIPLIER * ((((1.f - SUGGESTION_HEIGHT) * mHeight) - mupLetter->getHeight()) / 2));
        mupLetter->setPosition(letterX, letterY);
    }

    void FutureKey::transformSuggestion()
    {
        mupSuggestion->transform(); // has to be called first to calculate width and height
        int suggestionX = mX + (mWidth - mupSuggestion->getWidth()) / 2;

        // Standard height of suggestion
        int suggestionStandardY = mY + (mHeight * (1.f - SUGGESTION_HEIGHT)); // move to correct area
        suggestionStandardY += ((SUGGESTION_HEIGHT * mHeight) - mupSuggestion->getHeight()) / 2; // center in area

        // Height of suggestion when in center of complete key
        int suggestionCenterY = mY + (mHeight - mupSuggestion->getHeight()) / 2;

        // Set position
        mupSuggestion->setPosition(suggestionX, glm::mix(suggestionStandardY, suggestionCenterY, mSecondThreshold.getValue()));
    }
}
