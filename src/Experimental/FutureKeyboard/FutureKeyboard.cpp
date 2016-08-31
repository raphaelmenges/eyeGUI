// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureKeyboard.h"

#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"

#include <string>

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
		bool adaptiveScaling,
		Mode mode) : InteractiveElement(
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
        mMode = mode;
        mCurrentWord = u"";
        mCollectedWords = u"";
        mLastLetter = u"";

		// Initialize suggestion line
		mupSuggestionA = std::unique_ptr<FutureSuggestion>(new FutureSuggestion(mpLayout, mpAssetManager, 1.f));
		mupSuggestionB = std::unique_ptr<FutureSuggestion>(new FutureSuggestion(mpLayout, mpAssetManager, 1.f));
		mupSuggestionC = std::unique_ptr<FutureSuggestion>(new FutureSuggestion(mpLayout, mpAssetManager, 1.f));
		// TODO: handle those

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

        // For first letter, use upper case
        for(auto& rspKey : mKeyList)
        {
			// TODO: ok, one should use upper case in standard scenario but for now we need lower
            rspKey->setCase(KeyboardCase::LOWER);
        }

        // Diplay
        mupDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);
        updateDisplayAndSuggestions();

        // Pre display
        mupPreDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);
	}

	FutureKeyboard::~FutureKeyboard()
	{

	}

    void FutureKeyboard::setKeySuggestion(std::string keyId, std::u16string suggestion)
    {
        // Search for key
        for(auto& rspKey : mKeyList)
        {
            if(rspKey->getId() == keyId)
            {
                rspKey->setSuggestion(suggestion);
                break;
            }
        }
    }

	std::u16string FutureKeyboard::getContent() const
	{
		return buildContent();
	}

	void FutureKeyboard::nextSentence(std::u16string sentence)
	{
		// Setup display
		mCurrentWord.clear();
		mCollectedWords.clear();
		mupPreDisplay->setContent(sentence);

		// Setup keys
		for (auto& rspKey : mKeyList)
		{
			rspKey->clearSuggestion();
		}

		// Update display
		updateDisplayAndSuggestions();
	}

	float FutureKeyboard::specialUpdate(float tpf, Input* pInput)
	{
        // Prepare tasks
		enum class KeyTask { TOGGLE_CASE, CLEAR_SUGGESTION };
        std::vector<KeyTask> tasks;
        bool firstLetterOfSentence = false;
        bool keyHit = false;

		// Update suggestions
		mupSuggestionA->update(tpf, pInput);
		mupSuggestionB->update(tpf, pInput);
		mupSuggestionC->update(tpf, pInput);

        // Go over keys and update them
        for(auto& rspKey : mKeyList)
        {
            // Record whether hit
            FutureKey::HitType type = rspKey->update(tpf, pInput);

            // *** LETTERS ***
            if(type == FutureKey::HitType::LETTER
            && rspKey->getId() != "return"
            && rspKey->getId() != "backspace"
            && rspKey->getId() != "shift"
            && rspKey->getId() != "repeat"
            && rspKey->getId() != "space"
			&& rspKey->getId() != "dot")
            {
                // Seems to be standard letter, just add it to content
                mCurrentWord.append(rspKey->getLetter());
                keyHit = true;
                mLastLetter = rspKey->getLetter(); // remember last letter for repeating
                updateDisplayAndSuggestions();
            }

            // *** SUGGESTIONS ***
            if(type == FutureKey::HitType::SUGGESTION)
            {
				// Replace current word with suggestion and display it
                mCurrentWord = rspKey->getSuggestion();                

				// Clear all suggestions
				tasks.push_back(KeyTask::CLEAR_SUGGESTION);

				// Append space to collected and clear current word
				mCollectedWords.append(mCurrentWord + u" ");
				mCurrentWord = u"";
				updateDisplayAndSuggestions();
				keyHit = true;
            }

            // *** SPECIAL LETTERS ***

			// Shift
			if (type == FutureKey::HitType::LETTER && rspKey->getId() == "shift")
			{
				tasks.push_back(KeyTask::TOGGLE_CASE);
			}

            // Space
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "space")
            {
				// Since TextFlow is at the moment not capable to display more
				// than one space in a row, only allow one in a row
				std::u16string content = buildContent();
				char16_t compareValue = u' ';
				if ((!content.empty() && (content.back() != compareValue)) || content.empty())
				{
					// Clear all suggestions
					tasks.push_back(KeyTask::CLEAR_SUGGESTION);

					// Append space to content
					mCollectedWords.append(mCurrentWord + u" ");
					mCurrentWord = u"";
					updateDisplayAndSuggestions();
				}
            }

            // Dot
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "dot")
            {
                mCollectedWords.append(mCurrentWord + u". ");
                mCurrentWord = u"";
                firstLetterOfSentence = true;
                updateDisplayAndSuggestions();
            }

            // Backspace
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "backspace")
			{
                // Try to delete something from current word
                if(!mCurrentWord.empty())
                {
					mCurrentWord = mCurrentWord.substr(0, mCurrentWord.size() - 1);
                }
                else if(!mCollectedWords.empty())
                {
					mCollectedWords = mCollectedWords.substr(0, mCollectedWords.size() - 1);

					// Try to get last word from collected words and set it as current word
					if (!mCollectedWords.empty())
					{
						// Go through words in collected
						std::u16string delimiter = u" ";
						size_t pos = 0;
						std::u16string copyCollectedWords = mCollectedWords;
						mCollectedWords = u"";
						while ((pos = copyCollectedWords.find(delimiter)) != std::u16string::npos)
						{
							mCollectedWords.append(copyCollectedWords.substr(0, pos) + u" "); // would have probably problems with mutliple spaces
							copyCollectedWords.erase(0, pos + delimiter.length());
						}
						mCurrentWord = copyCollectedWords;
					}
                }

                // Handle upper case at first letter of sentence
                if(mCollectedWords.empty())
                {
                    firstLetterOfSentence = true;
                }

                updateDisplayAndSuggestions();
            }

            // Repeat
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "repeat")
            {
                std::u16string lowerLastLetter = mLastLetter;
                toLower(lowerLastLetter);
                mCurrentWord.append(lowerLastLetter);
                updateDisplayAndSuggestions();
            }

            // *** RESET SECOND THRESHOLD OF ALL KEYS BUT CURRENT ONE ***
            if(type != FutureKey::HitType::NONE)
            {
                for(auto& rspOtherKey : mKeyList)
                {
                    if(rspOtherKey->getId() != rspKey->getId())
                    {
                        rspOtherKey->backToFirstThreshold();
                    }
                }
            }
        }

		// When some key was hit, return to lower case
		if (keyHit)
		{
			for (auto& rspKey : mKeyList)
			{
				rspKey->setCase(KeyboardCase::LOWER);
			}
		}

        // Execute tasks after updating all keys
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
				case KeyTask::CLEAR_SUGGESTION:
					for (auto& rspKey : mKeyList)
					{
						rspKey->clearSuggestion();
					}
					break;
            }
        }

		// Update last letter on repeat key
		mspRepeatKey->setInfo(mLastLetter);

		// Some information should be only in new mode available
		if (mMode == Mode::SUGGESTION_PER_KEY)
		{
			// Update word on space key
			mspSpaceKey->setInfo(mCurrentWord);
		}

		return 0.f;
	}

	void FutureKeyboard::specialDraw() const
	{
		// *** SUGGESTIONS ***
		mupSuggestionA->draw(
			getStyle()->color,
			getStyle()->fontColor,
			getStyle()->thresholdColor,
			getMultipliedDimmedAlpha());
		mupSuggestionB->draw(
			getStyle()->color,
			getStyle()->fontColor,
			getStyle()->thresholdColor,
			getMultipliedDimmedAlpha());
		mupSuggestionC->draw(
			getStyle()->color,
			getStyle()->fontColor,
			getStyle()->thresholdColor,
			getMultipliedDimmedAlpha());

		// *** KEYS ***
        for(auto& rspKey : mKeyList)
        {
            rspKey->draw(
                getStyle()->color,
                getStyle()->fontColor,
                getStyle()->backgroundColor,
                getStyle()->thresholdColor,
                getMultipliedDimmedAlpha());
        }

        // *** DISPLAY ***
        mupPreDisplay->draw(glm::vec4(0.4f,0.4f,0.4f,1), getMultipliedDimmedAlpha());;
        mupDisplay->draw(glm::vec4(1,1,1,1), getMultipliedDimmedAlpha());
	}

	void FutureKeyboard::specialTransformAndSize()
	{
		int xOffset = (int)(0.01f * mWidth);
		int suggestionWidth = (int)((mWidth - (2.f * xOffset)) / 3.f);
		int keyOffsetY = (int)(0.4f * mHeight);
		int keyWidth = (int)(0.09f * mWidth);
		int keyHeight = (int)(0.125f * mHeight);
		int keySpace = (int)(0.01f * mWidth);

		// Display
		mupDisplay->transformAndSize((int)(mX + 0.05f * mWidth), (int)(mY + 0.05f * mHeight), mWidth, (int)(0.2f * mHeight));

		// Pre display
		mupPreDisplay->transformAndSize((int)(mX + 0.05f * mWidth), (int)(mY + 0.05f * mHeight), mWidth, (int)(0.2f * mHeight));

		// Transform and size suggestions
		mupSuggestionA->transformAndSize(xOffset + mX, (int)(0.3f * mHeight) + mY - keySpace, suggestionWidth, (int)(0.1f * mHeight));
		mupSuggestionB->transformAndSize(xOffset + mX + suggestionWidth, (int)(0.3f * mHeight) + mY - keySpace, suggestionWidth, (int)(0.1f * mHeight));
		mupSuggestionC->transformAndSize(xOffset + mX + (2 * suggestionWidth), (int)(0.3f * mHeight) + mY - keySpace, suggestionWidth, (int)(0.1f * mHeight));

		// Transform and size keys

        // First row
        mspQKey->transformAndSize(xOffset + mX,                               keyOffsetY + mY, keyWidth, keyHeight);
        mspWKey->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspEKey->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspRKey->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspTKey->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspYKey->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspUKey->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspIKey->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspOKey->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspPKey->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);

        // Second row
        mspAKey    ->transformAndSize(xOffset + mX,                               keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspSKey    ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspDKey    ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspFKey    ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspGKey    ->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspHKey    ->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspJKey    ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspKKey    ->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspLKey    ->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspEnterKey->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);

        // Third row
        mspZKey        ->transformAndSize(xOffset + mX,                               keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspXKey        ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCKey        ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspVKey        ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBKey        ->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspNKey        ->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspMKey        ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBackspaceKey->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);

        // Forth row
        mspShiftKey      ->transformAndSize(xOffset + mX,                               keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspRepeatKey     ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCommaKey      ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspSpaceKey      ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);
        mspDotKey        ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspQuestionKey   ->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspExclamationKey->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspColonKey      ->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
	}

	void FutureKeyboard::specialReset()
	{
		// Go over keys and reset them
		for (auto& rspKey : mKeyList)
		{
			rspKey->reset();
		}

		// Reset last letter
		mLastLetter = u"";
		mspRepeatKey->setInfo(mLastLetter);
	}

	void FutureKeyboard::specialInteract()
	{

	}

	void FutureKeyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
	{
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::FUTURE_KEY_NEEDS_SUGGESTION:
        {
            // Go over all keys which need suggestions and ask for one
            for(auto& rspKey : mKeyList)
            {
                // Only do so for keys which want a suggestion
				if(rspKey->suggestionShown() && rspKey->atFirstThreshold())
                {
                    notifyListener(
                        &eyegui_experimental::FutureKeyboardListener::keySuggestion,
                        pLayout,
                        getId(),
                        rspKey->getId(),
                        rspKey->getLetter());
                }
            }

            break;
        }
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Future keyboard got notification which is not thought for it.");
            break;
        }
	}

    void FutureKeyboard::updateDisplayAndSuggestions()
    {
        // *** DISPLAY ***
        mupDisplay->setContent(buildContent() + u"_");

        // *** SUGGESTIONS ***
		// TODO: suggestions in line
        mpNotificationQueue->enqueue(getId(), NotificationType::FUTURE_KEY_NEEDS_SUGGESTION);
    }

	std::u16string FutureKeyboard::buildContent() const
	{
		return mCollectedWords + mCurrentWord;
	}
}