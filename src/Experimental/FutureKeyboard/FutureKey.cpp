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
        bool showSuggestion,
        KeyboardCase keyCase,
        bool ignoreCase)
	{
		// Initialize members
		mX = 0;
		mY = 0;
		mWidth = 0;
		mHeight = 0;
        mFirstThreshold.setValue(0.f);
        mSecondThreshold.setValue(0.f);
        mDoingSecondThreshold = false;
        mLetterFading.setValue(0.f);
        mupSuggestion = NULL;
        mPressing.setValue(0.f);
        mSuggestionAnimation = std::make_pair(0.f, nullptr);
        mRetriggerTime = 0;
        mGazeDistanze = 0;

		// Save members
        mId = id;
		mpLayout = pLayout;
		mpAssetManager = pAssetManager;
        mLetterScale = letterScale;
        mShowSuggestion = showSuggestion;
        mKeyCase = keyCase;
        mIgnoreCase = ignoreCase;

		// Fetch render items
        mpKeyItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
        mpBackgroundItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
        mpSuggestionBackgroundItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
		mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_THRESHOLD, meshes::Type::QUAD);

        // Simple text for letter
        if(!mIgnoreCase)
        {
            switch(mKeyCase)
            {
            case KeyboardCase::LOWER:
                toLower(letter);
                break;
            case KeyboardCase::UPPER:
                toUpper(letter);
                break;
            }
        }
        mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, letter);

        // Simple text for suggestion
        if(mShowSuggestion) { mupSuggestion = mpAssetManager->createTextSimple(FontSize::SMALL, 1.f, u"suggestion"); }

		// Simple text for info
		mupInfo = mpAssetManager->createTextSimple(FontSize::SMALL, 1.f);
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

		// Info
		transformInfo();
	}

    FutureKey::HitType FutureKey::update(float tpf, Input const * pInput)
	{
        // Return value
        HitType value = HitType::NONE;

        // Update pressing
        mPressing.update(-tpf / PRESS_DURATION);

        // Update retrigger time
        mRetriggerTime -= tpf;
        mRetriggerTime = glm::max(0.f, mRetriggerTime);

        // Update animation of suggestion which was chosen
        if(mSuggestionAnimation.first > 0)
        {
            mSuggestionAnimation.first -= tpf;
            mSuggestionAnimation.first = std::max(0.f, mSuggestionAnimation.first);
        }

        // Process input
        bool penetrated = false;
        if(pInput != NULL)
        {
            // Decide penetration
            penetrated = (pInput->gazeX >= mX
                && pInput->gazeX < mX + mWidth
                && pInput->gazeY >= mY
                && pInput->gazeY < mY + mHeight);

            // Determine distance to gaze
            float centerX = mX + (mWidth / 2.f);
            float centerY = mY + (mHeight / 2.f);
            mGazeDistanze = glm::distance(glm::vec2(centerX, centerY), glm::vec2(pInput->gazeX, pInput->gazeY));
            mGazeDistanze /= mpLayout->getLayoutWidth(); // some random normalization
        }

        // Update threshold
        if(!mDoingSecondThreshold)
        {
            // First threshold (TODO: adjust threshold time)
            mFirstThreshold.update(tpf, !penetrated || (mRetriggerTime > 0.f));

            // Fading of letter when second threshold is active should be decreased since second threshold is not ongoing
            mLetterFading.update(-tpf / RETRIGGER_DELAY);
        }
        else
        {
            // Second threshold. But wait for fading of letter! (TODO: adjust threshold time)
            mSecondThreshold.update(tpf, !penetrated || (mRetriggerTime > 0.f));

            // Fading of letter when second threshold is ongoing
            mLetterFading.update(tpf / RETRIGGER_DELAY);
        }

        // Decide whether some threshold is reached
        if(!mDoingSecondThreshold)
        {
			// First threshold
            if(mFirstThreshold.getValue() >= 1.f)
            {
                mFirstThreshold.setValue(0.f);
                mDoingSecondThreshold = 
					mShowSuggestion
					&& mupSuggestion != nullptr
					&& !mupSuggestion->getContent().empty(); // only start second threshold when suggestion is shown and available
                value = HitType::LETTER;
                mPressing.setValue(1.f);
                mRetriggerTime = RETRIGGER_DELAY;
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
                mRetriggerTime = RETRIGGER_DELAY;

                // Copy suggestion for animation
                mSuggestionAnimation = std::make_pair(
                    SUGGESTION_ANIMATION_DURATION,
                    mpAssetManager->createTextSimple(
                        FontSize::SMALL, 1.f, mupSuggestion->getContent()));
            }
        }

        // Just update suggestion transformation each frame, inclusive the animation
        transformSuggestion();

        // Return value
        return value;
	}

    void FutureKey::draw(
        glm::vec4 keyColor,
        glm::vec4 fontColor,
        glm::vec4 suggestionBackgroundColor,
        glm::vec4 thresholdColor,
        float alpha) const
	{
        // *** DRAW BACKGROUND ***
        mpBackgroundItem->bind();
        mpBackgroundItem->getShader()->fillValue(
            "matrix",
            calculateDrawMatrix(
                mpLayout->getLayoutWidth(),
                mpLayout->getLayoutHeight(),
                mX - BACKGROUND_PIXEL_BULGE,
                mY - BACKGROUND_PIXEL_BULGE,
                mWidth + (BACKGROUND_PIXEL_BULGE * 2),
                mHeight + (BACKGROUND_PIXEL_BULGE * 2)));
        glm::vec3 compositeBackgroundColor = glm::min(glm::vec3(1,1,1), 0.1f + glm::vec3(keyColor.r, keyColor.g, keyColor.b));
        mpBackgroundItem->getShader()->fillValue("color", glm::vec4(compositeBackgroundColor, 1.f));
        mpBackgroundItem->getShader()->fillValue("alpha", alpha);
        mpBackgroundItem->draw();

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
        float keyColorMultiplier = (1.f - (glm::sin(mPressing.getValue() * 3.14f)));
        glm::vec3 compositeKeyColor = keyColorMultiplier * glm::vec3(keyColor.r, keyColor.g, keyColor.b);
        mpKeyItem->getShader()->fillValue("color", glm::vec4(compositeKeyColor, 1.f));
        mpKeyItem->getShader()->fillValue("alpha", alpha);
        mpKeyItem->draw();

        // *** DRAW LETTER ***
        mupLetter->draw(fontColor, alpha * (1.f - (LETTER_FADING_MULTIPLIER * mLetterFading.getValue())), false, 0, 0);

        if(mShowSuggestion)
        {
            // *** DRAW SUGGESTION BACKGROUND ***
            float suggestionHeight = glm::mix(SUGGESTION_HEIGHT, 1.f, mSecondThreshold.getValue());
            mpSuggestionBackgroundItem->bind();
            mpSuggestionBackgroundItem->getShader()->fillValue(
                "matrix",
                calculateDrawMatrix(
                    mpLayout->getLayoutWidth(),
                    mpLayout->getLayoutHeight(),
                    mX,
                    mY + (int)(((1.f - suggestionHeight) * mHeight)),
                    mWidth,
                    (int)((suggestionHeight * (float)mHeight) + 1.f)));
            mpSuggestionBackgroundItem->getShader()->fillValue("color", suggestionBackgroundColor);
            mpSuggestionBackgroundItem->getShader()->fillValue("alpha", alpha);
            mpSuggestionBackgroundItem->draw();

            // *** DRAW SUGGESTION ***
            float suggestionAlpha =
                (1.f - glm::clamp(GAZE_DISTANCE_MULTIPLIER * mGazeDistanze, 0.f, 1.f))
                * ((1.f - LETTER_FADING_MULTIPLIER)
                    + (LETTER_FADING_MULTIPLIER * mLetterFading.getValue()))
                * alpha;
            mupSuggestion->draw(fontColor, suggestionAlpha, false, 0, 0);

            // *** DRAW ANIMATION OF CHOSEN SUGGESTION ***
            if(mSuggestionAnimation.first > 0 && mSuggestionAnimation.second != NULL)
            {
                mSuggestionAnimation.second->draw(fontColor, (mSuggestionAnimation.first / SUGGESTION_ANIMATION_DURATION) * alpha, false, 0, 0);
            }
        }

		// *** DRAW INFO ***
		mupInfo->draw(fontColor, alpha);

        // *** DRAW THRESHOLD ***

		// Calculate draw matrix for threshold
        int thresholdSize = (int)glm::sqrt((mWidth * mWidth) + (mHeight * mHeight));
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
        mpThresholdItem->getShader()->fillValue("thresholdColor", thresholdColor);
        mpThresholdItem->getShader()->fillValue("threshold", mFirstThreshold.getValue());
        mpThresholdItem->getShader()->fillValue("alpha", alpha);
		mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
		mpThresholdItem->draw();

		// Pop scissor again
		popScissor();
	}

	void FutureKey::reset()
	{
        mFirstThreshold.setValue(0.f);
        mSecondThreshold.setValue(0.f);
        mDoingSecondThreshold = false;
        mLetterFading.setValue(0.f);
        mPressing.setValue(0.f);
        mRetriggerTime = 0;
        mGazeDistanze = 0;
	}

    void FutureKey::setCase(KeyboardCase keyCase)
    {
        if(!mIgnoreCase)
        {
            mKeyCase = keyCase;

            // Load correct letter
            std::u16string letter = mupLetter->getContent();
            switch(mKeyCase)
            {
            case KeyboardCase::LOWER:
                toLower(letter);
                mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, letter);
                break;
            case KeyboardCase::UPPER:
                toUpper(letter);
                mupLetter = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mLetterScale, letter);
                break;
            }

            // Call transform of letter
            transformLetter();
        }
    }

    std::u16string FutureKey::getLetter()
    {
        return mupLetter->getContent();
    }

    std::u16string FutureKey::getSuggestion()
    {
        if(mupSuggestion != NULL) { return mupSuggestion->getContent(); } else { return std::u16string(); };
    }

    void FutureKey::setSuggestion(std::u16string suggestion)
    {
        if(mShowSuggestion && mupSuggestion != nullptr) { mupSuggestion->setContent(suggestion); };
        transformSuggestion();
    }

	void FutureKey::clearSuggestion()
	{
		setSuggestion(u"");
	}

    void FutureKey::backToFirstThreshold()
    {
        mSecondThreshold.setValue(0.f);
        mDoingSecondThreshold = false;
    }

	bool FutureKey::suggestionShown() const
	{
		return mShowSuggestion;
	}

	bool FutureKey::atFirstThreshold() const
	{
		return !mDoingSecondThreshold;
	}

	void FutureKey::setInfo(std::u16string content)
	{
		mupInfo->setContent(content);
		transformInfo();
	}
	
    void FutureKey::transformLetter()
    {
        mupLetter->transform(); // has to be called first to calculate width and height
        int letterX = mX + ((mWidth - mupLetter->getWidth()) / 2);
        int letterY = mY + (int)(((((1.f - SUGGESTION_HEIGHT) * mHeight) - mupLetter->getHeight()) / 2.f) + (LETTER_Y_OFFSET * mHeight));
        mupLetter->setPosition(letterX, letterY);
    }

    void FutureKey::transformSuggestion()
    {
        if(mShowSuggestion)
        {
            mupSuggestion->transform(); // has to be called first to calculate width and height
            int suggestionX = mX + (mWidth - mupSuggestion->getWidth()) / 2;

            // Standard height of suggestion
            int suggestionStandardY = mY + (int)(mHeight * (1.f - SUGGESTION_HEIGHT)); // move to correct area
            suggestionStandardY += (int)(((SUGGESTION_HEIGHT * mHeight) - mupSuggestion->getHeight()) / 2.f); // center in area

            // Height of suggestion when in center of complete key
            int suggestionCenterY = mY + (mHeight - mupSuggestion->getHeight()) / 2;

            // Set position
            mupSuggestion->setPosition(suggestionX, glm::mix(suggestionStandardY, suggestionCenterY, mSecondThreshold.getValue()));

            // Animation update
            if(mSuggestionAnimation.first > 0 && mSuggestionAnimation.second != NULL)
            {
				int suggestionSuggestionX = mX + (mWidth - mSuggestionAnimation.second->getWidth()) / 2;
				int suggestionAnimationCenterY = mY + (mHeight - mSuggestionAnimation.second->getHeight()) / 2;
                mSuggestionAnimation.second->setPosition(suggestionSuggestionX, glm::mix(mY, suggestionAnimationCenterY, mSuggestionAnimation.first / SUGGESTION_ANIMATION_DURATION));
            }
        }
    }

	void FutureKey::transformInfo()
	{
		mupInfo->transform();
		int infoY = mY + (int)(mHeight * (1.f - SUGGESTION_HEIGHT)); // use suggestion position also for information
		mupInfo->setPosition(mX + ((mWidth - mupInfo->getWidth()) / 2), infoY);
	}
}
