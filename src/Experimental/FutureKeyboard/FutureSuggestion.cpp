//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureSuggestion.h"

#include "src/Rendering/AssetManager.h"
#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Rendering/ScissorStack.h"

namespace eyegui
{
	FutureSuggestion::FutureSuggestion(
        Layout const * pLayout,
        AssetManager* pAssetManager,
        float fontScale)
	{
		// Initialize members
		mX = 0;
		mY = 0;
		mWidth = 0;
		mHeight = 0;
        mThreshold.setValue(0.f);
        mPressing.setValue(0.f);
        mRetriggerTime = 0;

		// Save members
		mpLayout = pLayout;
		mpAssetManager = pAssetManager;
        mFontScale = fontScale;

		// Fetch render items
        mpBackgroundItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
		mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_THRESHOLD, meshes::Type::QUAD);

        // Create suggestion text
		mupSuggestion = mpAssetManager->createTextSimple(FontSize::KEYBOARD, mFontScale, u"Hello World");
	}

	FutureSuggestion::~FutureSuggestion()
	{

	}

	void FutureSuggestion::transformAndSize(int x, int y, int width, int height)
	{
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;
        transformSuggestion();
	}

	bool FutureSuggestion::update(float tpf, Input const * pInput)
	{
        // Return value
		bool hit = false;

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
        }

        // Update threshold
		mThreshold.update(tpf, !penetrated || (mRetriggerTime > 0.f));
        if(mThreshold.getValue() >= 1.f)
        {
			mThreshold.setValue(0.f);
            hit = true;
            mPressing.setValue(1.f);
            mRetriggerTime = RETRIGGER_DELAY;
        }

        // Just update suggestion transformation each frame, inclusive the animation
        transformSuggestion();

        // Return value
        return hit;
	}

    void FutureSuggestion::draw(
		glm::vec4 backgroundColor,
		glm::vec4 fontColor,
		glm::vec4 thresholdColor,
		float alpha) const
	{
		// Push scissor to limit rendering to current key (especially threshold)
		pushScissor(mX, mY, mWidth, mHeight);

        // *** DRAW BACKGROUND ***
        mpBackgroundItem->bind();
        mpBackgroundItem->getShader()->fillValue(
            "matrix",
            calculateDrawMatrix(
                mpLayout->getLayoutWidth(),
                mpLayout->getLayoutHeight(),
                mX,
                mY,
                mWidth,
                mHeight));
        mpBackgroundItem->getShader()->fillValue("color", backgroundColor);
        mpBackgroundItem->getShader()->fillValue("alpha", alpha);
        mpBackgroundItem->draw();

        // *** DRAW SUGGESTION ***
        mupSuggestion->draw(fontColor, 1.f, false, 0, 0);

        // *** DRAW ANIMATION OF CHOSEN SUGGESTION ***
        if(mSuggestionAnimation.first > 0 && mSuggestionAnimation.second != NULL)
        {
            mSuggestionAnimation.second->draw(fontColor, (mSuggestionAnimation.first / SUGGESTION_ANIMATION_DURATION) * alpha, false, 0, 0);
        }
   
        // *** DRAW THRESHOLD ***

		// Calculate draw matrix for threshold
        int thresholdSize = (int)(glm::sqrt((mWidth * mWidth) + (mHeight * mHeight)));
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
        mpThresholdItem->getShader()->fillValue("threshold", mThreshold.getValue());
        mpThresholdItem->getShader()->fillValue("alpha", alpha);
		mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
		mpThresholdItem->draw();

		// Pop scissor again
		popScissor();
	}

	void FutureSuggestion::reset()
	{
		mThreshold.setValue(0.f);
        mPressing.setValue(0.f);
        mRetriggerTime = 0;
	}

    std::u16string FutureSuggestion::getSuggestion()
    {
        return mupSuggestion->getContent();
    }

    void FutureSuggestion::setSuggestion(std::u16string suggestion)
    {
        mupSuggestion->setContent(suggestion);
        transformSuggestion();
    }

	void FutureSuggestion::clearSuggestion()
	{
		setSuggestion(u"");
	}

    void FutureSuggestion::transformSuggestion()
    {
        mupSuggestion->transform(); // has to be called first to calculate width and height
        int suggestionX = mX + (mWidth - mupSuggestion->getWidth()) / 2;
        int suggestionY = mY + (mHeight - mupSuggestion->getHeight()) / 2;

        // Set position
        mupSuggestion->setPosition(suggestionX, suggestionY);

        // Animation update
        if(mSuggestionAnimation.first > 0 && mSuggestionAnimation.second != NULL)
        {
			// TODO: animation end depending on height of element
            mSuggestionAnimation.second->setPosition(suggestionX, glm::mix(suggestionY, suggestionY - mHeight, mSuggestionAnimation.first / SUGGESTION_ANIMATION_DURATION));
        }
    }
}
