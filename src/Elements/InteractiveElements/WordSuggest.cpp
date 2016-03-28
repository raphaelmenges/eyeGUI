//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "WordSuggest.h"

#include "src/Layout.h"
#include "src/Rendering/ScissorStack.h"
#include "src/Utilities/Helper.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
    WordSuggest::WordSuggest(
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
        bool adaptiveScaling,
        FontSize fontSize) : InteractiveElement(
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
        mType = Type::WORD_SUGGEST;

        // Fill members
        mFontSize = fontSize;
		mOffset.setValue(0.f);
		mStartOffset = 0;
		mDelta = 0;
		mCompleteWidth = 0;

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::COLOR,
            meshes::Type::QUAD);

		// Fetch single render item to draw all thresholds
		mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::BOX_THRESHOLD, meshes::Type::QUAD);
    }

    WordSuggest::~WordSuggest()
    {
        // Nothing to do
    }

    void WordSuggest::suggest(std::u16string input, Dictionary const * pDictionary)
    {
		// Clear up
		mSuggestions.clear();
		mThresholds.clear();

		// Only do something when there is input
		if (!input.empty())
		{
			// Decide whether word should start with big letter
			bool startsWithUpperCase = false;
			if (input[0] != toLower(input[0]))
			{
				startsWithUpperCase = true;
			}

			// Ask for suggestions
			std::vector<std::u16string> suggestions = pDictionary->similarWords(input, startsWithUpperCase);
			uint i = 0;
			for (const std::u16string& rSuggestion : suggestions)
			{
				mSuggestions.push_back(std::move(mpAssetManager->createTextSimple(mFontSize, 1, rSuggestion)));

				// Only add maximal allowed number of suggestions
				if (++i >= WORD_SUGGEST_MAX_SUGGESTIONS)
				{
					break;
				}
			}

			// Prepare thresholds
			mThresholds.resize(mSuggestions.size(), LerpValue(0));

			// Transform and position the suggestions initially
			transformSuggestions();
			positionSuggestions();
		}
    }

	void WordSuggest::clear()
	{
		mOffset.setValue(0.f);
		mSuggestions.clear();
	}

    float WordSuggest::specialUpdate(float tpf, Input* pInput)
    {
		// Super call
		float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

		// Check for penetration
		bool penetrated = penetratedByInput(pInput);
		int focusedWord = -1; // no word focused is indicated by -1
		if (penetrated)
		{
			// Update offset
			float gazeInElementHorizontal = ((float)(pInput->gazeX - mX)) / (float)mWidth;
			gazeInElementHorizontal -= 0.5f;
			gazeInElementHorizontal *= 2; // scale to [-1,1]
			mOffset.update((tpf * gazeInElementHorizontal) / WORD_SUGGEST_SCROLL_DURATION);

			// Tell it suggestions (no transform needed)
			positionSuggestions();

			// Decide whether word is focused (just linear search)
			int i = 0;
			for (const std::unique_ptr<TextSimple>& rSuggestion : mSuggestions)
			{
				// Uses real position, offset is already in suggestion
				int x = rSuggestion->getX();
				int width = rSuggestion->getWidth();
				if (pInput->gazeX >= x && pInput->gazeX < x + width)
				{
					focusedWord = i;
					break;
				}
				i++;
			}
		}

		// Update threshold (TODO: durations should be in config)
		int i = 0;
		for (LerpValue& rThreshold : mThresholds)
		{
			// Update threshold (TODO: Make it depending on length of word?)
			rThreshold.update(tpf, i != focusedWord);

			// Check for full threshold
			if (rThreshold.getValue() >= 1)
			{
				// Remember chosen suggestion
				mLastChosenSuggestion = mSuggestions[i]->getContent();

				// Inform listener after updating
				mpNotificationQueue->enqueue(getId(), NotificationType::WORD_SUGGEST_CHOSEN);

				// Reset that threshold
				rThreshold.setValue(0.f);

				// Break
				break;
			}

			i++;
		}

		return adaptiveScale;
    }

    void WordSuggest::specialDraw() const
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

		// Push scissor to prohibit text simple assets to draw outside of this element
		pushScissor(mX, mY, mWidth, mHeight);

        // Draw suggestions
        for(const std::unique_ptr<TextSimple>& rSuggestion : mSuggestions)
        {
            rSuggestion->draw(getStyle()->fontColor, mAlpha);
        }

		// Draw thresholds (should be same number as suggestions
		int count = (int)mSuggestions.size();
		for (int i = 0; i < count; i++)
		{
			// Get threshold for that suggestion
			float threshold = mThresholds[i].getValue();

			// Only draw necessary stuff
			if (threshold > 0)
			{
				// Calculate matrix
				glm::mat4 matrix = calculateDrawMatrix(
					mpLayout->getLayoutWidth(),
					mpLayout->getLayoutHeight(),
					mSuggestions[i]->getX() - (mDelta / 2),
					mY,
					mSuggestions[i]->getWidth() + mDelta,
					mHeight);

				// Draw threshold
				mpThresholdItem->bind();
				mpThresholdItem->getShader()->fillValue("matrix", matrix);
				mpThresholdItem->getShader()->fillValue("thresholdColor", getStyle()->thresholdColor);
				mpThresholdItem->getShader()->fillValue("threshold", threshold);
				mpThresholdItem->getShader()->fillValue("alpha", mAlpha);
				mpThresholdItem->getShader()->fillValue("orientation", 1.f); // vertical threshold
				mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
				mpThresholdItem->draw();
			}
		}

		popScissor();
    }

    void WordSuggest::specialTransformAndSize()
    {
		// First transform and then position the text simple assets
		transformSuggestions();
		positionSuggestions();
    }

    void WordSuggest::specialReset()
    {
		// Nothing so far that would make sense
    }

    void WordSuggest::specialInteract()
    {
        // TODO
    }

    void WordSuggest::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
		// Pipe notifications to notifier template including own data
		switch (notification)
		{
		case NotificationType::WORD_SUGGEST_CHOSEN:
		{
			// Notify listener method with UTF-16 string
			notifyListener(&WordSuggestListener::chosen, pLayout, getId(), mLastChosenSuggestion);

			// Convert suggestion value to UTF-8 string
			std::string lastChosenSuggestionValue8;
			convertUTF16ToUTF8(mLastChosenSuggestion, lastChosenSuggestionValue8);

			// Notify listener method with UTF-8 string
			notifyListener(&WordSuggestListener::chosen, pLayout, getId(), lastChosenSuggestionValue8);
			break;
		}
		default:
			throwWarning(
				OperationNotifier::Operation::BUG,
				"Word suggest got notification which is not thought for it.");
			break;
		}
    }

	void WordSuggest::transformSuggestions()
	{
		if (!mSuggestions.empty())
		{
			// Fetch size of space in font (somehow strange way, but who cares)
			auto spaceText = mpAssetManager->createTextSimple(mFontSize, 1, u" ");
			int space = spaceText->getWidth();

			// Go over suggestions and transform them
			mStartOffset = space;
			mDelta = 2 * space;
			mCompleteWidth = mStartOffset;
			for (const std::unique_ptr<TextSimple>& rSuggestion : mSuggestions)
			{
				// Transform suggestion to fit available space
				rSuggestion->transform();

				// Get width after transformation
				int width = rSuggestion->getWidth();

				// Remind width
				mCompleteWidth += width + mDelta;
			}
		}
	}

	void WordSuggest::positionSuggestions()
	{
		if (!mSuggestions.empty())
		{
			// Now set position
			int xOffset = mStartOffset - (int)(mOffset.getValue() * (float)glm::max(mCompleteWidth - mWidth, 0));
			for (const std::unique_ptr<TextSimple>& rSuggestion : mSuggestions)
			{
				// Get values
				int width = rSuggestion->getWidth();
				int height = rSuggestion->getHeight();

				// Set new position
				int x = mX + xOffset;
				int y = mY + (mHeight - height) / 2;
				rSuggestion->setPosition(x, y);

				// Remind offset
				xOffset += width + mDelta;
			}
		}
	}
}
